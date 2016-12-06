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
#include <util/keydefs.h>

OkApplication::OkApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id )
	: tuner::app::Application( ext, id )
{
	visibility( tuner::app::visibility::both );
}

OkApplication::OkApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id, const std::string &path )
	: tuner::app::Application( ext, id, path )
{
	visibility( tuner::app::visibility::both );
}

KeysApplication::KeysApplication( tuner::app::Extension *ext, const tuner::app::ApplicationID &id )
	: tuner::app::Application( ext, id )
{
	_dispatchedKeys=0;

	std::vector<util::key::type> keys;
	keys.push_back( util::key::red );
	keys.push_back( util::key::green );
	keys.push_back( util::key::yellow );
	reserveKeys(keys);
}

void KeysApplication::dispatchKeyImpl( util::key::type /*key*/, bool /*isUp*/ ) {
	_dispatchedKeys++;
}

int KeysApplication::dispatchedKeys() {
	return _dispatchedKeys;
}

std::string getScanDir( const std::string &mountPoint ) {
	fs::path mount(test::getTestRoot());
	mount /= "testScan";
	mount /= mountPoint;
	return mount.string();
}

TEST_F( ApplicationExtensionTest, init ) {
}

TEST_F( ApplicationExtensionTest, start_stop_service ) {
	tuner::ServiceID id( 0x10, 0x73b, 0x73b);
	tuner::Service *srv = new tuner::Service( 8, id, 0x100 );
	mgr()->startSrv( srv );
	delete srv;
}

TEST_F( ApplicationExtensionTest, basic_add_remove ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application *app = new tuner::app::Application( extension(), id, "/tmp" );

	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	apps.clear();

	extension()->remove( id );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
	apps.clear();
}

TEST_F( ApplicationExtensionTest, add_with_autostart ) {
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = new OkApplication( extension(), id, "/tmp" );
	app->autoStart( true );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( app->isRunning() );
}

TEST_F( ApplicationExtensionTest, stopAll ) {
	size_t max=3;
	Applications apps;

	for (size_t i=0; i<max; i++) {
		tuner::app::ApplicationID id(1,(tuner::ID) i);
		OkApplication *app = createApp( id, "/tmp" );
		app->autoStart( true );
		extension()->add( app );
	}
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max; i++) {
		ASSERT_TRUE( apps[i]->isRunning() );
	}
	apps.clear();

	extension()->stopAll();
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max; i++) {
		ASSERT_FALSE( apps[i]->isRunning() );
	}
	apps.clear();
}

TEST_F( ApplicationExtensionTest, stop_all_org ) {
	size_t max=4;
	Applications apps;

	for (size_t i=0; i<max-1; i++) {
		tuner::app::ApplicationID id(1,(tuner::ID) i);
		OkApplication *app = createApp( id, "/tmp" );
		app->autoStart( true );
		extension()->add( app );
	}
	{	//	Add other application in other organization
		tuner::app::ApplicationID id(2,1);
		OkApplication *app = createApp( id, "/tmp" );
		app->autoStart( true );
		extension()->add( app );
	}
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max; i++) {
		ASSERT_TRUE( apps[i]->isRunning() );
	}
	apps.clear();

	tuner::app::ApplicationID all_org_1(1,0xFFFF);
	extension()->stop( all_org_1 );
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max-1; i++) {
		ASSERT_FALSE( apps[i]->isRunning() );
	}
	ASSERT_TRUE( apps[max-1]->isRunning() );
	apps.clear();
}

TEST_F( ApplicationExtensionTest, stop_all_org_signed ) {
	size_t max=4;
	Applications apps;

	for (size_t i=0; i<max-1; i++) {
		tuner::app::ApplicationID id(1, (tuner::ID)(0x4000+i));
		OkApplication *app = createApp( id, "/tmp" );
		app->autoStart( true );
		extension()->add( app );
	}
	{	//	Add other application signed
		tuner::app::ApplicationID id(1,1);
		OkApplication *app = createApp( id, "/tmp" );
		app->autoStart( true );
		extension()->add( app );
	}
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max; i++) {
		ASSERT_TRUE( apps[i]->isRunning() );
	}
	apps.clear();

	tuner::app::ApplicationID all_org_signed(1,0xFFFE);
	extension()->stop( all_org_signed );
	apps=getApps();
	ASSERT_TRUE( apps.size() == max );
	for (size_t i=0; i<max-1; i++) {
		ASSERT_FALSE( apps[i]->isRunning() );
	}
	ASSERT_TRUE( apps[max-1]->isRunning() );
	apps.clear();
}

TEST_F( ApplicationExtensionTest, add_wildcard_all_apps ) {
	tuner::app::ApplicationID id(1,0xFFFF);
	OkApplication *app = createApp( id, "/tmp" );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( ApplicationExtensionTest, add_wildcard_all_signed ) {
	tuner::app::ApplicationID id(1,0xFFFE);
	OkApplication *app = createApp( id, "/tmp" );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( ApplicationExtensionTest, stop_service_with_app_bound ) {
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id, "/tmp" );
	app->autoStart( true );
	app->service( 10 );

	tuner::ServiceID srvID(1, 1, 10);
	tuner::Service *srv = new tuner::Service( 8, srvID, 12 );
	mgr()->startSrv( srv );

	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, stop_service_without_app_bound ) {
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id, "/tmp" );
	app->service( 11 );
	app->autoStart( true );

	tuner::ServiceID srvID(1, 1, 10);
	tuner::Service *srv = new tuner::Service( 8, srvID, 12 );
	mgr()->startSrv( srv );

	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, download_and_stop ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->autoStart( true );
	app->componentTag( tag );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, download_twice ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->autoStart( true );
	app->componentTag( tag );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send Object carousel
	probeOC( ocPID );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->isMounted() );

	extension()->download( id );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->isMounted() );

	mgr()->stopSrv( srv );
	delete srv;
}

