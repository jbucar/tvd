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
#include "extension.h"
#include "application.h"
#include "factory.h"
#include "controller/controller.h"
#include "../../serviceprovider.h"
#include "../../service.h"
#include "../../../provider/provider.h"
#include "../../../resourcemanager.h"
#include "../../../demuxer/psi/dsmcc/datacarouselhelper.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>


namespace tuner {
namespace app {

namespace fs = boost::filesystem;

class FindApplicationIterator {
public:
	FindApplicationIterator() {}
	virtual ~FindApplicationIterator() {}

	virtual bool operator()( Application * /*app*/ ) const {
		return false;
	}
};

class AllApplications : public FindApplicationIterator {
public:
	bool operator()( app::Application * /*app*/ ) const {
		return true;
	}
};

class QueuedApplications : public FindApplicationIterator {
public:
	bool operator()( const app::Application *app ) const {
		if (app->isRunning()) {
			return true;
		}
		return false;
	}
};

class AppFinderByTag : public FindApplicationIterator {
public:
	AppFinderByTag( BYTE tag ) : _tag(tag) {}
	bool operator()( app::Application *app ) const {
		return app->componentTag() == _tag;
	}
	BYTE _tag;
};

class AppFinderByAppID : public FindApplicationIterator {
public:
	AppFinderByAppID( const app::ApplicationID &id ) : _id(id) {}
	bool operator()( app::Application *app ) const {
		return *app == _id;
	}
	const app::ApplicationID &_id;

private:
	// Avoid VisualStudio warning C4512
	AppFinderByAppID &operator=(AppFinderByAppID &/*af*/) { return *this; }
};

class AppFinderByBounded : public FindApplicationIterator {
public:
	AppFinderByBounded( ID srvID ) : _srvID(srvID) {}
	bool operator()( app::Application *app ) const {
		return app->isBoundToService( _srvID );
	}
	ID _srvID;
};

Extension::Extension( Controller *ctrl )
{
	DTV_ASSERT(ctrl);
	_ctrl = ctrl;
	_dsmcc  = new dsmcc::DataCarouselHelper();
	_appID = 1;
}

Extension::~Extension()
{
	CLEAN_ALL( Factory *, _factories );
	delete _dsmcc;
	removeAll();
}

//	Factories method
void Extension::addFactory( Factory *factory ) {
	_factories.push_back( factory );
}

//	Aux filesystem
void Extension::scan( const std::string path, int maxDepth, const EndScanApplicationCallback &callback ) {
	//	Scan applications
	try {
		scanApplications( path, maxDepth, 0 );
	} catch (...) {
		LWARN("Extension", "scan of applications terminated with errors");
	}

	//	On finish, call to callback
	if (!callback.empty()) {
		callback();
	}
}

void Extension::scanApplications( const std::string &appPath, int maxDepth, int curDepth ) {
	LDEBUG("Extension", "Scan applications on %s", appPath.c_str());

	curDepth++;
	if (fs::exists( appPath )) {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr( appPath ); itr != end_itr; ++itr ) {
			if (fs::is_directory( itr->status() ) && curDepth < maxDepth) {
				scanApplications( itr->path().string(), maxDepth, curDepth );
			}
			else if (fs::is_regular_file( itr->status() )) {
				fs::path file = itr->path();

				//	factories to detect new applications?
				BOOST_FOREACH( Factory *factory, _factories ) {
					//	Try detect file ...
					ApplicationID appID(0, _appID);
					Application *app = factory->tryFile( file.string(), appID );
					if (app) {
						add( app );
						_appID++;
					}
				}
			}
		}
	}
}

//	Add/Remove application
void Extension::add( Application *app ) {
	DTV_ASSERT( app );

	const ApplicationID &appID = app->appID();
	if (!appID.isWildcard()) {
		LINFO("Extension", "Add application: id=%s", appID.asString().c_str());

		//	Notificate that a new application was added
		if (app->visibility() != visibility::none) {
			ctrl()->onApplicationAdded( app );
		}

#ifdef _DEBUG
		app->show();
#endif

		//	Add application to list
		_applications.push_back( app );

		//	Need download or start application
		if (app->needDownload()) {
			app->startDownload();
		}
		else if (app->autoStart()) {
			app->start();
		}
	}
	else {
		LWARN("Extension", "application ID is not valid: id=%s", appID.asString().c_str());
	}
}

void Extension::removeScanned() {
	//	Remove all scanned applications
	ApplicationID wildcard(0,0xFFFF);
	remove( wildcard );
}

void Extension::removeAll() {
	AllApplications iter;
	remove( &iter );
}

void Extension::remove( const ApplicationID &appID ) {
	AppFinderByAppID iter(appID);
	remove( &iter );
}

void Extension::remove( FindApplicationIterator *iter ) {
	//	Find application
	Application *app;
	std::vector<Application *>::iterator it = _applications.begin();
	while (it != _applications.end()) {
		app = (*it);

		if ((*iter)( app )) {
			LINFO("Extension", "Remove application: app=%s", app->name().c_str());
			//	Stop download
			app->stopDownload();

			//	Kill application
			app->stop( true );

			//	Unmount filesystem
			app->unmount();

			//	Notificate that a application was removed
			if (app->visibility() != visibility::none) {
				ctrl()->onApplicationRemoved( app );
			}
			delete app;

			//	Remove from list
			it=_applications.erase( it );
		}
		else {
			it++;
		}
	}
}

void Extension::changePriority( const ApplicationID &appID, BYTE priority ) {
	//	Find all applications matching appID and change the priority
	bool any=loopApplications(
		AppFinderByAppID(appID),
		boost::bind(&Application::priority,_1) );

	if (any) {
		LINFO("Extension", "Change priority: appID=%s, priority=%d", appID.asString().c_str(), priority);
	} else {
		LWARN("Extension", "cannot change priority: appID=%s, priority=%d", appID.asString().c_str(), priority);
	}
}

//	Aux applications
void Extension::start( const ApplicationID &appID ) {
	//	Stop all applications matching appID
	bool any=loopApplications(
		AppFinderByAppID(appID),
		boost::bind(&Application::start,_1) );

	if (!any) {
		LWARN("Extension", "cannot start application; application not found: id=%s", appID.asString().c_str());
	}
}

void Extension::stop( const ApplicationID &appID, bool kill/*=false*/ ) {
	//	Stop all applications matching appID
	bool any=loopApplications(
		AppFinderByAppID(appID),
		boost::bind(&Application::stop,_1,kill) );

	if (!any) {
		LWARN("Extension", "cannot stop application; application not found: id=%s", appID.asString().c_str());
	}
}

void Extension::stopAll() {
	LDEBUG("Extension", "On stop all application");
	loopApplications(
		AllApplications(),
		boost::bind(&Application::stop,_1,true) );
}

void Extension::queueAll() {
	LDEBUG("Extension", "On queue all application");
	loopApplications(
		AllApplications(),
		boost::bind(&Application::queue,_1) );
}

void Extension::startQueued() {
	LDEBUG("Extension", "Start queued applications");
	loopApplications(
		QueuedApplications(),
		boost::bind(&Application::start,_1) );
}

//	Service Provider notifications
static bool collectDSMCC( BYTE streamType ) {
	return streamType == PSI_ST_TYPE_B || streamType == PSI_ST_TYPE_C;
}

void Extension::onServiceStarted( Service *srv, bool started ) {
	LDEBUG("Extension", "On Service started begin: factories=%d, started=%d",
		_factories.size(), started );

	if (started) {
		//	Start factories on service
		BOOST_FOREACH( Factory *factory, _factories ) {
			factory->serviceStarted( srv );
		}

		//	Collect all DSMCC tags for object carousel
		_dsmcc->collectTags( srv, &collectDSMCC );
	}
	else {
		//	Stop factories
		BOOST_FOREACH( Factory *factory, _factories ) {
			factory->serviceStopped( srv );
		}

		//	Remove all applications bounded to service
		AppFinderByBounded iter(srv->id());
		remove( &iter );

		//	Clear all tags referenced by the service
		std::vector<BYTE> tags=_dsmcc->clearTags( srv );

		//	Stop all tags
		BOOST_FOREACH( BYTE tag, tags ) {
			loopApplications(
				AppFinderByTag(tag),
				boost::bind(&Application::stopDownload,_1) );
		}
	}

	LDEBUG("Extension", "On Service started end");
}

bool Extension::onInitialize() {
	_ctrl->attach( srvMgr() );
	return true;
}

void Extension::onFinalize() {
	//	Remove all applications
	removeAll();
}

//	Aux download
void Extension::download( const ApplicationID &appID ) {
	//	Start all downloads matching appID
	bool any=loopApplications(
		AppFinderByAppID(appID),
		boost::bind(&Application::startDownload,_1) );

	if (!any) {
		LWARN("Extension", "cannot download application; application not found: id=%s", appID.asString().c_str());
	}
}

ID Extension::findTag( util::BYTE tag, ID service ) {
	return service ? _dsmcc->findTag( service, tag ) : _dsmcc->findTag( tag );
}

//	Aux keys
void Extension::dispatchKey( const ApplicationID &appID, util::key::type key, bool isUp ) {
	//	Dispatch key to appID
	bool any=loopApplications(
		AppFinderByAppID(appID),
		boost::bind(&Application::dispatchKey,_1,key,isUp) );

	if (!any) {
		LWARN("Extension", "cannot dispatch key; application not found: id=%s", appID.asString().c_str());
	}
}

bool Extension::dispatchKey( util::key::type key, bool isUp ) {
	bool sended=false;
	BOOST_FOREACH( Application *app, _applications ) {
		sended |= app->dispatchKey( key, isUp );
	}
	return sended;
}


}
}

