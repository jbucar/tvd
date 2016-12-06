/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "provider.h"
#include "frontend.h"
#include "timer.h"
#include "ts/provider.h"
#include "generated/config.h"
#if DVB_USE_DVB
#include	"broadcast/dvb/provider.h"
#endif
#if DVB_USE_BDA
#include	"broadcast/bda/provider.h"
#endif
#include "filter.h"
#include "../demuxer/psi/psidemuxer.h"
#include "../demuxer/psi/psi.h"
#include "../demuxer/psi/cache.h"
#include "../demuxer/pes/pesdemuxer.h"
#include <util/task/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <util/pool/circularpool.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/functions.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#if DVB_USE_DVB
#	define DEFAULT_PROVIDER "dvb"
#elif DVB_USE_BDA
#	define DEFAULT_PROVIDER "bda"
#else
#	define DEFAULT_PROVIDER "tsdata"
#endif

namespace tuner {

REGISTER_INIT_CONFIG( tuner ) {
	root().addNode( "tuner" ).addNode( "provider" )
		.addValue( "cache", "Use PSI cache", false )
		.addValue( "statusPoll", "Poll tuner status each N ms", 500 )
		.addValue( "lockTimeout", "Lock tuner timeout, in ms", 2000 )
		.addValue( "lockLatency", "Lock tuner latency, in ms", 0 )
		.addValue( "use", "Provider to initialize", std::string(DEFAULT_PROVIDER) );
}

template<typename T>
struct FilterFinder {
	ID _pid;
	FilterFinder( ID pid ) : _pid( pid ) {}
	bool operator()( const T filter ) const {
		return filter->pid() == _pid;
	}
};

template<typename T>
	inline typename std::list<T>::iterator findByPid( std::list<T> &list, ID pid ) {
	return std::find_if( list.begin(), list.end(), FilterFinder<T>(pid) );
}

template<typename T>
struct RemoveSections {
	RemoveSections( ID pid ) : _pid(pid) {}
	bool operator()( const T &item ) const {
		return _pid == TS_PID_NULL || item.pid == _pid;
	}