TEST_F( ApplicationExtensionTest, update_carousel ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->autoStart( true );
	app->componentTag( tag );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send Object carousel
	probeOC( ocPID );

	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );

	//	Send again
	mgr()->probe( ocPID, "dsi_v2_sample_tc001.sec" );
	mgr()->probe( ocPID, "dii_v2_sample_tc001.sec" );
	mgr()->probe( ocPID, "ddb01_v2_sample_tc001.sec" );
	mgr()->probe( ocPID, "ddb02_v2_sample_tc001.sec" );

	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, download_without_autostart ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->autoDownload( true );
	app->componentTag( tag );
	app->autoStart( false );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_TRUE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, stop_when_download ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->autoDownload( true );
	app->autoStart( true );
	app->componentTag( tag );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	app->stopDownload();

	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, stop_srv_unbound_when_download ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->autoDownload( true );
	app->autoStart( true );
	app->componentTag( tag );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	mgr()->stopSrv( srv );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, start_app_without_auto ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xc;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->componentTag( tag );
	app->autoDownload( false );
	app->autoStart( false );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );

	//	Start application
	extension()->start( id );

	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_TRUE( app->autoStart() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );

	mgr()->stopSrv( srv );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );

	delete srv;
}

TEST_F( ApplicationExtensionTest, add_with_needDownload_without_valid_tag ) {
	//	createService( tuner::ID srvID, tuner::ID srvPID, tuner::ID pcrPID, tuner::ID ocPID, util::BYTE ocTAG ) {
	util::BYTE tag=0xb;
	tuner::ID ocPID = 0x102;
	tuner::ID srvID = 10;
	tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );

	mgr()->startSrv( srv );

	//	Add application
	tuner::app::ApplicationID id(1,1);
	OkApplication *app = createApp( id );
	app->service( srvID );
	app->componentTag( tag );
	app->autoDownload( true );
	app->autoStart( true );
	extension()->add( app );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_FALSE( app->isMounted() );

	//	Send object carousel
	probeOC( ocPID );

	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_both_as_user ) {
	tuner::app::ApplicationID id(1,1);
	extension()->add( new tuner::app::Application( extension(), id, "/tmp" ) );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->appID() == id );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_both_as_no_user ) {
	tuner::app::ApplicationID id(1,1);
	extension()->add( new tuner::app::Application( extension(), id, "/tmp" ) );

	Applications apps=getApps( false );
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->appID() == id );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_api_as_user ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application *app = new tuner::app::Application( extension(), id, "/tmp" );
	app->visibility( tuner::app::visibility::api );
	extension()->add( app );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_api_as_no_user ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application *app = new tuner::app::Application( extension(), id, "/tmp" );
	app->visibility( tuner::app::visibility::api );
	extension()->add( app );

	Applications apps=getApps( false );
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->appID() == id );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_none_as_user ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application *app = new tuner::app::Application( extension(), id, "/tmp" );
	app->visibility( tuner::app::visibility::none );
	extension()->add( app );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( ApplicationExtensionTest, list_app_with_visibility_none_as_no_user ) {
	tuner::app::ApplicationID id(1,1);
	tuner::app::Application *app = new tuner::app::Application( extension(), id, "/tmp" );
	app->visibility( tuner::app::visibility::none );
	extension()->add( app );

	Applications apps=getApps(false);
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( ApplicationExtensionTest, scan_basic ) {
	extension()->addFactory( new OkApplicationFactory( extension() ) );

	extension()->scan( getScanDir( "test1" ), 3 );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );

	//	Check that not remove scanned applications when stop extension
	mgr()->stop();

	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
}

TEST_F( ApplicationExtensionTest, scan_two_mountpoints ) {
	extension()->addFactory( new OkApplicationFactory( extension() ) );

	extension()->scan( getScanDir( "test1" ), 3 );
	extension()->scan( getScanDir( "test2" ), 3 );

	Applications apps=getApps();
	ASSERT_EQ( apps.size(), 4 );

	for( unsigned int i=0; i < apps.size(); i++ ) {
		ASSERT_EQ( apps.at(i)->appID().appID(), i+1 );
	}

	//	Check that not remove scanned applications when stop extension
	mgr()->stop();

	apps=getApps();
	ASSERT_TRUE( apps.size() == 4 );
}

TEST_F( ApplicationExtensionTest, scan_basic_repeat ) {
	Applications apps;
	extension()->addFactory( new OkApplicationFactory( extension() ) );

	extension()->scan( getScanDir( "test1" ), 3 );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );

	extension()->removeScanned();
	extension()->scan( getScanDir( "test1" ), 3 );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
}

TEST_F( ApplicationExtensionTest, scan_max_depth_1 ) {
	Applications apps;
	extension()->addFactory( new OkApplicationFactory( extension() ) );

	extension()->scan( getScanDir( "test1" ), 1 );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 2 );
}

TEST_F( ApplicationExtensionTest, reserve_keys ) {
	tuner::app::ApplicationID id(1,1);
	KeysApplication app( extension(), id );

	extension()->add(&app);
	ASSERT_TRUE( app.dispatchedKeys() == 0 );

	extension()->dispatchKey( id, util::key::red, true );
	extension()->dispatchKey( id, util::key::blue, true );
	ASSERT_TRUE( app.dispatchedKeys() == 1 );

	extension()->dispatchKey( id, util::key::yellow, true );
	extension()->dispatchKey( id, util::key::green, true );
	ASSERT_TRUE( app.dispatchedKeys() == 3 );
}
