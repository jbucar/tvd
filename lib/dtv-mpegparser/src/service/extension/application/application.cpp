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
#include "application.h"
#include "extension.h"
#include "controller/controller.h"
#include "controller/runtask.h"
#include "../../servicemanager.h"
#include "../../../resourcemanager.h"
#include "../../../demuxer/psi/dsmcc/event.h"
#include "../../../demuxer/psi/dsmcc/dsmccdemuxer.h"
#include "../../../demuxer/psi/dsmcc/streameventdemuxer.h"
#include "../../../demuxer/psi/dsmcc/objectcarouselfilter.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>


namespace tuner {
namespace app {

namespace fs = boost::filesystem;

struct EventFinder {
	EventFinder( const std::string &url, const std::string &name, WORD &eventID )
		: _url(url), _name(name), _eventID(eventID) {}
	bool operator()( dsmcc::Event *evt ) {
		if (evt->url() == _url && evt->includes( _name, _eventID )) {
			return true;
		}
		return false;
	}
	const std::string &_url;
	const std::string &_name;
	WORD &_eventID;

private:
	EventFinder &operator=( EventFinder &/*ef*/ ) { return *this; }
};

Application::Application( Extension *ext, const ApplicationID &id )
	: _appID ( id )
{
	DTV_ASSERT(ext);
	_extension    = ext;
	_autoStart    = false;
	_autoDownload = false;
	_readOnly     = true;
	_service      = 0;
	_visibility   = visibility::none;
	_priority     = 0;
	_componentTag = 0xFF;
	_status       = status::unmounted;
	_runTask      = NULL;
}

Application::Application( Extension *ext, const ApplicationID &id, const std::string &path )
	: _appID( id )
{
	//	Filesystem
	_extension    = ext;
	_path         = path;
	_autoStart    = false;
	_autoDownload = false;
	_readOnly     = true;
	_service      = 0;
	_visibility   = visibility::both;
	_priority     = 0;
	_componentTag = 0xFF;
	_status       = status::mounted;
	_runTask      = NULL;
}

Application::~Application()
{
	DTV_ASSERT( _runTask == NULL );
	DTV_ASSERT( _downloads.size() == 0 );
	DTV_ASSERT( _streams.size() == 0 );
	DTV_ASSERT( _streamCallbacks.size() == 0 );
	BOOST_FOREACH( dsmcc::Event *ev, _events ) {
		delete ev;
	}
}

//	Operators
bool Application::operator==( const ApplicationID &id ) {
	return _appID == id;
}

//	Start/Stop
bool Application::start() {
	LINFO("app::Application", "Start application: %s, status=%d", name().c_str(), status() );

	bool result=false;
	switch (status()) {
		case status::unmounted: {
			//	Force start
			autoStart( true );

			//	Download application
			result=startDownload();
			break;
		}
		case status::mounted: {
			status( status::waiting );
			_runTask = new RunTask(this);
			startImpl();
			result=true;
			break;
		}
		case status::stopping: {
			//	Called from Controller::stop to start again the application ..... invalid!
			DTV_ASSERT(false);
			result=false;
			break;
		}
		case status::waiting: {
			DTV_ASSERT(_runTask);
			startImpl();
			result=true;
			break;
		}
		case status::starting:
		case status::started: {
			LWARN("app::Application", "application already started: name=%s, id=%s",
				name().c_str(), appID().asString().c_str());
			result=false;
			break;
		}
	}

	return result;
}

void Application::startImpl() {
	if (ctrl()->canStartApplications()) {
		ctrl()->start( _runTask );
	}
}

void Application::stop( bool kill/*=false*/ ) {
	if (isRunning()) {
		LINFO("app::Application", "Stop application: name=%s, kill=%d", name().c_str(), kill );
		DTV_ASSERT(_runTask);

		if (status() > status::waiting) {
			status( status::stopping );
		}

		ctrl()->stop( _runTask, kill );
		DEL(_runTask);

		//	Change status
		status( status::mounted );
	}
	else {
		LDEBUG( "app::Application", "Cannot stop application: invalid state: name=%s, st=%d",
			name().c_str(), _status );
	}
}

void Application::started() {
	status( status::started );
}

void Application::starting() {
	status( status::starting );
}

void Application::queue() {
	LDEBUG("app::Application", "Queue application: name=%s", name().c_str() );

	if (status() > status::waiting) {
		status( status::stopping );

		ctrl()->stop( _runTask, true );

		status( status::waiting );
	}
}

void Application::onStarting( RunTask * /*task*/ ) {
}

void Application::onStarted() {
}

void Application::onStopping() {
}

void Application::onStopped() {
	//	Returns the keys
	std::vector<util::key::type> keys;
	reserveKeys( keys );

	//	Disable layer
	ctrl()->enableLayer(false);
}

int Application::exitTimeout() const {
	return 0;
}

Extension *Application::extension() const {
	return _extension;
}

Controller *Application::ctrl() const {
	return extension()->ctrl();
}

bool Application::isRunning() const {
	//	If application is waiting, need stop
	return status() > status::mounted;
}

status::type Application::status() const {
	return _status;
}

void Application::status( status::type st ) {
	//	Try change status
	if (_status != st && canChange(st)) {
		LDEBUG("app::Application", "On change status: cur=%d, new=%d", _status, st);

		//	Change status
		status::type old = _status;
		_status = st;

		//	Handle change of status
		switch (st) {
			case status::unmounted: {
				break;
			}
			case status::mounted: {
				if (old == status::stopping) {
					onStopped();
				}
				break;
			}
			case status::stopping: {
				onStopping();
				break;
			}
			case status::waiting: {
				if (old == status::stopping) {
					onStopped();
				}
				break;
			}
			case status::starting: {
				DTV_ASSERT(_runTask);
				onStarting(_runTask);
				break;
			}
			case status::started: {
				DTV_ASSERT(_runTask);
				onStarted();
				break;
			}
		};

		//	Notificate ctrl that status was changed
		ctrl()->onApplicationChanged( this );
	}
	else {
		LDEBUG("app::Application", "Can't change run state: application is not running: old=%d, st=%d", _status, st );
	}
}

bool Application::canChange( status::type st ) const {
	//	Check status
	bool canChange=false;
	switch (st) {
		case status::unmounted: {
			canChange = (_status == status::mounted);
			break;
		}
		case status::mounted: {
			canChange = (_status == status::unmounted || _status == status::stopping || _status == status::waiting);
			break;
		}
		case status::stopping: {
			canChange = (_status > status::waiting);
			break;
		}
		case status::waiting: {
			canChange = (_status == status::mounted || _status == status::stopping);
			break;
		}
		case status::starting: {
			canChange = (_status == status::waiting);
			break;
		}
		case status::started: {
			canChange = (_status == status::starting);
			break;
		}
	};

	return canChange;
}

void Application::show() const {
	LINFO("app::Application", "name=%s, language=%s, id=%s", _name.c_str(), _language.code(), _appID.asString().c_str());
	LINFO("app::Application", "\treadOnly=%d, start=%d, download=%d", _readOnly, _autoStart, _autoDownload );
	LINFO("app::Application", "\tpath=%s, service=%04x, visibility=%d", _path.c_str(), _service, _visibility );
	LINFO("app::Application", "\tpriority=%d, tag=%02x", _priority, _componentTag );
	LINFO("app::Application", "\tmodes (%d)", _videoModes.size() );
	BOOST_FOREACH( video::mode::type mode, _videoModes ) {
		LINFO("app::Application", "\t\t%x", mode);
	}
	LINFO("app::Application", "\ticons (%d)", _icons.size());
	BOOST_FOREACH( const std::string &icon, _icons) {
		LINFO("app::Application", "\t\t%s", icon.c_str());
	}
}

void Application::componentTag( util::BYTE tag ) {
	_componentTag = tag;
}

BYTE Application::componentTag() const {
	return _componentTag;
}

const std::string &Application::path() const {
	return _path;
}

void Application::mount( const std::string &path ) {
	if (fs::exists( path )) {
		LINFO("app::Application", "Mount application: app=%s, path=%s", _appID.asString().c_str(), path.c_str());
		_path = path;
		status( status::mounted );

		//	Application need start?
		if (autoStart()) {
			start();
		}
	}
	else {
		LWARN("app::Application", "mount point not exist: path=%s", path.c_str());
	}
}

void Application::mount( const std::string &path, const dsmcc::Events &events ) {
	stopStreamEvents();
	mount( path );
	_events = events;
}

bool Application::isMounted() const {
	return !_path.empty();
}

void Application::unmount() {
	if (isMounted()) {
		LINFO("app::Application", "Unmount application: app=%s", _appID.asString().c_str());

		try {
			if (!readOnly() && fs::exists( _path ))	{
				//	Remove all content
				fs::remove_all( _path );
			}
		}
		catch (std::exception& e) {
			LWARN("app::Application", "Exception: %s", e.what());
		}

		_path = "";
		status( status::unmounted );
	}
}

//	Getters/setters
bool Application::isBoundToService() const {
	return _service ? true : false;
}

bool Application::isBoundToService( ID serviceID ) const {
	return _service ? (serviceID == _service) : false;
}

ID Application::service() const {
	return _service;
}

void Application::service( ID bound ) {
	_service = bound;
}

visibility::type Application::visibility() const {
	return _visibility;
}

void Application::visibility( visibility::type var ) {
	_visibility = var;
}

int Application::priority() const {
	return _priority;
}

void Application::priority( int pri ) {
	_priority = pri;
}

void Application::addIcon( const std::string &icon ) {
	_icons.push_back( icon );
}

void Application::addIcons( const std::vector<std::string> &icons ) {
	_icons.insert( _icons.end(), icons.begin(), icons.end() );
}

const std::vector<std::string> &Application::icons() const {
	return _icons;
}

void Application::addVideoMode( video::mode::type mode ) {
	std::vector<video::mode::type>::const_iterator it = std::find( _videoModes.begin(), _videoModes.end(), mode );
	if (it == _videoModes.end()) {
		LDEBUG("app::Application", "Add video mode: %d", mode);
		_videoModes.push_back( mode );
	}
}

const std::vector<video::mode::type> &Application::supportedModes() const {
	return _videoModes;
}

bool Application::readOnly() const {
	return _readOnly;
}

void Application::readOnly( bool var ) {
	_readOnly = var;
}

const ApplicationID &Application::appID() const {
	return _appID;
}

const Language &Application::language() const {
	return _language;
}

const std::string &Application::name() const {
	return _name;
}

void Application::name( const std::string &n, const Language &lang/*=""*/ ) {
	_name = n;
	_language = lang;
}

bool Application::autoStart() const {
	return _autoStart;
}

void Application::autoStart( bool var ) {
	_autoStart = var;
}

//	Download methods
bool Application::needDownload() const {
	return (_autoDownload || _autoStart) && !isMounted() && componentTag() != 0xFF;
}

bool Application::autoDownload() const {
	return _autoDownload;
}

void Application::autoDownload( bool var ) {
	_autoDownload = var;
}

void Application::onMounted( const boost::shared_ptr<dsmcc::ObjectCarousel> &oc ) {
	LDEBUG("app::Application", "On Object Carousel mounted: root=%s, events=%d",
		oc->root().c_str(), oc->events().size() );

	//	Applications already mounted?
	if (isMounted()) {
		//	Un mount previously mounted applications
		unmount();
	}

	//	Create application name
	fs::path path = _extension->resMgr()->applicationsPath();
	path /= _appID.asString();

	//	If exits path, remove!
	if (fs::exists( path )) {
		fs::remove_all( path );
	}

	//	Rename rootDirectory to applicationsPath/ApplicationName
	fs::rename( oc->root(), path );

	//	Mount filesystem
	mount( path.string(), oc->events() );
}

void Application::onDownloadChanged( int step, int total ) {
	//	Notificate ctrl the download status
	ctrl()->onDownloadProgress( this, step, total );
}

void Application::onEvent( WORD eventID, util::Buffer *streamEvent ) {
	StreamEventCallback::const_iterator it=_streamCallbacks.find( eventID );
	if (it != _streamCallbacks.end()) {
		(*it).second( *streamEvent );
	}
	delete streamEvent;
}

bool Application::startObjectCarousel( ID pid, ID tag ) {
	ResourceManager *resMgr = _extension->resMgr();

	//	Create DSMCC
	dsmcc::DSMCCDemuxer *dsmcc=new dsmcc::DSMCCDemuxer( pid, resMgr );

	//	Create object carousel and setup
	dsmcc::ObjectCarouselFilter *oc = new dsmcc::ObjectCarouselFilter( tag, dsmcc );
	oc->onMounted( boost::bind(&Application::onMounted, this, _1) );
	oc->onProgressChanged( boost::bind(&Application::onDownloadChanged, this, _1, _2) );

	//	Start filter
	if (!_extension->srvMgr()->startFilter( dsmcc )) {
		LWARN("app::Application", "cannot download application. Filter cannot be started: pid=%04x", pid);
		return false;
	}

	return true;
}

bool Application::startDownload() {
	//	Find PID from DSMCC tag
	ID pid=_extension->findTag( componentTag(), service() );
	if (pid == TS_PID_NULL) {
		LWARN("app::Application", "ignoring download becouse cannot find data carousel");
		return false;
	}

	//	Setup and start object carousel demuxer
	if (startObjectCarousel( pid, componentTag() )) {
		//	Save object carousel demuxer
		_downloads.push_back( pid );
		return true;
	}
	return false;
}

void Application::stopDownload() {
	//	Stop all filters
	stopStreamEvents();

	//	Stop all object carousel
	BOOST_FOREACH( ID pid, _downloads ) {
		_extension->srvMgr()->stopFilter( pid );
	}
	_downloads.clear();
}

//	Keys
bool Application::dispatchKey( util::key::type key, bool isUp ) {
	std::vector<util::key::type>::const_iterator it=std::find(
		_keys.begin(),
		_keys.end(),
		key );

	if (it != _keys.end()) {
		dispatchKeyImpl( key, isUp );
		return true;
	}
	return false;
}

void Application::dispatchKeyImpl( util::key::type /*key*/, bool /*isUp*/ ) {
	//	Do nothing
}

void Application::reserveKeys( const std::vector<util::key::type> &keys ) {
	_keys = keys;
	ctrl()->reserveKeys( appID(), keys, priority() );
}

//	Stream event
void Application::stopStreamEvent( ID pid ) {
	_extension->srvMgr()->stopFilter( pid );
}

void Application::stopStreamEvents()  {
	BOOST_FOREACH( const StreamEventItem &item, _streams ) {
		const StreamEventType &ste = item.second;
		stopStreamEvent( ste.first->pid() );
	}
	_streams.clear();
	_streamCallbacks.clear();
}

bool Application::registerEvent( const std::string &url, const std::string &eventName, const EventCallback &callback ) {
	bool result=false;
	dsmcc::StreamEventDemuxer *demux=NULL;

	//	Find event on list of available events
	WORD eventID;
	dsmcc::Events::const_iterator it=std::find_if( _events.begin(), _events.end(), EventFinder(url,eventName,eventID) );
	if (it != _events.end()) {
		bool created=false;

		//	Find url on already started DSMCC event filters
		StreamEventFilters::iterator fit=_streams.find( url );
		if (fit != _streams.end()) {
			StreamEventType &ste = (*fit).second;
			//	Get already started demux
			demux = ste.first;
			//	Add reference
			ste.second++;
		}
		else {
			//	Find PID from DSMCC tag
			ID pid=_extension->findTag( (*it)->tag(), service() );
			if (pid == TS_PID_NULL) {
				LWARN("app::Application", "ignoring stream event becouse cannot find PID");
			}
			else {
				//	Create DSMCC demuxer from tag
				demux = new dsmcc::StreamEventDemuxer( pid );
				created = true;
			}
		}

		if (demux) {
			//	Setup demuxer
			dsmcc::StreamEventDemuxer::Callback onEvent = boost::bind( &Application::onEvent,this,eventID,_1 );
			demux->registerStreamEvent( eventID, onEvent );

			//	Start demuxer
			if (created) {
				//	Try start filter
				result=_extension->srvMgr()->startFilter( demux );
				if (result) {
					//	Save demux
					_streams[url] = std::make_pair(demux,1);
				} else {
					LWARN("app::Application", "cannot start filter for event: url=%s, eventName=%s",
						url.c_str(), eventName.c_str());
				}
			} else {
				result=true;
			}

			if (result) {
				//	Save eventID callback
				_streamCallbacks[eventID] = callback;
			}
		}
	} else {
		LWARN("app::Application", "event not found: url=%s, name=%s", url.c_str(), eventName.c_str());
	}
	LDEBUG("app::Application", "Register event: url=%s, name=%s, eventID=%04x, result=%d", url.c_str(), eventName.c_str(), eventID, result);
	return result;
}

void Application::unregisterEvent( const std::string &url, const std::string &eventName ) {
	//	Find url on already started DSMCC event filters
	StreamEventFilters::iterator fit=_streams.find( url );
	if (fit != _streams.end()) {
		//	Find event
		WORD eventID;
		dsmcc::Events::const_iterator it=std::find_if( _events.begin(), _events.end(), EventFinder(url,eventName,eventID) );
		if (it != _events.end()) {
			StreamEventType &ste = (*fit).second;

			//	Get already started demux and un-register event
			dsmcc::StreamEventDemuxer *demux = ste.first;
			demux->unregisterStreamEvent( eventID );

			//	Remove reference
			ste.second--;
			if (!ste.second) {
				stopStreamEvent( demux->pid() );
				_streams.erase( fit );
			}

			//	Remove stream event callback
			StreamEventCallback::iterator it=_streamCallbacks.find( eventID );
			if (it != _streamCallbacks.end()) {
				_streamCallbacks.erase( it );
			}
		}

		LDEBUG("app::Application", "Un-register event: url=%s, name=%s, eventID=%04x", url.c_str(), eventName.c_str(), eventID);
	}
}

}
}