	ID _pid;
};

/**
 * Crea una instancia de @c Provider, utilizando la implementación registrada en el árbol de configuración (ver @b dtv-util).
 * @return Una nueva instancia de @c Provider.
 */
Provider* Provider::create() {
	Provider *provider=NULL;

	const std::string &use = util::cfg::getValue<std::string>("tuner.provider.use");
	LINFO("Provider", "Using provider: %s", use.c_str() );
	if (use == "tsdata") {
		provider = new ts::Provider();
	}
#if DVB_USE_DVB
	else if (use == "dvb") {
		provider = new dvb::Provider();
	}
#endif
#if DVB_USE_BDA
	else if (use == "bda") {
		provider = new bda::Provider();
	}
#endif
	else {
		return NULL;
	}

	return provider;
}

namespace mask {
	enum type { exit=0x01, timer=0x02 };
}

/**
 * Constructor base.
 */
Provider::Provider()
{
	_frontend   = NULL;
	_current    = size_t(-1);
	_state      = impl::state::off;
	_pool       = NULL;
	_cache      = NULL;
	_disp       = NULL;
	memset(&_st,0,sizeof(_st));
}

/**
 * Desctructor base.
 */
Provider::~Provider()
{
	DTV_ASSERT(_state < impl::state::idle);
}

std::string Provider::name() const {
	return "Generic";
}

/**
 * Crea el Frontend y comienza a leer datos desde el TS utilizando un buffer de tamaño @b networkBlocks.
 * @param networkBlocks El tamaño de buffer a utilizar.
 * @return true si pudo comenzar la lectura de datos correctamente, false en caso contrario.
 */
bool Provider::start( util::DWORD networkBlocks/*=1000*/ ) {
	LDEBUG( "Provider", "Start" );

	DTV_ASSERT( _state == impl::state::off );

	//	Setup variables
	_current = size_t(-1);
	state( impl::state::fail );

	//	Register into dispatcher
	if (!_disp) {
		LERROR( "Provider", "Dispatcher empty" );
		return false;
	}

	//	Create network resource pool
	_pool = new SectionsPool( "provider", networkBlocks, TSS_PRI_MAX_BYTES );
	_pool->enable(false);

	//	Create sections cache
	if (util::cfg::getValue<bool>("tuner.provider.cache")) {
		_cache = new psi::Cache( _pool );
		if (!_cache->load()) {
			LERROR( "Provider", "Cannot start engine filter" );
			return false;
		}
	}

	//	Create and initialize frontend
	_frontend = createFrontend();
	if (!_frontend || !_frontend->initialize()) {
		LERROR( "Provider", "Cannot start tuner" );
		return false;
	}

	//	Start filter engine
	if (!startEngineFilter()) {
		LERROR( "Provider", "Cannot start engine filter" );
		return false;
	}

	//	Start demuxer thread
	_thread = boost::thread( boost::bind( &Provider::providerThread, this ) );

	//	Setup internal state to idle
	state( impl::state::idle );

	return true;
}

/**
 * Finaliza la lectura de datos desde el TS y destruye el frontend.
 */
void Provider::stop() {
	if (_state >= impl::state::idle) {
		LDEBUG( "Provider", "Stop" );

		//	Unregister in dispatcher
		_disp->unregisterTarget( this );

		//	Stop current network
		stopCurrent();

		//	Wait section demuxer thread
		_pool->notify_all( mask::exit );
		_thread.join();
		LDEBUG( "Provider", "Join to thread successfully" );

		//	Stop lost filters
		_mFilters.lock();
		BOOST_FOREACH( Filter *filter, _filters ) {
			LWARN( "Provider", "Filter lost: pid=%04x", filter->pid() );
			stopFilter( filter, true );
		}
		_filters.clear();
		_mFilters.unlock();

		//	Clear timers
		_mTimers.lock();
		CLEAN_ALL( impl::Timer *, _timers );
		_mTimers.unlock();

		//	Stop filter engine
		stopEngineFilter();

		//	Stop tuner
		_frontend->finalize();
		DEL(_frontend);

		//	Store cache
		if (_cache) {
			_cache->unload();
			DEL(_cache)
		}

		DEL(_pool);
	}

	//	Set state always to off
	state( impl::state::off );
}

int Provider::statusPollTimeout() const {
	return util::cfg::getValue<int>("tuner.provider.statusPoll");
}

int Provider::lockTimeout() const {
	return util::cfg::getValue<int>("tuner.provider.lockTimeout");
}

int Provider::lockLatency() const {
	return util::cfg::getValue<int>("tuner.provider.lockLatency");
}

void Provider::state( impl::state::type cur ) {
	//	Process change of state
	if (_state != cur) {
		LDEBUG( "Provider", "Changing state: old=%02x, new=%02x", _state, cur );
		status::Type st;
		memset(&st,0,sizeof(st));

		if (_state <= impl::state::idle && cur > impl::state::idle) {
			//	idle -> tuned
			_pool->enable(true);
			updateStatus(st,true,false);
			addTimer( TS_PID_NULL, statusPollTimeout() );
		}
		else if (_state > impl::state::idle && cur <= impl::state::idle) {
			//	tuned -> idle
			_pool->enable(false);
			rmTimer( TS_PID_NULL );
			updateStatus(st,false,false);
		}

		_state = cur;
	}
}

impl::state::type Provider::getState() const {
	return _state;
}

/**
 * Devuelve el estado del frontend
 */
void Provider::status( status::Type &st ) const {
	st = _st;
}

/**
 * @return Retorna true si el tuner se encuentra sintonizando una señal valida.
 */
bool Provider::isLocked() const {
	DTV_ASSERT(_frontend);
	return _st.isLocked;
}

void Provider::updateStatus( const status::Type &st, bool start, bool post/*=false*/ ) {
	//	Update change lock timer!
	if (start) {
		int latency = lockLatency();
		if (latency) {
			_lockLatency = impl::pt::microsec_clock::local_time() + impl::pt::milliseconds(latency);
			_lockTimer = impl::pt::pos_infin;
		}
		else {
			_lockLatency = impl::pt::pos_infin;
			_lockTimer = impl::pt::microsec_clock::local_time() + impl::pt::milliseconds(lockTimeout());
		}
	}

	if (st.isLocked != _st.isLocked) {
		LDEBUG( "Provider", "Lock state changed: old=%d, cur=%d", _st.isLocked, st.isLocked );

		_st.isLocked = st.isLocked;
		if (!start) {
			_lockTimer = impl::pt::pos_infin;
		}

		//	Notify
		if (!_onLockChanged.empty()) {
			if (post) {
				_disp->post( this, _onLockChanged );
			}
			else {
				_onLockChanged();
			}
		}
	}

	//	TODO: Use atomic operations? Data race ....
	if (st.isLocked) {
		_st.snr = st.snr;
		_st.signal = st.signal;
	}
	else {
		_st.snr = 0;
		_st.signal = 0;
	}
}

void Provider::checkStatus() {
	if (!_lockLatency.is_special()) {
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();
		if (now > _lockLatency) {
			_lockLatency = impl::pt::pos_infin;
			_lockTimer = impl::pt::microsec_clock::local_time() + impl::pt::milliseconds(lockTimeout());
		}
	}

	if (_lockLatency.is_special()) {
		//	Check status
		status::Type st;
		memset(&st,0,sizeof(st));
		bool res = _frontend->getStatus( st );
		if (res) {
			updateStatus(st,false,true);
		}

		LTRACE( "Provider", "Check status begin: res=%d, locked=%d, isSpecial=%d",
			res, st.isLocked, _lockTimer.is_special() );
		if ((!res || !st.isLocked) && !_lockTimer.is_special()) {
			impl::pt::ptime now = impl::pt::microsec_clock::local_time();
			if (now > _lockTimer) {
				LINFO( "Provider", "Set network timeout!" );
				_lockTimer = impl::pt::pos_infin;
				if (!_onLockTimeout.empty()) {
					_disp->post( this, _onLockTimeout );
				}
			}
		}

		LTRACE( "Provider", "Check status end: res=%d, isLocked=%d", res, st.isLocked );
	}
}

/**
 * Crea un filtro a partir de @b demux y lo ejecuta, filtrando así las secciones correspondientes.
 * @param demux Un PSIDemuxer
 * @return true si pudo crear y ejecutar el filtro correctamente, false en caso contrario.
 */
bool Provider::startFilter( PSIDemuxer *demux ) {
 	DTV_ASSERT(demux);

	LDEBUG( "Provider", "Start section filter: demux=%p, pid=%04x, timeout=%d", demux, demux->pid(), demux->timeout() );

	//	Check if can alloc a new filter
	bool result = canAllocFilter( true, demux->pid() );
	if (result) {
		//	Create filter
		Filter *filter = createFilter( demux );
		if (filter) {
			result=startFilter( filter );
			if (result) {
				//	Setup demuxer
				demux->setDispatcher( boost::bind(&Provider::onNotification,this,_1,_2) );
				if (_cache) {
					demux->setCache( _cache );
				}

				_mDemuxers.lock();

				//	Add demux to list of demuxers
				_demuxers.push_back( demux );

				//	Try push PSI section from cache
				if (_cache && demux->supportCache()) {
					util::Buffer *buf= _cache->get( demux->pid() );
					if (buf) {
						enqueue( demux->pid(), buf );
					}
				}

				_mDemuxers.unlock();
			}
		}
		else {
			LWARN( "Provider", "Cannot create filter: pid=%04x", demux->pid() );
			result=false;
		}
	}

	if (!result) {
		delete demux;
	}

	return result;
}

/**
 * Crea un filtro segun los parametros indicados en @b params.
 * @param params Indica el id, tipo y modo del flujo de datos, y el pipe para enviar los datos filtrados.
 * @return true si pudo crear y ejecutar el filtro correctamente, false en caso contrario.
 */
bool Provider::startFilter( pes::FilterParams *params ) {
	LDEBUG( "Provider", "Start stream filter: pid=%04x, pes::type=%d, pes::mode=%d, pipe=%p",
		params->pid, params->type, params->mode, params->pipe );

	//	Check if can alloc a new filter
	bool result = canAllocFilter( false, params->pid );
	if (result) {
		//	Create filter
		Filter *filter = createFilter( params );
		if (filter) {
			result=startFilter( filter );
		}
		else {
			LWARN( "Provider", "Cannot create filter: pid=%04x", params->pid );
			result=false;
		}
	}

	return result;
}

/**
 * Finaliza el filtro identificado por @b pid y si @b destroy es true, destruye el mismo.
 * @param pid El id del filtro.
 * @param destroy Indica si se debe destruir el filtro.
 */
void Provider::stopFilter( ID pid, bool destroy/*=true*/ ) {
	if (pid != TS_PID_NULL) {
		//	Try remove from list
		Filter *filter=removeFilter( pid );
		if (filter) {
			LDEBUG( "Provider", "Stop filter: pid=%04x", pid );
			stopFilter( filter, destroy );
		}
	}
}

int Provider::maxFilters() const {
	return 100;
}

bool Provider::haveTimers() const {
	return false;
}

//	Aux filters engine
bool Provider::startEngineFilter() {
	LDEBUG( "Provider", "Start engine filter" );
	return true;
}

void Provider::stopEngineFilter() {
	LDEBUG( "Provider", "Stop engine filter" );
}

bool Provider::checkCRC() const {
	return true;
}

/**
 * @return La cantidad de redes a escanear cuando se buscan canales.
 */
size_t Provider::getNetworksCount() const {
	DTV_ASSERT(_frontend);
	return _frontend->getCount();
}

/**
 * @param index El indice de la red.
 * @return El nombre de la red en la posición indicada por @b nIndex
 */
std::string Provider::getNetworkName( size_t index ) const {
	DTV_ASSERT(_frontend);
	return _frontend->getName( index );
}

/**
 * @param name El nombre de la red
 * @return El índice de la red llamada @b name.
 */
size_t Provider::findNetwork( const std::string &name ) const {
	DTV_ASSERT(_frontend);
	return _frontend->find( name );
}

/**
 * @return El indice de la red actual.
 */
size_t Provider::currentNetwork() const {
	DTV_ASSERT(_frontend);
	return _current;
}

/**
 * Setea como red actual a la red indicada por @b nIndex.
 * @param index Índice de la red a setear.
 * @return true si pudo setear la red correctamente, false en caso contrario.
 */
bool Provider::setNetwork( size_t index ) {
	DTV_ASSERT(_frontend);

	if (index == _current) {
		return true;
	}

	bool result=false;
	if (_state >= impl::state::idle) {
		impl::state::type st = impl::state::idle;
		LINFO( "Provider", "Set network: index=%d, name=%s",
			index, getNetworkName(index).c_str() );

		//	Stop current network
		stopCurrent();

		//	Tune network
		result=_frontend->start( index );
		if (result) {
			//	Initialize cache
			if (_cache) {
				_cache->setNetwork( index );
			}

			//	Save state
			st = impl::state::tuned;

			//	Set current network
			_current = index;
		}

		//	Set state
		state( st );
	}

	return result;
}

void Provider::stopNetwork() {
}

void Provider::stopCurrent() {
	if (_state > impl::state::idle) {
		LDEBUG( "Provider", "Stop current network: current=%d", _current );

		//	Set state (no accept more sections or tasks)
		state( impl::state::idle );

		//	Stop frontend
		_frontend->stop();

		//	Stop network
		stopNetwork();

		//	Clean all data of sections
		clear( TS_PID_NULL );

		//	Reset current network
		_current = size_t(-1);
	}
}

void Provider::onLockChanged( const OnLockChanged &callback ) {
	DTV_ASSERT(!_frontend);
	_onLockChanged = callback;
}

void Provider::onLockTimeout( const OnLockTimeout &callback ) {
	DTV_ASSERT(!_frontend);
	_onLockTimeout = callback;
}

bool Provider::startFilter( Filter *filter ) {
	bool result=true;

	//	Initialize filter
	if (!filter->initialized()) {
		result=filter->initialize();
	}

	if (result) {
		//	Start filter
		result=filter->start();
		if (result) {
			//	Add to list of filters
			_mFilters.lock();
			_filters.push_back( filter );
			_mFilters.unlock();

			//	Add timer if filter not support it!
			if (!haveTimers() && filter->timeout()) {
				addTimer( filter->pid(), filter->timeout() );
			}
		}
		else {
			filter->deinitialize();
			LWARN( "Provider", "Filter cannot be started: pid=%04x", filter->pid() );
		}
	}
	else {
		LWARN( "Provider", "Filter initialization fail: filter=%04x", filter->pid() );
	}

	return result;
}

void Provider::stopFilter( Filter *filter, bool destroy ) {
	DTV_ASSERT(filter);

	ID pid = filter->pid();

	//	Stop filter
	filter->stop();

	//	Remove demuxer
	_mDemuxers.lock();
	DemuxerList::iterator it = findByPid(_demuxers,pid);
	if (it != _demuxers.end()) {
		//	Remove from list
		delete (*it);
		_demuxers.erase( it );
	}
	_mDemuxers.unlock();

	//	Remove timer if necesary
	if (!haveTimers()) {
		rmTimer( pid );
	}

	//	Cleanup data from filter && demuxer
	clear( pid );

	//	Destroy filter
	if (destroy) {
		filter->deinitialize();
		delete filter;
	}
}

void Provider::restartFilter( ID pid ) {
	LDEBUG( "Provider", "Restart filter: pid=%04x", pid );

	_mFilters.lock();
	FilterList::iterator it = findByPid(_filters,pid);
	if (it != _filters.end()) {
		//	Get filter
		(*it)->restart();
	}
	_mFilters.unlock();
}

Filter *Provider::removeFilter( ID pid ) {
	Filter *filter=NULL;
	//LDEBUG( "Provider", "Remove filter: pid=%04x", pid );

	_mFilters.lock();
	FilterList::iterator it = findByPid(_filters,pid);
	if (it != _filters.end()) {
		//	Get filter
		filter = (*it);

		//	Remove from list
		_filters.erase( it );
	}
	_mFilters.unlock();

	return filter;
}

bool Provider::canAllocFilter( bool isSection, ID pid ) {
	bool result;
	bool maxAllocated=true;

	_mFilters.lock();
	result = isSection ? (_filters.size() < static_cast<size_t>(maxFilters())) : true;
	if (result) {
		maxAllocated=false;
		FilterList::const_iterator it = findByPid( _filters, pid );
		result = (it == _filters.end());
	}
	_mFilters.unlock();

	if (!result) {
		std::string err="Cannot alloc a new filter. ";
		err += maxAllocated ? "Maximum already allocated" : "PID already filtrated";
		err += ": pid=%04x";
		LWARN( "Provider", err.c_str(), pid );
	}

	return result;
}

void Provider::timeout( ID pid ) {
	LTRACE( "Provider", "Timer expired: pid=%04x", pid );
	enqueue( pid, NULL );
}

void Provider::enqueue( ID pid, util::BYTE *data, util::DWORD size ) {
	DTV_ASSERT(data);
	util::Buffer *buf = getNetworkBuffer();
	buf->copy( data, size );
	enqueue( pid, buf );
}

void Provider::enqueue( ID pid, util::Buffer *buf ) {
	//	Enqueue filter data into provider thread
	impl::SectionBuffer data;
	data.pid = pid;
	data.buf = buf;
	_pool->put( data );
	//LDEBUG( "Provider", "Enqueue section: pid=%04x", pid );
}

void Provider::clear( ID pid ) {
	//	Clean sections from pid
	_pool->remove_if( RemoveSections<impl::SectionBuffer>(pid) );

	//	Clean notifications from pid
	_mNotifications.lock();
	util::erase_if( _notifications, RemoveSections<impl::SectionNotification>(pid) );
	_mNotifications.unlock();
}

void Provider::processSectionBuffer( PSIDemuxer *demux, util::Buffer *buf ) {
	demux->process( buf );
}

void Provider::processSection( impl::SectionBuffer &data ) {
	//	Process section buffer if demuxer exists
	_mDemuxers.lock();
	DemuxerList::const_iterator it = findByPid( _demuxers, data.pid );
	if (it != _demuxers.end()) {
		processSectionBuffer( (*it), data.buf );
	}
	_mDemuxers.unlock();

	//	Free buffer
	if (data.buf) {
		_pool->free( data.buf );
	}
}

/**
 * Si existe un demuxer activo con identificador @b pid, ejecuta la función indicada en @b callback.
 * @param pid El identificador del Demuxer.
 * @param callback El callback a ejecutar.
 */
void Provider::onNotification( ID pid, const boost::function<void (void)> &callback ) {
	if (!haveTimers()) {
		resetTimer(pid);
	}

	_mNotifications.lock();
	impl::SectionNotification data;
	data.callback = callback;
	data.pid = pid;
	_notifications.push_back( data );
	_mNotifications.unlock();
	_disp->post( this, boost::bind(&Provider::postNotification,this) );
}

void Provider::postNotification() {
	impl::SectionNotification evt;
	bool exit=false;

	while (!exit) {
		//	Get element from list
		_mNotifications.lock();
		if (_notifications.empty()) {
			exit=true;
		}
		else {
			evt = _notifications.front();
			_notifications.pop_front();
		}
		_mNotifications.unlock();

		if (!exit) {
			evt.callback();
		}
	}
}

/**
 * Setea @b taskDispatcher como despachador de tareas del provider.
 * @param disp El despachador de tareas a setear.
 */
void Provider::setDispatcher( util::task::Dispatcher *disp ) {
	_disp = disp;
	_disp->registerTarget( this, "tuner::Provider" );
}

/**
 * @return El despachador de tareas usado por el provider.
 */
util::task::Dispatcher *Provider::dispatcher() const {
	return _disp;
}

//	Aux timers
void Provider::addTimer( ID pid, int ms ) {
	LTRACE( "Provider", "Add timer: pid=%04x", pid );
	_mTimers.lock();
	_timers.push_back( new impl::Timer( pid, ms ) );
	_mTimers.unlock();

	//	Notify
	_pool->notify_all( mask::timer );
}

void Provider::rmTimer( ID pid ) {
	LTRACE( "Provider", "Remove timer: pid=%04x", pid );
	_mTimers.lock();
	TimerList::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		boost::bind(&impl::Timer::pid,_1) == pid );
	if (it != _timers.end()) {
		delete (*it);
		_timers.erase( it );
	}
	_mTimers.unlock();

