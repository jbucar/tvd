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
#include "sdt.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/sdt.h"
#include "../../../src/demuxer/psi/sdtdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include "../../../src/service/types.h"
#include <boost/bind.hpp>
#include <util/buffer.h>

SdtTest::SdtTest() {
}

SdtTest::~SdtTest() {
}

void SdtTest::onSdt( const boost::shared_ptr<tuner::Sdt> &sdt ) {
	_sdt = sdt;
}

tuner::PSIDemuxer *SdtTest::createDemux( ) {
	return tuner::createDemuxer<tuner::SDTDemuxer, tuner::Sdt > (
			pid( ),
			boost::bind( &SdtTest::onSdt, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void SdtTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe( );
	_sdt.reset();
}

tuner::ID SdtTest::pid() const {
	return 0x011;
}

tuner::PSIDemuxer *SdtTestOther::createDemux() {
	tuner::SDTDemuxer *demux = new tuner::SDTDemuxer( pid(), false );
	demux->onParsed( boost::bind( &SdtTest::onSdt, this, _1 ) );
	demux->onExpired( boost::bind( &SectionDemuxerTest::onExpired, this, _1 ) );
	return demux;
}

TEST_F( SdtTest, create_demux ) {
}

TEST_F( SdtTest, sdt_ver1 ) {
	probeSection( "sdt_ver1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( _sdt->version( ) == 1 );
	ASSERT_TRUE( _sdt->tsID( ) == 0x73a );
	ASSERT_TRUE( _sdt->nitID( ) == 0x073a );
}

TEST_F( SdtTest, sdt_service_test ) {
	probeSection( "sdt_ver1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( _sdt->services( ).size( ) > 0 );

	const tuner::Sdt::Service &service = _sdt->services( )[0];
	ASSERT_TRUE( service.serviceID == 0xe740 );
	ASSERT_TRUE( service.eitSchedule == false );
	ASSERT_TRUE( service.eitPresentFollowing == false );
	ASSERT_TRUE( service.status == tuner::service::status::running );
	ASSERT_FALSE( service.caMode );
}

void testServiceDescriptor( const tuner::desc::Descriptors &desc, const tuner::service::Type type, const std::string provider, const std::string name ) {
	tuner::desc::ServiceDescriptor serv;
	ASSERT_TRUE( DESC_PARSE( desc, service, serv ) );
	ASSERT_TRUE( serv.type == type);
	ASSERT_TRUE( serv.name == name );
	ASSERT_TRUE( serv.provider == provider );
}

TEST_F( SdtTest, sdt_tc_cinco ) {
	probeSection( "tc_cinco_sdt1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( _sdt->services( ).size( ) == 5 );
	ASSERT_TRUE( _sdt->tsID( ) == 0x73b );
	ASSERT_TRUE( _sdt->nitID( ) == 0x73b );
	ASSERT_TRUE( _sdt->version( ) == 1 );

	//	1st service
	const tuner::Sdt::Service &service_sd0 = _sdt->services( )[0];
	ASSERT_TRUE( service_sd0.serviceID == 0xe760 );
	ASSERT_FALSE( service_sd0.eitSchedule );
	ASSERT_FALSE( service_sd0.eitPresentFollowing );
	ASSERT_TRUE( service_sd0.status == tuner::service::status::running );
	ASSERT_FALSE( service_sd0.caMode );

	const tuner::desc::Descriptors &desc0 = service_sd0.descriptors;
	testServiceDescriptor( desc0, 1, "", "LFA-SD1" );
	ASSERT_FALSE( HasFatalFailure() );

	//	2nd service
	const tuner::Sdt::Service &service_sd1 = _sdt->services( )[1];
	ASSERT_TRUE( service_sd1.serviceID == 0xe761 );
	ASSERT_FALSE( service_sd1.eitSchedule );
	ASSERT_FALSE( service_sd1.eitPresentFollowing );
	ASSERT_TRUE( service_sd1.status == tuner::service::status::running );
	ASSERT_FALSE( service_sd1.caMode );

	const tuner::desc::Descriptors &desc1 = service_sd1.descriptors;
	testServiceDescriptor( desc1, 1, "", "LFA-SD2" );
	ASSERT_FALSE( HasFatalFailure() );

	//	3rd service
	const tuner::Sdt::Service &service_sd2 = _sdt->services( )[2];
	ASSERT_TRUE( service_sd2.serviceID == 0xe762 );
	ASSERT_FALSE( service_sd2.eitSchedule );
	ASSERT_FALSE( service_sd2.eitPresentFollowing );
	ASSERT_TRUE( service_sd2.status == tuner::service::status::running );
	ASSERT_FALSE( service_sd2.caMode );

	const tuner::desc::Descriptors &desc2 = service_sd2.descriptors;
	testServiceDescriptor( desc2, 1, "", "LFA-SD3" );
	ASSERT_FALSE( HasFatalFailure() );

	//	4th service
	const tuner::Sdt::Service &service_sd3 = _sdt->services( )[3];
	ASSERT_TRUE( service_sd3.serviceID == 0xe763 );
	ASSERT_FALSE( service_sd3.eitSchedule );
	ASSERT_FALSE( service_sd3.eitPresentFollowing );
	ASSERT_TRUE( service_sd3.status == tuner::service::status::running );
	ASSERT_FALSE( service_sd3.caMode );

	const tuner::desc::Descriptors &desc3 = service_sd3.descriptors;
	testServiceDescriptor( desc3, 1, "", "LFA-SD4" );
	ASSERT_FALSE( HasFatalFailure() );

	//	5th service
	const tuner::Sdt::Service &service_sd4 = _sdt->services( )[4];
	ASSERT_TRUE( service_sd4.serviceID == 0xe764 );
	ASSERT_FALSE( service_sd4.eitSchedule );
	ASSERT_FALSE( service_sd4.eitPresentFollowing );
	ASSERT_TRUE( service_sd4.status == tuner::service::status::running );
	ASSERT_FALSE( service_sd4.caMode );

	const tuner::desc::Descriptors &desc4 = service_sd4.descriptors;
	testServiceDescriptor( desc4, 1, "", "LFA-SD5" );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdtTest, sdt_change_version ) {
	probeSection( "sdt_ver0_cni1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdt->version( ) == 0 );
	ASSERT_EQ( _sdt->services().size(), 1 );

	probeSection( "sdt_ver0_cni1.sec" );
	ASSERT_FALSE( _sdt );
	ASSERT_TRUE( wasExpired( ) == 0 );

	probeSection( "sdt_ver1_cni1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( wasExpired( ) == 1 );
	ASSERT_TRUE( _sdt->version( ) == 1 );
	ASSERT_EQ( _sdt->services().size(), 1 );

	probeSection( "sdt_ver1_cni1.sec" );
	ASSERT_FALSE( _sdt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( SdtTest, sdt_change_version_cni ) {
	probeSection( "sdt_ver0_cni1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdt->version( ) == 0 );

	probeSection( "sdt_ver1_cni0.sec" );
	ASSERT_FALSE( _sdt );
	ASSERT_TRUE( wasExpired( ) == 1 );

	probeSection( "sdt_ver1_cni1.sec" );
	ASSERT_TRUE( _sdt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdt->version( ) == 1 );
}

TEST_F( SdtTest, sdt_syntax_indicator_0 ) {
	probeSection( "sdt_ver0_cni1_synt.sec" );
	ASSERT_FALSE( _sdt );
}

TEST_F( SdtTest, sdt_invalid_tableid ) {
	probeSection( "sdt_invalid_tid.sec" );
	ASSERT_FALSE( _sdt );
}

TEST_F( SdtTestOther, sdt_multiple_sections ) {
	probeSection( "section_0011_0046_0002_0004_00_04.sec" );
	ASSERT_FALSE( _sdt );
	probeSection( "section_0011_0046_0002_0004_01_04.sec" );
	ASSERT_FALSE( _sdt );
	probeSection( "section_0011_0046_0002_0004_02_04.sec" );
	ASSERT_FALSE( _sdt );
	probeSection( "section_0011_0046_0002_0004_03_04.sec" );
	ASSERT_FALSE( _sdt );
	probeSection( "section_0011_0046_0002_0004_04_04.sec" );
	ASSERT_TRUE( _sdt != NULL );
}

