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
#include "pmt.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/pmt.h"
#include "../../../src/demuxer/psi/pmtdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include <boost/bind.hpp>
#include <util/buffer.h>

PmtTest::PmtTest() {
}

PmtTest::~PmtTest() {
}

void PmtTest::onPmt( const boost::shared_ptr<tuner::Pmt> &pmt ) {
	_pmt = pmt;
}

tuner::PSIDemuxer *PmtTest::createDemux( ) {
	return tuner::createDemuxer<tuner::PMTDemuxer, tuner::Pmt > (
			pid( ),
			boost::bind( &PmtTest::onPmt, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void PmtTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe( );
	_pmt.reset();
}

tuner::ID PmtTest::pid() const {
	return 0x407;
}

TEST_F( PmtTest, create_demux ) {
}

TEST_F( PmtTest, pmt_basic_test_ver1_cni1 ) {
	// basic test with PCR=264, video=2064, audio=2068
	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 1 );
	ASSERT_TRUE( _pmt->pcrPID( ) == 2064 );

	ASSERT_TRUE( _pmt->elements( ).size( ) == 2 );
	const tuner::ElementaryInfo &info0 = _pmt->elements( )[0];
	ASSERT_TRUE( info0.pid == 2064 );

	const tuner::ElementaryInfo &info1 = _pmt->elements( )[1];
	ASSERT_TRUE( info1.pid == 2068 );
}

TEST_F( PmtTest, pmt_basic_test_ver2_cni0 ) {
	// basic test with current_next_indicator=0
	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni0.sec" );
	ASSERT_FALSE( _pmt );
}

TEST_F( PmtTest, pmt_empty_ver1 ) {
	// empty stream_loop_item
	probeSection( "pmt_empty_ver1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 1 );
	ASSERT_TRUE( _pmt->elements( ).size( ) == 0 );
}

TEST_F( PmtTest, pmt_basic_test_ver2 ) {
	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 2 );
}

TEST_F( PmtTest, pmt_invalid_sections ) {
	// test if section_number and last_section_number != 0x00
	probeSection( "pmt_sec_invalid_sec2_lsn2_ver2.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( PmtTest, pmt_repeated_pid ) {
	// elementary streams with repeated PID
	probeSection( "pmt_repeated_pid2068_ver1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->elements( ).size( ) == 3 );
	// size deberia ser igual a 2 en assert anterior?
	// TODO: ver que pasa a nivel de PES si dos streams tienen mismo pid.
	const tuner::ElementaryInfo &elements0 = _pmt->elements( )[0];
	ASSERT_TRUE( elements0.pid == 2064 );
	const tuner::ElementaryInfo &elements1 = _pmt->elements( )[1];
	ASSERT_TRUE( elements1.pid == 2068 );
}

TEST_F( PmtTest, pmt_change_version ) {
	//	Version=1, apply=1
	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 1 );
	ASSERT_EQ( _pmt->elements().size(), 2 );

	util::BYTE tagsToCheck[] = {};	//	TODO: Add section with descriptors!
	ASSERT_TRUE( CHECK_TAGS(_pmt->descriptors(), tagsToCheck) );

	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );

	//	Version=2, apply=1
	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 1 );
	ASSERT_TRUE( _pmt->version( ) == 2 );
	ASSERT_EQ( _pmt->elements().size(), 2 );
	ASSERT_TRUE( CHECK_TAGS(_pmt->descriptors(), tagsToCheck) );

	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni1.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( PmtTest, pmt_change_version_cni ) {
	//	Version=1, apply=1
	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 1 );

	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );

	//	Version=2, apply=0
	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni0.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 1 );

	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni0.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );

	//	Version=2, apply=1
	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni1.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->version( ) == 2 );

	probeSection( "pmt_pcr2064_ver2_sec0_lsn0_cni1.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( PmtTest, pmt_table_id_invalid ) {
	// table_id=0x00, la pmt debera ser descartada
	probeSection( "pmt_ver1_tableid0.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( PmtTest, pmt_pcr_1fff ) {
	probeSection( "pmt_ver1_pcr0x1fff.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->pcrPID( ) == 0x1fff );
}

TEST_F( PmtTest, pmt_max_program_number ) {
	probeSection( "pmt_ver1_pn0xffff.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->programID( ) == 0xffff );
}

TEST_F( PmtTest, pmt_pn_pcr_pid_1fff ) {
	probeSection( "pmt_ver2_pn_pcr_elpid_0x1fff.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->pcrPID( ) == 0x1fff );
	ASSERT_TRUE( _pmt->programID( ) == 0x510 );

	const tuner::ElementaryInfo &info = _pmt->elements( )[0];
	ASSERT_TRUE( info.pid == 0x1fff );
	ASSERT_TRUE( info.streamType == 0xff );
}

TEST_F( PmtTest, pmt_badcrc ) {
	probeSection( "pmt_pcr2064_ver1_sec0_lsn0_cni1_badcrc.sec" );
	ASSERT_FALSE( _pmt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( PmtTest, pmt_sd1_tc_cinco_base ) {
	probeSection( "tc_cinco_pmt_sd11.sec" );
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pmt->programID( ) == 0xe760 );
	ASSERT_TRUE( _pmt->pcrPID( ) == 2060 );
	ASSERT_TRUE( _pmt->version( ) == 1 );

	const tuner::desc::Descriptors &desc = _pmt->descriptors( );
	ASSERT_EQ( true, desc.empty() );
}

void testFirstApplicationSignalingDescriptor( const tuner::desc::Descriptors &desc, const int appType, const int aitVer ) {
	tuner::desc::ApplicationSignallingDescriptor app;
	ASSERT_TRUE( DESC_PARSE( desc, application_signalling, app ) );
	ASSERT_TRUE( app.size() == 1 );

	const tuner::desc::ApplicationSignallingElementInfo &elem = app[0];
	ASSERT_TRUE( elem.type == appType );
	ASSERT_TRUE( elem.version == aitVer );
}

void testDataComponentDescriptor( const tuner::desc::Descriptors &desc, util::WORD componentId ) {
	tuner::desc::DataComponentDescriptor datacomp;
	ASSERT_TRUE( DESC_PARSE( desc, data_component, datacomp ) );
	ASSERT_TRUE( datacomp.codingMethodID == componentId );
}

void testAssociationTagDescriptor( const tuner::desc::Descriptors &desc, const util::WORD tag, const util::WORD use ) {
	tuner::desc::AssociationTagDescriptor assDesc;
	ASSERT_TRUE( DESC_PARSE( desc, association_tag, assDesc ) );
	ASSERT_TRUE( assDesc.tag == tag );
	ASSERT_TRUE( assDesc.use == use );
}

void testStreamIdentifierDescriptor( const tuner::desc::Descriptors &desc, const util::BYTE tag ) {
	util::BYTE dtag;
	ASSERT_TRUE( DESC_PARSE( desc, stream_identifier, dtag ) );
	ASSERT_TRUE( dtag == tag  );
}

void testCarouselIdentifierDescriptor( const tuner::desc::Descriptors &desc, util::DWORD id, util::BYTE format ) {
	tuner::desc::CarouselIdentifierDescriptor carousel;
	ASSERT_TRUE( DESC_PARSE( desc, carousel_id, carousel ) );
	ASSERT_TRUE( carousel.carouselID == id );
	ASSERT_TRUE( carousel.formatID == format );
}

TEST_F( PmtTest, pmt_sd1_tc_cinco_descriptors ) {
	probeSection( "tc_cinco_pmt_sd11.sec");
	ASSERT_TRUE( _pmt != NULL );
	ASSERT_TRUE( _pmt->version() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	const tuner::desc::Descriptors &desc = _pmt->descriptors( );
	ASSERT_TRUE( desc.empty() );

	const std::vector<tuner::ElementaryInfo> &info = _pmt->elements();
	ASSERT_TRUE( info.size() == 4 );

	//	1st descriptor test
	const tuner::ElementaryInfo &info0 = info[0];
	ASSERT_TRUE( info0.streamType == 0x02 );
	ASSERT_TRUE( info0.pid == 2060 );
	const tuner::desc::Descriptors &desc0 = info0.descriptors;
	ASSERT_TRUE( desc0.empty() );

	//	2nd descriptor test
	const tuner::ElementaryInfo & info1 = info[1];
	ASSERT_TRUE( info1.streamType == 0x03 );
	ASSERT_TRUE( info1.pid == 2080 );
	const tuner::desc::Descriptors &desc1 = info1.descriptors;
	ASSERT_TRUE( desc1.empty() );

	//	3rd descriptor test
	const tuner::ElementaryInfo &info2 = info[2];
	ASSERT_TRUE( info2.streamType == 0x05 );
	ASSERT_TRUE( info2.pid == 2001 );

	const tuner::desc::Descriptors &desc2 = info2.descriptors;
	ASSERT_TRUE( !desc2.empty() );

	testFirstApplicationSignalingDescriptor( desc2, 9, 1 );
	ASSERT_FALSE( HasFatalFailure() );
	testDataComponentDescriptor( desc2, 0xA3 );
	ASSERT_FALSE( HasFatalFailure() );
	// TODO: falta pasar app_type, ait_ver

	//	4th descriptor test
	const tuner::ElementaryInfo &info3 = info[3];
	ASSERT_TRUE( info3.streamType == 0x0B );
	ASSERT_TRUE( info3.pid == 2010);

	const tuner::desc::Descriptors &desc3 = info3.descriptors;
	ASSERT_TRUE( !desc3.empty() );
	testDataComponentDescriptor( desc3, 0x00A0 );
	ASSERT_FALSE( HasFatalFailure() );
	testAssociationTagDescriptor( desc3, 0x000C, 0 );
	ASSERT_FALSE( HasFatalFailure() );
	testCarouselIdentifierDescriptor( desc3, 0x02, 0 );
	ASSERT_FALSE( HasFatalFailure() );
	testStreamIdentifierDescriptor( desc3, 0x0C );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( PmtTest, pmt_syntax_indicator_0 ) {
	probeSection( "pmt_synt.sec" );
	ASSERT_FALSE( _pmt );
}

TEST_F( PmtTest, pmt_invalid_tableid ) {
	probeSection( "pmt_invalid_tid.sec");
	ASSERT_FALSE( _pmt );
}
