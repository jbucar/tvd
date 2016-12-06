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
#include "nit.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/nit.h"
#include "../../../src/demuxer/psi/nitdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include <boost/bind.hpp>
#include <util/buffer.h>

NitTest::NitTest() {
}

NitTest::~NitTest() {
}

void NitTest::onNit( const boost::shared_ptr<tuner::Nit> &nit ) {
	_nit = nit;
}

tuner::PSIDemuxer *NitTest::createDemux( ) {
	return tuner::createDemuxer<tuner::NITDemuxer, tuner::Nit > (
			pid( ),
			boost::bind( &NitTest::onNit, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void NitTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_nit.reset();
}

tuner::ID NitTest::pid() const {
	return 0x010; // ?????
}

TEST_F( NitTest, create_demux ) {
}

TEST_F( NitTest, nit_ver0_sec0_lsn0 ) {
	probeSection( "nit_ver0_sec0_lsn0.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( _nit->networkID( ) == 0x073f );
	ASSERT_TRUE( _nit->version( ) == 0 );
	ASSERT_TRUE( _nit->elements( ).size( ) == 1 );
}

void testNetworkDescriptor( tuner::desc::Descriptors &desc, const std::string &nameToTest ) {
	// Check network name descriptor
	std::string name;
	ASSERT_TRUE( DESC_PARSE( desc, network_name, name ) );
	ASSERT_EQ( name, nameToTest );
}

TEST_F( NitTest, nit_tc_cinco_networkname_descriptor ) {
	// test network_name from network_descriptor_loop
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( _nit->networkID( ) == 0x73b );
	ASSERT_TRUE( _nit->version( ) == 1 );

	tuner::desc::Descriptors &desc = _nit->descriptors();
	ASSERT_TRUE( false == desc.empty() );
	testNetworkDescriptor( desc, "LIFIATVD" );
	ASSERT_FALSE( HasFatalFailure() );

	std::vector<tuner::Nit::ElementaryInfo> &elems = _nit->elements( );
	ASSERT_TRUE( elems.size( ) == 1 );
}

TEST_F( NitTest, nit_tc_cinco_system_management_descriptor ) {
	// test system_management from network_descriptor_loop
	// de OpenCaster systemID se compone de:
	// byte#1=broadcasting_identifier
	// byte#2=additional_broadcasting_information
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );

	tuner::desc::Descriptors &desc = _nit->descriptors( );
	tuner::desc::SystemManagementDescriptor sys_desc;
	ASSERT_TRUE( DESC_PARSE( desc, system_management, sys_desc ) );
	ASSERT_TRUE( sys_desc.systemID == 0x301 );
}

TEST_F( NitTest, nit_tc_cinco_service_list_descriptor ) {
	// tag=service_list=0x41
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );

	std::vector<tuner::Nit::ElementaryInfo> &elems = _nit->elements( );
	ASSERT_TRUE( _nit->elements().size() == 1 );
	tuner::desc::Descriptors &desc = elems[0].descriptors;

	tuner::desc::ServiceListDescriptor service_descriptor;
	ASSERT_TRUE( DESC_PARSE( desc, service_list, service_descriptor ) );
	ASSERT_TRUE( service_descriptor.size( ) == 5 );

	// tvd_service_id_sd1 = 0xe760
	// service_type = 1
	ASSERT_TRUE( service_descriptor[0].first == 0xe760 );
	ASSERT_TRUE( service_descriptor[0].second == 1 );

	// tvd_service_id_sd2 = 0xe761
	// service_type = 1
	ASSERT_TRUE( service_descriptor[1].first == 0xe761 );
	ASSERT_TRUE( service_descriptor[1].second == 1 );

	// tvd_service_id_sd3 = 0xe762
	// service_type = 1
	ASSERT_TRUE( service_descriptor[2].first == 0xe762 );
	ASSERT_TRUE( service_descriptor[2].second == 1 );

	// tvd_service_id_sd4 = 0xe763
	// service_type = 1
	ASSERT_TRUE( service_descriptor[3].first == 0xe763 );
	ASSERT_TRUE( service_descriptor[3].second == 1 );

	// tvd_service_id_sd5 = 0xe764
	// service_type = 1
	ASSERT_TRUE( service_descriptor[4].first == 0xe764 );
	ASSERT_TRUE( service_descriptor[4].second == 1 );
}

TEST_F( NitTest, nit_tc_cinco_terrestrial_delivery_system_descriptor ) {
	// area_code = 1341, guard_interval = 0x01, transmission_mode = 0x02,
	// frequencies = OpenCaster=533MHz, difiere del valor real en el .sec
	// tag=terrestrial_delivery_system=0xfa
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );

	std::vector<tuner::Nit::ElementaryInfo> &elems = _nit->elements( );
	ASSERT_TRUE( _nit->elements().size() == 1 );
	tuner::desc::Descriptors &desc = elems[0].descriptors;
	tuner::desc::TerrestrialDeliverySystemDescriptor terrestrial_descriptor;
	ASSERT_TRUE( DESC_PARSE( desc, terrestrial_delivery_system, terrestrial_descriptor ) );

	ASSERT_TRUE( terrestrial_descriptor.area == 1341 );
	ASSERT_TRUE( terrestrial_descriptor.interval == 0x01 );
	ASSERT_TRUE( terrestrial_descriptor.mode == 0x02 );

	ASSERT_TRUE( terrestrial_descriptor.frequencies.size( ) == 1 );
	ASSERT_TRUE( terrestrial_descriptor.frequencies[0] == 0xe94 );
}

