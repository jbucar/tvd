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

#include "servicemanager.h"
#include "service.h"
#include "ca/filterdelegateimpl.h"
#include "ca/conditional.h"
#include "extension/extension.h"
#include "../resourcemanager.h"
#include "../provider/provider.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>


#define NOTIFY(f)	\
	BOOST_FOREACH(Extension *ext, _extensions) { if (ext->mustNotify(_inScanMode)) { ext->f; } }

namespace tuner {

/**
 * Constructor base.
 * @param resMgr Instancia de @c ResourceManager.
 */
ServiceManager::ServiceManager( ResourceManager *resMgr )
{
	DTV_ASSERT(resMgr);
	_resMgr = resMgr;
	_ca = ca::Conditional::create( new ca::FilterDelegateImpl(this) );
	_initialized = false;
	_running = false;
	_inScan = false;
	_inScanMode = false;
	_isReady = false;
}

/**
 * Destructor base.
 */
ServiceManager::~ServiceManager()
{
	//	Destroy extensions
	CLEAN_ALL( Extension *, _extensions );
	delete _ca;
	delete _resMgr;

	//	Checks
	DTV_ASSERT(!_initialized);
	DTV_ASSERT(!_running);
	DTV_ASSERT(!_inScan);
	DTV_ASSERT(!_inScanMode);
	DTV_ASSERT(!_isReady);
	DTV_ASSERT(_services.empty());
	DTV_ASSERT(_servicesQueued.empty());
}

//	Initialization/finalization
void ServiceManager::initialize() {
	LDEBUG( "ServiceManager", "Initialize" );
	DTV_ASSERT(!_initialized);
	DTV_ASSERT(!_running);
	DTV_ASSERT(!_inScan);
	DTV_ASSERT(!_inScanMode);
	DTV_ASSERT(!_isReady);
	DTV_ASSERT(_services.empty());
	DTV_ASSERT(_servicesQueued.empty());

	std::vector<Extension *> failed;
	BOOST_FOREACH( Extension *ext, _extensions ) {
		if (!ext->initialize( this )) {
			LWARN( "ServiceManager", "Cannot initialize extension; detach" );
			failed.push_back( ext );
		}
	}

	BOOST_FOREACH( Extension *ext, failed ) {
		detach( ext );
		delete ext;
	}

	_initialized = true;
}

void ServiceManager::finalize() {
	LDEBUG( "ServiceManager", "Finalize" );

	//	Checks
	DTV_ASSERT(!_running);
	DTV_ASSERT(!_isReady);
	DTV_ASSERT(_services.empty());
	DTV_ASSERT(_servicesQueued.empty());

	if (_initialized) {
		BOOST_FOREACH( Extension *ext, _extensions ) {
			ext->finalize();
		}
		_initialized = false;
	}
}

//	Start/Stop
void ServiceManager::start( bool scan/*=false*/ ) {
	LDEBUG( "ServiceManager", "Start: scan=%d", scan );

	//	Checks
	DTV_ASSERT(_initialized);
	DTV_ASSERT(!_running);
	DTV_ASSERT(!_inScan);
	DTV_ASSERT(!_inScanMode);
	DTV_ASSERT(!_isReady);
	DTV_ASSERT(_services.empty());

	_inScanMode = scan;

	if (_ca) {
		_ca->start();
	}

	NOTIFY( onStart() );
	_running = true;
}

void ServiceManager::stop() {
	if (_running) {
		LDEBUG( "ServiceManager", "Stop" );

		//	Notify all extensions
		NOTIFY( onStop() );

		if (_ca) {
			_ca->stop();
		}

		//	Clean state
		_servicesQueued.clear();
		_running = false;
		_inScan = false;
		_inScanMode = false;
		_isReady = false;

		//	Clean temporary files
		resMgr()->clean();
	}
}

bool ServiceManager::isRunning() const {
	return _running;
}

bool ServiceManager::isEnqueued( ID serviceID ) const {
	std::vector<ID>::const_iterator it=std::find( _servicesQueued.begin(), _servicesQueued.end(), serviceID );
	return (it == _servicesQueued.end()) ? false : true;
}

void ServiceManager::removeQueued( ID serviceID ) {
	std::vector<ID>::iterator it=std::find( _servicesQueued.begin(), _servicesQueued.end(), serviceID );
	if (it != _servicesQueued.end()) {
		LDEBUG( "ServiceManager", "Remove queued service: serviceID=%x", serviceID );
		_servicesQueued.erase( it );
	}
}

//	Start/Stop services
void ServiceManager::startService( ID service ) {
	LDEBUG("ServiceManager", "Start service by ID=%x", service);

	//	Check if service ready ...
	Service *srv = findService( service );
	if (srv && isReady() && srv->state() == service::state::ready) {
		//	Start service ...
		startService( srv );
	}
	else if (!isEnqueued( service )) {
		//	enqueue and wait ...
		_servicesQueued.push_back( service );
	}
}

bool ServiceManager::isRunning( ID serviceID ) const {
	Service *srv = findService( serviceID );
	return (srv && isReady() && srv->state() == service::state::running);
}

Service *ServiceManager::get( ID serviceID ) const {
	return findService( serviceID );
}

void ServiceManager::stopService( ID service ) {
	LDEBUG("ServiceManager", "Stop service by ID begin: %04x", service);

	//	Find service ...
	Service *srv = findService( service );
	if (srv) {
		//	Stop service ...
		stopService( srv );
	}
	else {
		//	Find service in services queued to run
		removeQueued( service );
	}

	LDEBUG("ServiceManager", "Stop service by ID end: %04x", service);
}

void ServiceManager::stopServices() {
	LDEBUG("ServiceManager", "Stop services");

	//	Stop all services runnning
	BOOST_FOREACH( Service *srv, _services ) {
		stopService( srv );
	}

	//	Cleanup all services queued
	_servicesQueued.clear();
}

//	Services
ServiceManager::Services &ServiceManager::services() {
	return _services;
}

void ServiceManager::addService( Service *srv ) {
	_services.push_back( srv );
}

Service *ServiceManager::findService( ID service ) const {
	Services::const_iterator it=std::find_if(
		_services.begin(),
		_services.end(),
		boost::bind(&Service::id,_1) == service
	);
	return (it != _services.end()) ? (*it) : NULL;
}

Service *ServiceManager::findServiceByPID( ID pid ) {
	Services::const_iterator it=std::find_if(
		_services.begin(),
		_services.end(),
		boost::bind(&Service::pid,_1) == pid
	);
	return (it != _services.end()) ? (*it) : NULL;
}

void ServiceManager::startService( Service *srv ) {
	LDEBUG("ServiceManager", "On start service begin: id=%x", srv->id());

	DTV_ASSERT( srv->state() == service::state::ready );

	//	Remove if queued
	removeQueued( srv->id() );

	//	Notify to CA
	if (_ca) {
		_ca->onStart( srv );
	}

	//	Notify to extensions of service was started
	NOTIFY( onServiceStarted( srv, true ) );

	//	Mark service as running
	srv->state( service::state::running );

	LDEBUG("ServiceManager", "On start service end: id=%x", srv->id());
}

bool ServiceManager::stopService( Service *srv ) {
	bool result=false;

	if (srv->state() == service::state::running) {
		LDEBUG("ServiceManager", "On stop service: %04x", srv->id());

		//	Mark service as ready
		srv->state( service::state::ready );

		//	Notify to extensions
		NOTIFY( onServiceStarted( srv, false ) );

		//	Notify to CA
		if (_ca) {
			_ca->onStop( srv );
		}

		result=true;
	}

	return result;
}

void ServiceManager::setServiceReady( Service *srv ) {
	LDEBUG("ServiceManager", "Service ready: name=%s, id=%s",
		srv->name().c_str(), srv->serviceID().asString().c_str() );

	//	Set state
	DTV_ASSERT( srv->state() == service::state::complete );
	srv->state( service::state::ready );

	//	Notify to extensions
	NOTIFY( onServiceReady( srv, true ) );

	//	If serviceID was queued to run
	if (isEnqueued(srv->id())) {
		//	Start service
		startService( srv );
	}
}

void ServiceManager::expireService( Service *srv, bool needExpire/*=true*/ ) {
	if (srv->state() >= service::state::ready) {
		LDEBUG("ServiceManager", "On expire service: serviceID=%04x, needExpire=%d", srv->id(), needExpire);

		//	Stop service if running
		bool stopped=stopService( srv );

		//	Set new status
		srv->state( needExpire ? service::state::expired : service::state::complete );

		//	Notify to extensions
		NOTIFY( onServiceReady( srv, false ) );

		//	Queue running state
		if (stopped) {
			startService( srv->id() );
		}
	}
}

void ServiceManager::serviceChanged( Service *srv ) {
	DTV_ASSERT(srv);
	if (isReady() && srv->state() == service::state::complete) {
		setServiceReady( srv );
	}
}

//	State
bool ServiceManager::isReady() const {
	return _isReady;
}

void ServiceManager::setReady() {
	if (!_isReady) {
		LDEBUG("ServiceManager", "Service provider ready");

		_isReady = true;

		//	Notify to extensions
		NOTIFY( onReady(true) );

		//	Notify all services ready
		BOOST_FOREACH( Service *srv, _services ) {
			if (srv->state() == service::state::complete) {
				setServiceReady( srv );
			}
		}

		//	Notify end scan
		notifyEndScan();
	}
}

void ServiceManager::expire() {
	if (_isReady) {
		LDEBUG("ServiceManager", "Service provider expired");

		_isReady = false;

		//	Expire all services
		BOOST_FOREACH( Service *srv, _services ) {
			expireService( srv, false );
		}

		//	Notify to extensions
		NOTIFY( onReady( false ) );
	}
}

//	Scan
void ServiceManager::onEndScan( const OnEndScan &callback ) {
	_onEndScan = callback;
}

void ServiceManager::notifyEndScan( bool allScanned/*=false*/ ) {
	if (!_inScan) {
		//	Check if last scanned ...
		if (!allScanned) {
			allScanned=true;
			BOOST_FOREACH( Service *srv, _services ) {
				allScanned &= (srv->state() >= service::state::timeout);
			}
		}

		if (allScanned) {
			LDEBUG("ServiceManager", "Notify end scan");
			_inScan = true;
			if (!_onEndScan.empty()) {
				_onEndScan();
			}
		}
	}
}

//	Extensions
bool ServiceManager::isAttached( Extension *ext ) const {
	return std::find( _extensions.begin(), _extensions.end(), ext ) != _extensions.end();
}

/**
 * Agrega la extensión @b extension al ServiceManager.
 * @param extension La extensión a agregar.
 */
void ServiceManager::attach( Extension *extension ) {
	LDEBUG("ServiceManager", "Attach extension: extension=%p", extension );
	if (_initialized) {
		extension->initialize( this );

		if (_running) {
			extension->onStart();

			if (isReady()) {
				extension->onReady( true );

				BOOST_FOREACH( Service *srv, _services ) {
					if (srv->state() >= service::state::ready) {
						extension->onServiceReady( srv, true );

						if (srv->state() >= service::state::running) {
							extension->onServiceStarted( srv, true );
						}
					}
				}
			}
		}
	}

	_extensions.push_back( extension );
}

/**
 * Elimina la extensión @b extension del ServiceManager.
 * @param extension La extensión a eliminar.
 */
void ServiceManager::detach( Extension *extension ) {
	std::vector<Extension *>::iterator it=std::find(
		_extensions.begin(), _extensions.end(), extension );
	if (it != _extensions.end()) {
		LDEBUG("ServiceManager", "Detach extension: extension=%p", extension );
		if (_initialized) {
			if (_running) {
				if (isReady()) {
					BOOST_FOREACH( Service *srv, _services ) {
						if (srv->state() >= service::state::ready) {
							if (srv->state() >= service::state::running) {
								extension->onServiceStarted( srv, false );
							}

							extension->onServiceReady( srv, false );
						}
					}

					extension->onReady( false );
				}

				extension->onStop();
			}

			extension->finalize();
		}

		_extensions.erase( it );
	}
}

/**
 * @return El administrador de recursos utilizado por el ServiceManager.
 */
ResourceManager *ServiceManager::resMgr() {
	return _resMgr;
}

ca::Conditional *ServiceManager::ca() const {
	return _ca;
}

}
