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
#include "../../../../../src/service/extension/application/factories/aitfactory.h"
#include "../../../../../src/service/extension/application/factories/profile/ginga-ncl/nclprofile.h"
#include "../../../../../src/service/extension/application/factories/profile/ginga-ncl/nclapplication.h"

#define OC_A_TAG 0xa
#define OC_A_PID 0x200

#define OC_B_TAG 0xb
#define OC_B_PID 0x201

#define OC_C_TAG 0xc
#define OC_C_PID 0x202

#define OC_D_TAG 0xd
#define OC_D_PID 0x203

#define OC_E_TAG 0xe
#define OC_E_PID 0x204

#define SRV_ID      10
#define SRV_PID     0x100
#define SRV_PCR_PID 0x101

#define AIT_PID  0x103
#define OC_DEFAULT_TAG OC_C_TAG
#define OC_DEFAULT_PID OC_C_PID


class NCLAppCtrl : public FakeAppCtrl {
public:
	NCLAppCtrl( void ) {}
	virtual ~NCLAppCtrl( void ) {}

protected:
	//	Create extension
	virtual void setupFactories( tuner::app::Extension *ext ) {
		tuner::app::AitFactory *ait = new tuner::app::AitFactory( ext );
		ait->addProfile( new tuner::app::NCLProfile( ext ) );
		ext->addFactory( ait );
	}
};

class NCLApplicationTest : public ApplicationExtensionTest {
public:
	NCLApplicationTest( void ) {}
	virtual ~NCLApplicationTest( void ) {}

	virtual void SetUp() {
		ApplicationExtensionTest::SetUp();
		_srv = createAit();
		_srv->state( tuner::service::state::ready );
		mgr()->startSrv( _srv );
	}

	virtual void TearDown() {
		mgr()->stopSrv( _srv );
		delete _srv;

		//	Check application not exist!
		ASSERT_TRUE( getApps().size() == 0 );

		ApplicationExtensionTest::TearDown();
	}

	void probe( const char *file ) {
		mgr()->probe( AIT_PID, file );
	}

	void probeAllOC() {
		probeOC( OC_A_PID, OC_A_TAG );
		probeOC( OC_B_PID, OC_B_TAG );
		probeOC( OC_C_PID, OC_C_TAG );
		probeOC( OC_D_PID, OC_D_TAG );
		probeOC( OC_E_PID, OC_E_TAG );
	}

	virtual FakeAppCtrl *createCtrl( void ) {
		return new NCLAppCtrl();
	}

	tuner::Service *createAit( void ) {
		std::vector<tuner::ElementaryInfo> elems;

		//	Objects carousels
		test::addTypeB( elems, OC_A_PID, OC_A_TAG );
		test::addTypeB( elems, OC_B_PID, OC_B_TAG );
		test::addTypeB( elems, OC_C_PID, OC_C_TAG );
		test::addTypeB( elems, OC_D_PID, OC_D_TAG );
		test::addTypeB( elems, OC_E_PID, OC_E_TAG );

		//	Add streams
		test::addStream( elems, PSI_ST_TYPE_AIT, AIT_PID );
		tuner::Pmt *pmt = test::createPmt( SRV_ID, SRV_PID, SRV_PCR_PID, elems );
		return test::createService( SRV_ID, SRV_PID, pmt );
	}

protected:
	tuner::Service *_srv;
};

TEST_F( NCLApplicationTest, init ) {
}

TEST_F( NCLApplicationTest, ait_simple ) {
	probe( "ait_simple_ver0_sec0.sec" );
	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
}

TEST_F( NCLApplicationTest, ait_single_app ) {
	probe( "ait_single_app_ver1_sec0_lsn0.sec" );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	tuner::app::Application *app = apps[0];

	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x60 );
	ASSERT_TRUE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "COCINEROS" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );
}

TEST_F( NCLApplicationTest, ait_multi_app_ver1 ) {
	Applications apps;

	probe( "ait_multiple_app_10_11_12_ver1.sec" );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );

	tuner::app::NCLApplication *app = (tuner::app::NCLApplication *)apps[0];
	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x10 );
	ASSERT_FALSE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "app_a" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->script() == fs::path("/app_a/main_a.ncl").make_preferred().string() );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );

	app = (tuner::app::NCLApplication *)apps[1];
	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x11 );
	ASSERT_FALSE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "app_b" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->script() == fs::path("/app_b/main_b.ncl").make_preferred().string() );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );

	app = (tuner::app::NCLApplication *)apps[2];
	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x12 );
	ASSERT_FALSE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_FALSE( app->isMounted() );
	ASSERT_FALSE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "app_c" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->script() == fs::path("/app_c/main_c.ncl").make_preferred().string() );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );

	apps.clear();
}

