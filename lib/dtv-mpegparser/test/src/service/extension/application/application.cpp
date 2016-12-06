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
#include "applicationextension.h"
#include "../../../util.h"
#include "../../../../../src/service/extension/application/application.h"
#include "../../../../../src/demuxer/psi/dsmcc/event.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

TEST( Application, default_constructor ) {
	tuner::app::ApplicationID id(1,1);

	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_FALSE( app.isRunning() );
	ASSERT_TRUE( app.readOnly() );
	ASSERT_TRUE( app.componentTag() == 0xFF );
	ASSERT_FALSE( app.isMounted() );
	ASSERT_TRUE( app.appID() == id );
	ASSERT_TRUE( app.path().empty() );
	ASSERT_TRUE( app.language().empty() );
	ASSERT_TRUE( app.name().empty() );
	ASSERT_FALSE( app.autoStart() );
	ASSERT_FALSE( app.autoDownload() );
	ASSERT_TRUE( app.service() == 0 );
	ASSERT_TRUE( app.visibility() == tuner::app::visibility::none );
	ASSERT_TRUE( app.priority() == 0 );
	ASSERT_TRUE( app.supportedModes().empty() );
}

TEST( Application, filesystem_constructor ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id, "/tmp" );

	ASSERT_FALSE( app.isRunning() );
	ASSERT_TRUE( app.readOnly() );
	ASSERT_TRUE( app.componentTag() == 0xFF );
	ASSERT_TRUE( app.isMounted() );
	ASSERT_TRUE( app.appID() == id );
	ASSERT_TRUE( app.path() == "/tmp" );
	ASSERT_TRUE( app.language().empty() );
	ASSERT_TRUE( app.name().empty() );
	ASSERT_FALSE( app.autoStart() );
	ASSERT_FALSE( app.autoDownload() );
	ASSERT_TRUE( app.visibility() == tuner::app::visibility::both );
	ASSERT_TRUE( app.priority() == 0 );
	ASSERT_TRUE( app.service() == 0 );
	ASSERT_TRUE( app.supportedModes().empty() );
}

TEST( Application, name ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.language().empty() );
	ASSERT_TRUE( app.name().empty() );

	app.name( "one", "two" );
	ASSERT_TRUE( app.name() == "one" );
	ASSERT_TRUE( app.language() == "two" );
}

TEST( Application, readOnly ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.readOnly() );
	app.readOnly( false );
	ASSERT_FALSE( app.readOnly() );
}

TEST( Application, visibility ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.visibility() == tuner::app::visibility::none );

	app.visibility( tuner::app::visibility::both );
	ASSERT_TRUE( app.visibility() == tuner::app::visibility::both );

	app.visibility( tuner::app::visibility::api );
	ASSERT_TRUE( app.visibility() == tuner::app::visibility::api );
}

TEST( Application, componentTag ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.componentTag() == 0xFF );

	app.componentTag( 10 );
	ASSERT_TRUE( app.componentTag() == 10 );

	app.componentTag( 14 );
	ASSERT_TRUE( app.componentTag() == 14 );
}

TEST( Application, autoStart ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_FALSE( app.autoStart() );
	app.autoStart( true );
	ASSERT_TRUE( app.autoStart() );
}

TEST( Application, autoDownload ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_FALSE( app.autoDownload() );
	app.autoDownload( true );
	ASSERT_TRUE( app.autoDownload() );
}

TEST( Application, needDownload ) {
	tuner::app::ApplicationID id(1,1);
	FakeAppCtrl ctrl;
	tuner::app::Application app( ctrl.initialize(), id );

	//	Need:
	//	(autoDownload | autoStart) &&
	//	Not mounted
	//	Tag != 0xFF

	ASSERT_FALSE( app.autoDownload() );
	ASSERT_FALSE( app.autoStart() );
	ASSERT_FALSE( app.isMounted() );
	ASSERT_FALSE( app.componentTag() != 0xFF );
	ASSERT_FALSE( app.needDownload() );
	ASSERT_FALSE( app.isRunning() );

	app.autoDownload( true );
	ASSERT_TRUE( app.autoDownload() );
	ASSERT_FALSE( app.autoStart() );
	ASSERT_FALSE( app.isMounted() );
	ASSERT_FALSE( app.componentTag() != 0xFF );
	ASSERT_FALSE( app.needDownload() );
	ASSERT_FALSE( app.isRunning() );

	app.componentTag( 10 );
	ASSERT_TRUE( app.autoDownload() );
	ASSERT_FALSE( app.autoStart() );
	ASSERT_FALSE( app.isMounted() );
	ASSERT_TRUE( app.componentTag() != 0xFF );
	ASSERT_TRUE( app.needDownload() );
	ASSERT_FALSE( app.isRunning() );

	app.autoDownload( false );
	app.autoStart( true );
	ASSERT_FALSE( app.autoDownload() );
	ASSERT_TRUE( app.autoStart() );
	ASSERT_FALSE( app.isMounted() );
	ASSERT_TRUE( app.componentTag() != 0xFF );
	ASSERT_TRUE( app.needDownload() );
	ASSERT_FALSE( app.isRunning() );

	app.mount( fs::temp_directory_path().string() );
	ASSERT_FALSE( app.autoDownload() );
	ASSERT_TRUE( app.autoStart() );
	ASSERT_TRUE( app.isMounted() );
	ASSERT_TRUE( app.componentTag() != 0xFF );
	ASSERT_FALSE( app.needDownload() );
	ASSERT_TRUE( app.isRunning() );
	app.stop();
}