TEST_F( NitTest, nit_tc_cinco_partial_reception_descriptor ) {
	// tc_cinco tiene este descriptor vacio
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );

	std::vector<tuner::Nit::ElementaryInfo> &elems = _nit->elements( );
	ASSERT_TRUE( _nit->elements().size() == 1 );
	tuner::desc::Descriptors &desc = elems[0].descriptors;

	tuner::desc::PartialReceptionDescriptor partial_descriptor;
	ASSERT_TRUE( DESC_PARSE( desc, partial_reception, partial_descriptor ) );
	ASSERT_TRUE( partial_descriptor.size( ) == 0 );
}

TEST_F( NitTest, nit_tc_cinco_transport_stream_information_descriptor ) {
	// tag=ts_information=0xcd
	probeSection( "tc_cinco_nit1.sec" );
	ASSERT_TRUE( _nit != NULL );

	std::vector<tuner::Nit::ElementaryInfo> &elems = _nit->elements( );
	ASSERT_TRUE( _nit->elements( ).size() == 1 );
	tuner::desc::Descriptors &desc = elems[0].descriptors;

	tuner::desc::TransportStreamInformationDescriptor ts_info_desc;
	ASSERT_TRUE( DESC_PARSE( desc, ts_information, ts_info_desc ) );
	ASSERT_TRUE( ts_info_desc.remoteControlKeyID == 0x05 );
	ASSERT_TRUE( ts_info_desc.name == "LIFIATVP" );

	std::vector<tuner::desc::TransmissionTypeInfo>trans = ts_info_desc.transmissions;
	ASSERT_TRUE( trans.size( ) == 2 );
	const tuner::desc::TransmissionTypeInfo &trans0 = trans[0];
	const tuner::desc::TransmissionTypeInfo &trans1 = trans[1];

	ASSERT_TRUE( trans0.type == 0x0F );
	ASSERT_TRUE( trans0.services.size( ) == 5 );
	ASSERT_TRUE( trans0.services[0] == 0xe760 );
	ASSERT_TRUE( trans0.services[1] == 0xe761 );
	ASSERT_TRUE( trans0.services[2] == 0xe762 );
	ASSERT_TRUE( trans0.services[3] == 0xe763 );
	ASSERT_TRUE( trans0.services[4] == 0xe764 );

	ASSERT_TRUE( trans1.type == 0xAF );
	ASSERT_TRUE( trans1.services.size( ) == 0 );
}

TEST_F( NitTest, nit_change_version ) {
	probeSection( "nit_ver0_cni1.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _nit->version( ) == 0 );
	ASSERT_EQ( _nit->elements().size(), 1 );

	util::BYTE tagsToCheck[] = { 0x40, 0xFE };
	ASSERT_TRUE( CHECK_TAGS(_nit->descriptors(), tagsToCheck) );

	probeSection( "nit_ver0_cni1.sec" );
	ASSERT_FALSE( _nit );
	ASSERT_TRUE( wasExpired( ) == 0 );

	probeSection( "nit_ver1_cni1.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( wasExpired( ) == 1 );
	ASSERT_TRUE( _nit->version( ) == 1 );
	ASSERT_EQ( _nit->elements().size(), 1 );
	ASSERT_TRUE( CHECK_TAGS(_nit->descriptors(), tagsToCheck) );

	probeSection( "nit_ver1_cni1.sec" );
	ASSERT_FALSE( _nit );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( NitTest, nit_change_version_cni ) {
	probeSection( "nit_ver0_cni1.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _nit->version( ) == 0 );

	probeSection( "nit_ver1_cni0.sec" );
	ASSERT_FALSE( _nit );
	ASSERT_TRUE( wasExpired( ) == 1 );

	probeSection( "nit_ver1_cni1.sec" );
	ASSERT_TRUE( _nit != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _nit->version( ) == 1 );
}

TEST_F( NitTest, nit_syntax_indicator ) {
	probeSection( "nit_ver0_cni1_synt.sec" );
	ASSERT_FALSE( _nit );
}

TEST_F( NitTest, nit_invalid_tableid ) {
	probeSection( "nit_invalid_tid.sec" );
	ASSERT_FALSE( _nit );
}

TEST_F( NitTest, nit_multiple_sections ) {
	probeSection( "section_0010_0040_0001_000d_00_03.sec" );
	ASSERT_FALSE( _nit );
	probeSection( "section_0010_0040_0001_000d_01_03.sec" );
	ASSERT_FALSE( _nit );
	probeSection( "section_0010_0040_0001_000d_02_03.sec" );
	ASSERT_FALSE( _nit );
	probeSection( "section_0010_0040_0001_000d_03_03.sec" );
	ASSERT_TRUE( _nit != NULL );
}