TEST_F( NCLApplicationTest, ait_multi_app_ver2 ) {
	Applications apps;
	probe( "ait_multiple_app_10_12_13_ver2.sec" );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x12 );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x13 );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_update_multi_app ) {
	Applications apps;

	probe( "ait_multiple_app_10_11_12_ver1.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	apps.clear();

	probe( "ait_multiple_app_10_12_13_ver2.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x12 );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x13 );
	apps.clear();
}

TEST_F( NCLApplicationTest, ncl_start_stop_start ) {
	Applications apps;

	probe( "ait_multiple_app_ver01_10ast_11prs_12prf.sec" );
	probeOC( OC_DEFAULT_PID, OC_DEFAULT_TAG );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	tuner::app::ApplicationID id( apps[0]->appID() );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();

	//	Stop
	extension()->stop( id );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_FALSE( apps[0]->isRunning() );

	//	Start
	extension()->start( id );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
}

TEST_F( NCLApplicationTest, ait_basic_app_life ) {
	Applications apps;

	probe( "ait_multiple_app_ver01_10ast_11prs_12prf.sec" );
	probeOC( OC_DEFAULT_PID, OC_DEFAULT_TAG );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();

	probe( "ait_multiple_app_ver02_10des_11prs_12prf.sec" );
	probeOC( OC_DEFAULT_PID, OC_DEFAULT_TAG );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_FALSE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_app_life_2 ) {
	Applications apps;

	probe( "ait_multiple_app_ver01_10ast_11prs_12prf.sec" );
	probeOC( OC_DEFAULT_PID );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();

	probe( "ait_multiple_app_ver02_10des_11prs_12prf.sec" );
	probeOC( OC_DEFAULT_PID );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_FALSE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();

	probe( "ait_multiple_app_ver03_11prs_12prf_13ast.sec" );
	probeOC( OC_DEFAULT_PID );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[2]->isRunning() );
	apps.clear();

	probe( "ait_multiple_app_ver04_11prs_12prf_13kll.sec" );
	probeOC( OC_DEFAULT_PID );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 3 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x13 );
	ASSERT_FALSE( apps[2]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_without_kill ) {
	probe( "ait_single_app_ver1_sec0_lsn0.sec" );
	probeOC( OC_C_PID, OC_C_TAG );

	Applications apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	tuner::app::Application *app = apps[0];

	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x60 );
	ASSERT_TRUE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "COCINEROS" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );

	probe( "ait_single_app2_ver2_sec0_lsn0.sec" );
	probeOC( OC_C_PID, OC_C_TAG );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	app = apps[0];

	ASSERT_TRUE( app->appID().orgID() == 0xa );
	ASSERT_TRUE( app->appID().appID() == 0x69 );
	ASSERT_TRUE( app->autoStart() );
	ASSERT_FALSE( app->autoDownload() );
	ASSERT_FALSE( app->readOnly() );
	ASSERT_TRUE( app->priority() == 1 );
	ASSERT_TRUE( app->isMounted() );
	ASSERT_TRUE( app->isRunning() );
	ASSERT_TRUE( app->service() == SRV_ID );
	ASSERT_TRUE( app->componentTag() == OC_DEFAULT_TAG );
	ASSERT_TRUE( app->name() == "COCINEROS" );
	ASSERT_TRUE( app->language() == "deu" );
	ASSERT_TRUE( app->visibility() == tuner::app::visibility::both );
}