TEST( Application, boundToService ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_FALSE( app.isBoundToService( 0 ) );
	ASSERT_FALSE( app.isBoundToService( 5 ) );
	ASSERT_FALSE( app.isBoundToService( 20 ) );

	app.service( 20 );
	ASSERT_FALSE( app.isBoundToService( 0 ) );
	ASSERT_FALSE( app.isBoundToService( 5 ) );
	ASSERT_TRUE( app.isBoundToService( 20 ) );
}

TEST( Application, priority ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.priority() == 0 );
	app.priority( 10 );
	ASSERT_TRUE( app.priority() == 10 );
}

TEST( Application, mount_umount ) {
	tuner::app::ApplicationID id(1,1);
	FakeAppCtrl ctrl;
	tuner::app::Application app( ctrl.initialize(), id );

	ASSERT_FALSE( app.isMounted() );
	ASSERT_TRUE( app.readOnly() );
	app.mount( fs::temp_directory_path().string() );
	ASSERT_TRUE( app.isMounted() );
	app.unmount();
	ASSERT_FALSE( app.isMounted() );
}

TEST( Application, video_mode_simple_add ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.supportedModes().empty() );
	app.addVideoMode( tuner::app::video::mode::sd );

	const std::vector<tuner::app::video::mode::type> &modes = app.supportedModes();
	ASSERT_TRUE( modes.size() == 1 );
	ASSERT_TRUE( modes[0] == tuner::app::video::mode::sd );
}

TEST( Application, video_mode_multiple_add ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	ASSERT_TRUE( app.supportedModes().empty() );
	app.addVideoMode( tuner::app::video::mode::sd );
	app.addVideoMode( tuner::app::video::mode::hd );
	app.addVideoMode( tuner::app::video::mode::fullHD );

	const std::vector<tuner::app::video::mode::type> &modes = app.supportedModes();
	ASSERT_TRUE( modes.size() == 3 );
	ASSERT_TRUE( modes[0] == tuner::app::video::mode::sd );
	ASSERT_TRUE( modes[1] == tuner::app::video::mode::hd );
	ASSERT_TRUE( modes[2] == tuner::app::video::mode::fullHD );
}

TEST( Application, icons_simple_add ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	const std::vector<std::string> &icons = app.icons();

	ASSERT_TRUE( icons.size() == 0 );

	app.addIcon( "icon1" );
	ASSERT_TRUE( icons.size() == 1 );
	ASSERT_TRUE( icons[0] == "icon1" );
}

TEST( Application, icons_multiple_add ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	const std::vector<std::string> &icons = app.icons();

	ASSERT_TRUE( icons.size() == 0 );

	app.addIcon( "icon1" );
	app.addIcon( "icon2" );
	ASSERT_TRUE( icons.size() == 2 );
	ASSERT_TRUE( icons[0] == "icon1" );
	ASSERT_TRUE( icons[1] == "icon2" );
}

TEST( Application, icons_add_with_list ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( (tuner::app::Extension *)1, id );

	const std::vector<std::string> &icons = app.icons();

	ASSERT_TRUE( icons.size() == 0 );

	app.addIcon( "icon1" );
	ASSERT_TRUE( icons.size() == 1 );
	ASSERT_TRUE( icons[0] == "icon1" );

	//	Add list
	std::vector<std::string> tmp;
	tmp.push_back( "icon2" );
	tmp.push_back( "icon3" );
	tmp.push_back( "icon4" );
	app.addIcons( tmp );
	ASSERT_TRUE( icons.size() == 4 );
	ASSERT_TRUE( icons[0] == "icon1" );
	ASSERT_TRUE( icons[1] == "icon2" );
	ASSERT_TRUE( icons[2] == "icon3" );
	ASSERT_TRUE( icons[3] == "icon4" );
}