	//	Notify
	_pool->notify_all( mask::timer );
}

void Provider::resetTimer( ID pid ) {
	_mTimers.lock_shared();
	TimerList::const_iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		boost::bind(&impl::Timer::pid,_1) == pid );
	if (it != _timers.end()) {
		LTRACE( "Provider", "Reset timer: pid=%04x", pid );
		(*it)->reset();
	}
	_mTimers.unlock_shared();
}

int Provider::checkTimers() {
	int min = std::numeric_limits<int>::max();

	if (!_timers.empty()) {
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();

		//	Consume elapsed time, if any expired add to post processing
		_mTimers.lock_shared();
		BOOST_FOREACH( impl::Timer *timer, _timers ) {
			if (timer->consume( now )) {
				ID pid = timer->pid();

				if (pid != TS_PID_NULL) {
					timeout( pid );
					timer->reset();
				}
				else {
					checkStatus();
				}
			}
			if (timer->timer() < min) {
				min = timer->timer();
			}
		}
		_mTimers.unlock_shared();
	}

	if (min == std::numeric_limits<int>::max()) {
		min = -1;
	}

	LTRACE( "Provider", "Check timers end: ms=%d", min );

	return min;
}

//	Aux network buffer
util::Buffer *Provider::getNetworkBuffer() {
	return _pool->alloc();
}

void Provider::freeNetworkBuffer( util::Buffer *buf ) {
	_pool->free( buf );
}

//	Aux thread
void Provider::providerThread() {
	impl::SectionBuffer item;
	util::DWORD mask;
	bool exit=false;
	int ms;

	LINFO( "Provider", "Begin provider thread" );

	while (!exit) {
		//	Check timers and get minimum to wait
		ms = checkTimers();

		//	Get section
		if (_pool->get( item, mask, ms )) {
			processSection( item );
		}

		if (mask & mask::exit) {
			exit = true;
		}
	}

	LINFO( "Provider", "End provider thread" );
}

}

