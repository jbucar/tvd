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

#include "downloaderextension.h"

OkServiceManager *DownloaderExtensionTest2::createManager( tuner::ResourceManager *res ) {
	return new OkServiceManager( res );
}

OkServiceManager *DownloaderExtensionTest::createManager( tuner::ResourceManager *res ) {
	return new OkServiceManager( res, 0x73b, 0x73b );
}

TEST_F( DownloaderExtensionTest, init ) {
	start( new tuner::DownloaderExtension( 0x10, 0x10, 0x10 ) );
	ASSERT_TRUE( mgr()->currentTS() == 0x73b );
	ASSERT_TRUE( mgr()->currentNit() == 0x73b );	
}

//	Send basic SDTT (tsID,nitID,srvID,downloadID,makerID,modelID must be equal and version need appply)
TEST_F( DownloaderExtensionTest, basic_low_apply ) {
	ASSERT_TRUE( _total == 0 );
	ASSERT_TRUE( _step == 0 );	
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );

	{	//	Probe carousel
		tuner::ID pid = 0x102;
		
		ASSERT_TRUE( _total == 0 );
		ASSERT_TRUE( _step == 0 );	
		mgr()->probe( pid, "download/DII.sec" );
		
		ASSERT_TRUE( _total == 0 );
		ASSERT_TRUE( _step == 0 );			
		mgr()->probe( pid, "download/upgrade.tar_000000.sec" );
	}

	ASSERT_TRUE( _info != NULL );
	ASSERT_TRUE( _total == 1269 );
	ASSERT_TRUE( _step == 1269 );	
}

TEST_F( DownloaderExtensionTest, basic_low_apply_service_ready_after ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	doServiceReady();
	probeCarousel();
	ASSERT_TRUE( _info != NULL );
}

TEST_F( DownloaderExtensionTest, basic_low_apply_maker_invalid ) {
	start( new tuner::DownloaderExtension( 0x14, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_low_apply_model_invalid ) {
	start( new tuner::DownloaderExtension( 0x14, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest2, basic_low_apply_invalid_ts ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_low_not_service ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );

	{	//	Mark ready a invalid service
		util::BYTE tag=0x10;
		tuner::ID ocPID = 0x102;
		tuner::ID srvID = 0xe763;
		tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );
		mgr()->readySrv( srv );
		delete srv;
	}
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_low_not_tag ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );

	{	//	Mark ready a invalid service
		util::BYTE tag=0x11;
		tuner::ID ocPID = 0x102;
		tuner::ID srvID = 0xe763;
		tuner::Service *srv = test::createService( srvID, 0x100, 0x101, ocPID, tag );
		mgr()->readySrv( srv );
		delete srv;
	}
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_hi_tag_not_found ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	probe( TS_PID_SDTT_HIGH, "sdtt_base.sec" );
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_low_not_apply ) {
	start( new tuner::DownloaderExtension( 0x10, 0x10, 0x10 ) );
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	ASSERT_TRUE( _info == NULL );
}

TEST_F( DownloaderExtensionTest, basic_hi_not_apply ) {
	start( new tuner::DownloaderExtension( 0x10, 0x10, 0x10 ) );
	probe( TS_PID_SDTT_HIGH, "sdtt_base.sec" );
	ASSERT_TRUE( _info == NULL );
}

//	Send 2 SDTT with same (maker/model)
TEST_F( DownloaderExtensionTest, two_sdtt_same_but_chg_version ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probeCarousel();
	ASSERT_TRUE( _info != NULL );
	_info = NULL;
	probe( TS_PID_SDTT_LOW, "download/sdtt_version2.sec" );
	probeCarousel();
	ASSERT_TRUE( _info != NULL );
}

TEST_F( DownloaderExtensionTest, two_sdtt_same_conflict ) {
	start( new tuner::DownloaderExtension( 0x13, 0x01, 0x01 ) );
	doServiceReady();
	probe( TS_PID_SDTT_LOW, "sdtt_base.sec" );
	probe( TS_PID_SDTT_LOW, "download/sdtt_version2.sec" );
	probeCarousel();
	ASSERT_TRUE( _info != NULL );
}