static inline std::string getTestPath() {
	fs::path tmp = fs::temp_directory_path();
	tmp /= "mpegparser-test";
	return tmp.string();
}

#define START_MGR1(Mgr)	  \
	tuner::ResourceManager *resMgr = new tuner::ResourceManager( getTestPath(), 100, 100, 4096 ); \
	Mgr mgr(resMgr); \
	FakeAppCtrl ctrl; \
	mgr.attach( ctrl.initialize() ); \
	mgr.initialize(); \
	mgr.start();

#define START_MGR	  \
	tuner::ResourceManager *resMgr = new tuner::ResourceManager( getTestPath(), 100, 100, 4096 ); \
	OkServiceManager mgr(resMgr); \
	FakeAppCtrl ctrl; \
	mgr.attach( ctrl.initialize() ); \
	mgr.initialize(); \
	mgr.start();

#define STOP_MGR	\
	mgr.stop(); \
	mgr.finalize();

TEST( Application, start_download_without_tag ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );
	ASSERT_FALSE( app.startDownload() );
	STOP_MGR;
}

TEST( Application, start_download_ok ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	app.componentTag( tag );
	ASSERT_TRUE( app.startDownload() );
	app.stopDownload();

	mgr.stopSrv( srv );
	STOP_MGR;
}

TEST( Application, start_download_filter_error ) {
	START_MGR1(FilterFailServiceMgr);
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	app.componentTag( tag );
	ASSERT_FALSE( app.startDownload() );
	app.stopDownload();

	mgr.stopSrv( srv );
	STOP_MGR;
}

void onEvent( const util::Buffer &/*buf*/ ) {
}

TEST( Application, register_event_ok ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );


	{
		std::string evtURL = "url";
		std::string evtName = "eventName";

		tuner::dsmcc::Events evts;
		std::vector<tuner::dsmcc::EventName> names;
		tuner::dsmcc::EventName tmp;
		tmp.id = 1;
		tmp.name = evtName;
		names.push_back( tmp );
		evts.push_back( new tuner::dsmcc::Event( evtURL, tag, names ) );
		app.mount( fs::temp_directory_path().string(), evts );
		ASSERT_TRUE( app.registerEvent( evtURL, evtName, &onEvent ) );
		app.unregisterEvent( evtURL, evtName );
		app.unregisterEvent( evtURL, evtName );
	}

	mgr.stopSrv( srv );
	STOP_MGR;
}

TEST( Application, register_multiple_events_ok ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	{
		std::string evtURL = "url";

		tuner::dsmcc::Events evts;
		std::vector<tuner::dsmcc::EventName> names;
		tuner::dsmcc::EventName tmp;
		tmp.id = 1;
		tmp.name = "EventName1";
		names.push_back( tmp );
		tmp.id = 2;
		tmp.name = "EventName2";
		names.push_back( tmp );
		evts.push_back( new tuner::dsmcc::Event( evtURL, tag, names ) );
		app.mount( fs::temp_directory_path().string(), evts );

		ASSERT_TRUE( app.registerEvent( evtURL, "EventName1", &onEvent ) );
		ASSERT_TRUE( app.registerEvent( evtURL, "EventName2", &onEvent ) );
		app.unregisterEvent( evtURL, "EventName1" );
		app.unregisterEvent( evtURL, "EventName2" );
	}

	mgr.stopSrv( srv );
	STOP_MGR;
}

TEST( Application, register_invalid_event ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	{
		std::string evtURL = "url";

		tuner::dsmcc::Events evts;
		std::vector<tuner::dsmcc::EventName> names;
		tuner::dsmcc::EventName tmp;
		tmp.id = 1;
		tmp.name = "EventName1";
		names.push_back( tmp );
		evts.push_back( new tuner::dsmcc::Event( evtURL, tag, names ) );
		app.mount( fs::temp_directory_path().string(), evts );

		ASSERT_TRUE( app.registerEvent( evtURL, "EventName1", &onEvent ) );
		ASSERT_FALSE( app.registerEvent( evtURL, "EventName2", &onEvent ) );
		app.unregisterEvent( evtURL, "EventName1" );
		app.unregisterEvent( evtURL, "EventName2" );
	}

	mgr.stopSrv( srv );
	STOP_MGR;
}