TEST_F( NCLApplicationTest, ait_stop_wildcard_on_org ) {
	Applications apps;

	probe( "ait_stop_org_ver0.sec" );
	probeAllOC();
	apps=getApps();
	ASSERT_TRUE( apps.size() == 4 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );
	ASSERT_TRUE( apps[3]->appID().appID() == 0x13 );
	ASSERT_FALSE( apps[3]->isRunning() );
	apps.clear();

	probe( "ait_stop_org_ver2.sec" );
	probeAllOC();
	apps=getApps();
	ASSERT_TRUE( apps.size() == 4 );

	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_FALSE( apps[0]->isRunning() );

	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_FALSE( apps[1]->isRunning() );

	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_FALSE( apps[2]->isRunning() );

	ASSERT_TRUE( apps[3]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[3]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_external_authorisation ) {
	Applications apps;

	probe( "ait_ext_app_ver0.sec" );
	probeAllOC();
	apps=getApps();
	ASSERT_TRUE( apps.size() == 2 );

	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[0]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_FALSE( apps[1]->isBoundToService( SRV_ID ) );
	apps.clear();

	//	Remove all applications but app 11 in external application auth!
	probe( "ait_ext_app_ver1.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_FALSE( apps[0]->isBoundToService( SRV_ID ) );
	apps.clear();

	probe( "ait_ext_app_ver2.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_multi_app_external_authorisation ) {
	Applications apps;

	probe( "ait_ext02_app_ver0.sec" );
	probeAllOC();
	apps=getApps();
	ASSERT_TRUE( apps.size() == 4 );

	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[0]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_FALSE( apps[1]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_TRUE( apps[2]->isRunning() );
	ASSERT_TRUE( apps[2]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[3]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[3]->isRunning() );
	ASSERT_FALSE( apps[3]->isBoundToService( SRV_ID ) );
	apps.clear();

	//	Remove all applications but app 11 in external application auth!
	probe( "ait_ext02_app_ver1.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 2 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_FALSE( apps[0]->isBoundToService( SRV_ID ) );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_FALSE( apps[1]->isBoundToService( SRV_ID ) );
	apps.clear();

	probe( "ait_ext02_app_ver2.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_multi_app_wildcard_external_authorisation ) {
	Applications apps;

	probe( "ait_ext02_app_ver0.sec" );
	probeAllOC();
	apps=getApps();
	ASSERT_TRUE( apps.size() == 4 );

	ASSERT_TRUE( apps[0]->appID().appID() == 0x10 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_TRUE( apps[0]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[1]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_FALSE( apps[1]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[2]->appID().appID() == 0x12 );
	ASSERT_TRUE( apps[2]->isRunning() );
	ASSERT_TRUE( apps[2]->isBoundToService( SRV_ID ) );

	ASSERT_TRUE( apps[3]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[3]->isRunning() );
	ASSERT_FALSE( apps[3]->isBoundToService( SRV_ID ) );
	apps.clear();

	//	Remove all applications but app 11 in external application auth!
	probe( "ait_ext02w_app_ver1.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 2 );
	ASSERT_TRUE( apps[0]->appID().appID() == 0x11 );
	ASSERT_TRUE( apps[0]->isRunning() );
	ASSERT_FALSE( apps[0]->isBoundToService( SRV_ID ) );
	ASSERT_TRUE( apps[1]->appID().appID() == 0x13 );
	ASSERT_TRUE( apps[1]->isRunning() );
	ASSERT_FALSE( apps[1]->isBoundToService( SRV_ID ) );
	apps.clear();

	probe( "ait_ext02_app_ver2.sec" );
	apps=getApps();
	ASSERT_TRUE( apps.size() == 0 );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_autostart_to_present ) {
	Applications apps;

	probe( "ait_controlcode1_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode2_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	//	Check application mounted and running
	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_autostart_to_autostart ) {
	Applications apps;

	probe( "ait_controlcode1_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode1_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	//	Check application mounted and running
	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_autostart_to_destroy ) {
	Applications apps;

	probe( "ait_controlcode1_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode3_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_autostart_to_kill ) {
	Applications apps;

	probe( "ait_controlcode1_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode4_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_autostart_to_prefetch ) {
	Applications apps;

	probe( "ait_controlcode1_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode5_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_present_to_autostart ) {
	Applications apps;

	probe( "ait_controlcode2_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode1_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_present_to_destroy ) {
	Applications apps;

	probe( "ait_controlcode2_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode3_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_present_to_kill ) {
	Applications apps;

	probe( "ait_controlcode2_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode4_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_present_to_prefetch ) {
	Applications apps;

	probe( "ait_controlcode2_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode5_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_destroy_to_autostart ) {
	Applications apps;

	probe( "ait_controlcode3_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode1_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_destroy_to_kill ) {
	Applications apps;

	probe( "ait_controlcode3_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode4_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );

	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}


TEST_F( NCLApplicationTest, ait_ctrl_code_destroy_to_prefetch ) {
	Applications apps;

	probe( "ait_controlcode3_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode5_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_kill_to_kill ) {
	Applications apps;

	probe( "ait_controlcode4_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode4_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_kill_to_autostart ) {
	Applications apps;

	probe( "ait_controlcode4_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode1_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_kill_to_prefetch ) {
	Applications apps;

	probe( "ait_controlcode4_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode5_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_kill_to_present ) {
	Applications apps;

	probe( "ait_controlcode4_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode2_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_kill_to_destroy ) {
	Applications apps;

	probe( "ait_controlcode4_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode3_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );

	ASSERT_FALSE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_prefetch_to_autostart ) {
	Applications apps;

	probe( "ait_controlcode5_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode1_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_TRUE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_prefetch_to_present ) {
	Applications apps;

	probe( "ait_controlcode5_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode2_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_prefetch_to_destroy ) {
	Applications apps;

	probe( "ait_controlcode5_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode3_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_prefetch_to_kill ) {
	Applications apps;

	probe( "ait_controlcode5_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode4_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

TEST_F( NCLApplicationTest, ait_ctrl_code_prefetch_to_prefetch ) {
	Applications apps;

	probe( "ait_controlcode5_ver5.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();

	probe( "ait_controlcode5_ver6.sec" );
	probeOC( OC_DEFAULT_PID );

	apps=getApps();
	ASSERT_TRUE( apps.size() == 1 );
	ASSERT_TRUE( apps[0]->isMounted() );
	ASSERT_FALSE( apps[0]->isRunning() );
	apps.clear();
}

