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
#pragma once

#include "../extension.h"
#include "../../../../../src/service/extension/application/extension.h"
#include "../../../../../src/service/extension/application/application.h"
#include "../../../../../src/service/extension/application/factory.h"
#include "../../../../../src/service/extension/application/controller/controller.h"
#include "../../../../../src/service/extension/application/controller/runtask.h"
#include "../../../../../src/demuxer/psi/psi.h"
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

typedef std::vector<tuner::app::Application *> Applications;
typedef std::pair<tuner::app::Application *,bool> StoppedApplication;
typedef std::vector<StoppedApplication> StopeedApplications;

class FakeAppCtrl : public tuner::app::Controller {
public:
	FakeAppCtrl()
	{
	}
	virtual ~FakeAppCtrl()
	{
	}

	void getApplications( Applications &apps, bool asUser=true ) {
		BOOST_FOREACH( tuner::app::Application *app, _apps ) {
			if ((asUser && app->visibility() == tuner::app::visibility::both) || (!asUser)) {
				apps.push_back( app );
			}
		}
	}

	//	Zapper virtual methods
	virtual void onApplicationAdded( tuner::app::Application *app ) {
		_apps.push_back( app );
	}

	virtual void onApplicationRemoved( tuner::app::Application *app ) {
		Applications::iterator it=std::find( _apps.begin(), _apps.end(), app );
		if (it != _apps.end()) {
			_apps.erase( it );
		}
	}

	virtual void reserveKeys( const tuner::app::ApplicationID &/*id*/, const tuner::app::Keys &/*keys*/, int /*priority*/ ) {}

	//	Start application
	Applications _started;
	StopeedApplications _stopped;

	//	Start/stop application
	virtual void start( tuner::app::RunTask *task ) {
		_started.push_back( task->app() );
		// task->app()->starting();
		// task->app()->started();
	}

	virtual void stop( tuner::app::RunTask *task, bool kill ) {
		_stopped.push_back( std::make_pair(task->app(),kill) );
	}

protected:
	//	Create extension
	virtual void setupFactories( tuner::app::Extension * /*ext*/ ) {}
	virtual void enableLayer( bool /*enable*/ ) {}

private:
	Applications _apps;
};

class OkApplication : public tuner::app::Application {
public:
	OkApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id );
	OkApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id, const std::string &path );
};

class KeysApplication : public tuner::app::Application {
public:
	KeysApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id );
	int dispatchedKeys();

protected:
	virtual void dispatchKeyImpl( util::key::type key, bool isUp );

private:
	int _dispatchedKeys;
};

class OkApplicationFactory : public tuner::app::Factory {
public:
	OkApplicationFactory( tuner::app::Extension *ext ) : tuner::app::Factory( ext ) {}
	virtual ~OkApplicationFactory() {}

	virtual tuner::app::Application *tryFile( const std::string &file, const tuner::app::ApplicationID &appID ) {
		tuner::app::Application *app=NULL;
		fs::path f(file);
		if (f.extension() == ".test") {
			app = new OkApplication( extension(), appID, file );
		}
		return app;
	}

};

class ApplicationExtensionTest : public ServiceManagerTest {
public:
	ApplicationExtensionTest( void ) {}
	virtual ~ApplicationExtensionTest( void ) {}

	virtual void SetUp() {
		ServiceManagerTest::SetUp();
		_ctrl = createCtrl();
		mgr()->attach( _ctrl->initialize() );
		mgr()->initialize();
		mgr()->start();
	}

	virtual void TearDown() {
		mgr()->stop();
		delete _ctrl;
	}

	tuner::app::Extension *extension() {
		return _ctrl->extension();
	}

	virtual FakeAppCtrl *createCtrl( void ) {
		return new FakeAppCtrl();
	}

	Applications getApps( bool asUser=true ) {
		Applications apps;
		_ctrl->getApplications( apps, asUser );
		return apps;
	}

	OkApplication *createApp( const tuner::app::ApplicationID &id, const std::string &path ) {
		return new OkApplication( extension(), id, path );
	}

	OkApplication *createApp( const tuner::app::ApplicationID &id ) {
		return new OkApplication( extension(), id );
	}

	void probeOC( tuner::ID ocPID, util::BYTE tag=0xc ) {
		std::string file;

		switch (tag) {
			case 0x0a:
				file = "dsi_tc001_ver1_tag0x0A.sec";
				break;
			case 0x0b:
				file = "dsi_tc001_ver1_tag0x0B.sec";
				break;
			case 0x0c:
				file = "dsi_v1_sample_tc001.sec";
				break;
			case 0x0d:
				file = "dsi_tc001_ver1_tag0x0D.sec";
				break;
			case 0x0e:
				file = "dsi_tc001_ver1_tag0x0E.sec";
				break;
			default:
				file = "dsi_v1_sample_tc001.sec";
				break;
		};

		mgr()->probe( ocPID, file.c_str() );
		mgr()->probe( ocPID, "dii_v1_sample_tc001.sec" );
		mgr()->probe( ocPID, "ddb01_v1_sample_tc001.sec" );
		mgr()->probe( ocPID, "ddb02_v1_sample_tc001.sec" );
	}

protected:
	FakeAppCtrl *_ctrl;
};