TEST( Application, register_invalid_tag ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	{
		std::string evtURL = "url";

		tuner::dsmcc::Events evts;
		std::vector<tuner::dsmcc::EventName> names;
		tuner::dsmcc::EventName tmp;
		tmp.id = 1;
		tmp.name = "EventName1";
		names.push_back( tmp );
		evts.push_back( new tuner::dsmcc::Event( evtURL, 0xd, names ) );
		app.mount( fs::temp_directory_path().string(), evts );

		ASSERT_FALSE( app.registerEvent( evtURL, "EventName1", &onEvent ) );
		app.unregisterEvent( evtURL, "EventName1" );
	}

	mgr.stopSrv( srv );
	STOP_MGR;
}

TEST( Application, register_fail_filter ) {
	START_MGR1(FilterFailServiceMgr);
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );

	//	Create service
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	//	Pass service to extension
	mgr.startSrv( srv );

	{
		std::string evtURL = "url";

		tuner::dsmcc::Events evts;
		std::vector<tuner::dsmcc::EventName> names;
		tuner::dsmcc::EventName tmp;
		tmp.id = 1;
		tmp.name = "EventName1";
		names.push_back( tmp );
		evts.push_back( new tuner::dsmcc::Event( evtURL, tag, names ) );
		app.mount( fs::temp_directory_path().string(), evts );

		ASSERT_FALSE( app.registerEvent( evtURL, "EventName1", &onEvent ) );
		app.unregisterEvent( evtURL, "EventName1" );
	}

	mgr.stopSrv( srv );
	STOP_MGR;
}

class CheckApplication : public tuner::app::Application {
public:
	CheckApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id )
		: tuner::app::Application( ext, id )
	{
		_onStarting = _onStarted = _onStopping = _onStopped = 0;
	}

	int _onStarting, _onStarted, _onStopping, _onStopped;

protected:
	//	Stop handling
	virtual void onStarting( tuner::app::RunTask * /*task*/ ) {
		_onStarting++;
	}

	virtual void onStarted() {
		_onStarted++;
	}

	virtual void onStopping() {
		_onStopping++;
	}

	virtual void onStopped() {
		_onStopped++;
	}
};

TEST( Application, status_waiting_stop ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == &app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app._onStarting == 0 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	ASSERT_TRUE( app._onStarting == 0 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_FALSE( app.isRunning() );

	STOP_MGR;
}

TEST( Application, status_waiting_stop_with_ctrl_disabled ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication *app = new CheckApplication( ctrl.extension(), id );
	ctrl.extension()->add( app );
	app->autoStart( true );

	//	Disable ctrl
	ctrl.disableApplications(true);
	app->mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 0);
	ASSERT_TRUE( ctrl._stopped.size() == 0);

	ctrl.disableApplications(false);
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app->_onStarting == 0 );
	ASSERT_TRUE( app->_onStarted == 0 );
	ASSERT_TRUE( app->_onStopping == 0 );
	ASSERT_TRUE( app->_onStopped == 0 );
	ASSERT_TRUE( app->isRunning() );

	app->stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	ASSERT_TRUE( app->_onStarting == 0 );
	ASSERT_TRUE( app->_onStarted == 0 );
	ASSERT_TRUE( app->_onStopping == 0 );
	ASSERT_TRUE( app->_onStopped == 0 );
	ASSERT_FALSE( app->isRunning() );

	STOP_MGR;
}

TEST( Application, status_started_stop ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == &app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app._onStarting == 0 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.starting();
	//	status -> starting
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.started();
	//	status -> started
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 1 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 1 );
	ASSERT_TRUE( app._onStopping == 1 );
	ASSERT_TRUE( app._onStopped == 1 );
	ASSERT_FALSE( app.isRunning() );

	STOP_MGR;
}

TEST( Application, status_started_queue ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication *app = new CheckApplication( ctrl.extension(), id );
	ctrl.extension()->add( app );
	app->name( "test" );
	app->autoStart( true );
	app->mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app->_onStarting == 0 );
	ASSERT_TRUE( app->_onStarted == 0 );
	ASSERT_TRUE( app->_onStopping == 0 );
	ASSERT_TRUE( app->_onStopped == 0 );
	ASSERT_TRUE( app->isRunning() );

	app->starting();
	//	status -> starting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app->_onStarting == 1 );
	ASSERT_TRUE( app->_onStarted == 0 );
	ASSERT_TRUE( app->_onStopping == 0 );
	ASSERT_TRUE( app->_onStopped == 0 );
	ASSERT_TRUE( app->isRunning() );

	app->started();
	//	status -> started
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app->_onStarting == 1 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 0 );
	ASSERT_TRUE( app->_onStopped == 0 );
	ASSERT_TRUE( app->isRunning() );

	// -> queueAll
	ctrl.disableApplications(true);

	//	status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == app );
	ASSERT_TRUE( ctrl._stopped[0].second == true );
	ASSERT_TRUE( app->_onStarting == 1 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 1 );
	ASSERT_TRUE( app->_onStopped == 1 );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->status() == tuner::app::status::waiting );

	//	Start with extension disabled!
	app->start();
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == app );
	ASSERT_TRUE( ctrl._stopped[0].second == true );
	ASSERT_TRUE( app->_onStarting == 1 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 1 );
	ASSERT_TRUE( app->_onStopped == 1 );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->status() == tuner::app::status::waiting );

	ctrl.disableApplications(false);
	//	ctrl -> start
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( ctrl._started[0] == app );
	ASSERT_TRUE( ctrl._started[1] == app );
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( app->_onStarting == 1 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 1 );
	ASSERT_TRUE( app->_onStopped == 1 );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->status() == tuner::app::status::waiting );

	app->starting();
	//	status -> starting
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( app->_onStarting == 2 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 1 );
	ASSERT_TRUE( app->_onStopped == 1 );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->status() == tuner::app::status::starting );

	app->stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( ctrl._stopped.size() == 2);
	ASSERT_TRUE( ctrl._stopped[0].first == app );
	ASSERT_TRUE( ctrl._stopped[0].second == true );
	ASSERT_TRUE( ctrl._stopped[1].first == app );
	ASSERT_TRUE( ctrl._stopped[1].second == false );
	ASSERT_TRUE( app->_onStarting == 2 );
	ASSERT_TRUE( app->_onStarted == 1 );
	ASSERT_TRUE( app->_onStopping == 2 );
	ASSERT_TRUE( app->_onStopped == 2 );
	ASSERT_FALSE( app->isRunning() );

	STOP_MGR;
}

TEST( Application, status_started_starting ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == &app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app._onStarting == 0 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.starting();
	//	status -> starting
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.started();
	//	status -> started
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 1 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.starting();
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 1 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 1 );
	ASSERT_TRUE( app._onStopping == 1 );
	ASSERT_TRUE( app._onStopped == 1 );
	ASSERT_FALSE( app.isRunning() );

	STOP_MGR;
}

TEST( Application, status_starting_stop ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	CheckApplication app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );

	// status -> waiting
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == &app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app._onStarting == 0 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.starting();
	//	status -> starting
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 0 );
	ASSERT_TRUE( app._onStopped == 0 );
	ASSERT_TRUE( app.isRunning() );

	app.stop();
	//	status -> mounted
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	ASSERT_TRUE( app._onStarting == 1 );
	ASSERT_TRUE( app._onStarted == 0 );
	ASSERT_TRUE( app._onStopping == 1 );
	ASSERT_TRUE( app._onStopped == 1 );
	ASSERT_FALSE( app.isRunning() );

	STOP_MGR;
}

TEST( Application, start_twice ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._started[0] == &app );
	ASSERT_TRUE( ctrl._stopped.size() == 0);
	ASSERT_TRUE( app.status() == tuner::app::status::waiting );

	//	Called when application was queued and can be started
	app.start();
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( app.status() == tuner::app::status::waiting );

	app.starting();
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( app.status() == tuner::app::status::starting );

	app.start();
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( app.status() == tuner::app::status::starting );

	app.stop();
	ASSERT_TRUE( ctrl._started.size() == 2);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	STOP_MGR;
}

TEST( Application, stop_twice ) {
	START_MGR;
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application app( ctrl.extension(), id );
	app.autoStart( true );
	app.mount( fs::temp_directory_path().string() );
	ASSERT_TRUE( ctrl._started.size() == 1);
	app.stop();
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	ASSERT_TRUE( ctrl._stopped[0].first == &app );
	ASSERT_TRUE( ctrl._stopped[0].second == false );
	app.stop();
	ASSERT_TRUE( ctrl._started.size() == 1);
	ASSERT_TRUE( ctrl._stopped.size() == 1);
	STOP_MGR;
}

