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

#include "sdtt.h"
#include "../../../../src/demuxer/psi/psi.h"
#include "../../../../src/demuxer/psi/application/types.h"
#include "../../../../src/demuxer/psi/download/sdtt.h"
#include "../../../../src/demuxer/psi/download/sdttdemuxer.h"
#include "../../../../src/demuxer/psi/dsmcc/dsmcc.h"
#include <boost/bind.hpp>
#include <util/buffer.h>
#include <boost/foreach.hpp>

SdttTest::SdttTest() {
}

SdttTest::~SdttTest() {
}

void SdttTest::onSdtt( const boost::shared_ptr<tuner::Sdtt> &sdtt ) {
	_sdtt = sdtt;
}

tuner::PSIDemuxer *SdttTest::createDemux( ) {
	return tuner::createDemuxer<tuner::SDTTDemuxer, tuner::Sdtt > (
			pid( ),
			boost::bind( &SdttTest::onSdtt, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void SdttTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe( );
	_sdtt.reset();
}

tuner::ID SdttTest::pid() const {
	return 0x0023;
	//   shall be either 0x0023 or 0x0028 for terrestrial digital tv broadcasting
	//   ARIB STD B10  -  seccion 5.1 PID for tables
}

TEST_F( SdttTest, create_demux ) {
}

TEST_F( SdttTest, sdtt_basic ) {
	probeSection( "sdtt_base.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->tsID( ) == 0x073b );
	ASSERT_TRUE( _sdtt->version( ) == 1 );
	ASSERT_TRUE( _sdtt->modelID( ) == 0x1301 ); //  maker + model
	ASSERT_TRUE( _sdtt->nitID( ) == 0x073b );
	ASSERT_TRUE( _sdtt->tsID( ) == 0x073b );
	ASSERT_TRUE( _sdtt->serviceID( ) == 0xe764 );
}

void testSdttCompatibilityDesc( const tuner::dsmcc::compatibility::Descriptor &desc,
		const util::WORD model, const util::WORD version, const util::DWORD specifier,
		const util::BYTE type ) {
	ASSERT_TRUE( desc.specifier == specifier ); //  0xff819282
	ASSERT_TRUE( desc.type == type ); //  0x02
	ASSERT_TRUE( desc.model == model );
	ASSERT_TRUE( desc.version == version );

}

void testSdttDownloadContentDesc( const tuner::Sdtt::DownloadContentDescriptor desc, const bool reboot,
		const bool addOn, const util::DWORD componentSize, const util::DWORD downloadID,
		const util::DWORD timeoutDII, const util::DWORD leakRate, const util::BYTE componentTag,
		const bool hasCompatibility, const bool hasModuleInfo ) {
	ASSERT_TRUE( desc.reboot == reboot );
	ASSERT_TRUE( desc.addOn == addOn );
	ASSERT_TRUE( desc.componentSize == componentSize );
	ASSERT_TRUE( desc.downloadID == downloadID );
	ASSERT_TRUE( desc.timeoutDII == timeoutDII );
	ASSERT_TRUE( desc.leakRate == leakRate );
	ASSERT_TRUE( desc.componentTag == componentTag );
	ASSERT_TRUE( desc.hasCompatibility == hasCompatibility );
	ASSERT_TRUE( desc.hasModuleInfo == hasModuleInfo );
}

void testSdttElementaryInfo( const tuner::Sdtt::ElementaryInfo &elem,
		const util::BYTE group, const util::WORD targetVersion, const util::WORD newVersion,
		const util::BYTE downloadLevel, const util::BYTE versionIndicator, const util::BYTE scheduleTimeShiftInfo ) {
	ASSERT_TRUE( elem.group == group );
	ASSERT_TRUE( elem.targetVersion == targetVersion );
	ASSERT_TRUE( elem.newVersion == newVersion );
	ASSERT_TRUE( elem.downloadLevel == downloadLevel );
	ASSERT_TRUE( elem.versionIndicator == versionIndicator );
	ASSERT_TRUE( elem.scheduleTimeShiftInfo == scheduleTimeShiftInfo );
}

TEST_F( SdttTest, sdtt_descriptors ) {
	probeSection( "sdtt_base.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->elements( ).size( ) == 1 );

	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];

	testSdttElementaryInfo( elem, 0, 0x00e, 0x00f, 1, 2, 0 );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( elem.schedules.size( ) == 0 );

	ASSERT_TRUE( elem.contents.size( ) == 1 );
	const tuner::Sdtt::DownloadContentDescriptor &cont = elem.contents[0];
	testSdttDownloadContentDesc( cont, 0, 1, 0, 1, 36600000, 0, 0x10, true, false );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( cont.compatibilities.size( ) == 1 );
	testSdttCompatibilityDesc( cont.compatibilities[0], 0x1301, 0x000e, 0xff819282, 0x02 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdttTest, sdtt_download_desc_limits ) {
	probeSection( "sdtt_download_content_desc_limits.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->elements( ).size( ) == 1 );
	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];

	ASSERT_TRUE( elem.contents.size( ) == 1 );
	testSdttDownloadContentDesc( elem.contents[0], 1, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x3FFFFF, 0xFF, true, false );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdttTest, sdtt_compatibility_desc_limits ) {
	probeSection( "sdtt_compatibility_desc_limits.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->elements( ).size( ) == 1 );
	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];

	ASSERT_TRUE( elem.contents.size( ) == 1 );
	const tuner::Sdtt::DownloadContentDescriptor &cont = elem.contents[0];
	ASSERT_TRUE( cont.compatibilities.size( ) == 1 );
	testSdttCompatibilityDesc( cont.compatibilities[0], 0xffff, 0xffff, 0xffffffff, 0xff );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdttTest, sdtt_elementaryinfo_limits ) {
	probeSection( "sdtt_elementaryinfo_limits.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->elements( ).size( ) == 1 );
	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];
	testSdttElementaryInfo( elem, 0xf, 0xfff, 0xfff, 3, 3, 0xf );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdttTest, sdtt_schedule_descriptor ) {
	probeSection( "sdtt_schedule_descriptor.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->elements().size( ) == 1 );
	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];
	_sdtt->show( );
	ASSERT_TRUE( elem.scheduleTimeShiftInfo == 0x01 );
	ASSERT_TRUE( elem.schedules.size( ) == 1 );
	ASSERT_TRUE( elem.schedules[0].startTime == 0xDA07231122ULL );
	//	h=23 m=11 s=22, fecha en formato MJD
}

TEST_F( SdttTest, sdtt_change_version ) {
	probeSection( "sdtt_ver0_cni1.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdtt->version( ) == 0 );

	probeSection( "sdtt_ver0_cni1.sec" );
	ASSERT_FALSE( _sdtt );
	ASSERT_TRUE( wasExpired( ) == 0 );

	probeSection( "sdtt_ver1_cni1.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( wasExpired( ) == 1 );
	ASSERT_TRUE( _sdtt->version( ) == 1 );

	probeSection( "sdtt_ver1_cni1.sec" );
	ASSERT_FALSE( _sdtt );
	ASSERT_TRUE( wasExpired( ) == 0 );
}

TEST_F( SdttTest, sdtt_change_version_cni ) {
	probeSection( "sdtt_ver0_cni1.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdtt->version( ) == 0 );

	probeSection( "sdtt_ver1_cni0.sec" );
	ASSERT_FALSE( _sdtt );
	ASSERT_TRUE( wasExpired( ) == 1 );

	probeSection( "sdtt_ver1_cni1.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _sdtt->version( ) == 1 );
}

TEST_F( SdttTest, sdtt_multiple_download_content_descriptors ) {
	probeSection( "sdtt_mult_desc.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	ASSERT_TRUE( _sdtt->version( ) == 1 );
}

TEST_F( SdttTest, sdtt_2_loop_2_download_content_descriptors ) {
	probeSection( "sdtt_2_loop_2_desc.sec" );
	ASSERT_TRUE( _sdtt != NULL );
	_sdtt->show( );
	ASSERT_TRUE( _sdtt->version( ) == 2 );
	ASSERT_TRUE( _sdtt->elements( ).size( ) == 2 );

	// elem 0
	const tuner::Sdtt::ElementaryInfo &elem = _sdtt->elements( )[0];
	testSdttElementaryInfo( elem, 0x05, 0x20, 0x21, 0, 1, 1 );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( elem.schedules.size( ) == 2 );

	const tuner::Sdtt::ScheduleElementaryInfo &schedule = elem.schedules[0];
	ASSERT_TRUE( schedule.startTime == 0xDA07231122ULL );
	ASSERT_TRUE( schedule.duration == 0x100908 );

	const tuner::Sdtt::ScheduleElementaryInfo &schedule1 = elem.schedules[1];
	ASSERT_TRUE( schedule1.startTime == 0xd900183015ULL );
	ASSERT_TRUE( schedule1.duration == 0x050025 );

	ASSERT_TRUE( elem.contents.size( ) == 2 );
	const tuner::Sdtt::DownloadContentDescriptor &content = elem.contents[0];
	testSdttDownloadContentDesc( content, 0, 1, 0, 1, 36600000, 0, 0x10, true, false );
	ASSERT_FALSE( HasFatalFailure() );

	ASSERT_TRUE( content.compatibilities.size( ) == 1 );

	const tuner::dsmcc::compatibility::Descriptor &compatibility0 = content.compatibilities[0];
	testSdttCompatibilityDesc( compatibility0, 0x10F3, 0x02AB, 0xFF819282, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	const tuner::Sdtt::DownloadContentDescriptor &content1 = elem.contents[1];
	ASSERT_TRUE( content1.compatibilities.size( ) == 2 );

	const tuner::dsmcc::compatibility::Descriptor &compatibility10 = content1.compatibilities[0];
	testSdttCompatibilityDesc( compatibility10, 0x2345, 0x6789, 0xFF819282, 0x02 );
	ASSERT_FALSE( HasFatalFailure() );

	const tuner::dsmcc::compatibility::Descriptor &compatibility11 = content1.compatibilities[1];
	testSdttCompatibilityDesc( compatibility11, 0xABCD, 0xEF01, 0xFF819282, 0x02 );
	ASSERT_FALSE( HasFatalFailure() );

	const tuner::Sdtt::DownloadContentDescriptor &content01 = elem.contents[1];
	testSdttDownloadContentDesc( content01, 1, 0, 0x10, 2, 36600000, 0, 0x10, true, false );
	ASSERT_FALSE( HasFatalFailure() );

	// elem 1
	const tuner::Sdtt::ElementaryInfo &elem1 = _sdtt->elements( )[1];
	testSdttElementaryInfo( elem1, 0x01, 0xa2, 0xa3, 1, 3, 1 );
	ASSERT_FALSE( HasFatalFailure() );
	ASSERT_TRUE( elem1.schedules.size( ) == 0 );
	ASSERT_TRUE( elem1.contents.size( ) == 2 );

	const tuner::Sdtt::DownloadContentDescriptor &content20 = elem1.contents[0];
	testSdttDownloadContentDesc( content20, 0, 1, 0, 0xB1, 36600000, 0, 0x10, true, false );
	ASSERT_FALSE( HasFatalFailure() );

	ASSERT_TRUE( content20.compatibilities.size( ) == 1 );
	const tuner::dsmcc::compatibility::Descriptor &compatibility20 = content20.compatibilities[0];
	testSdttCompatibilityDesc( compatibility20, 0x10F3, 0x02AB, 0xFF819282, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );

	const tuner::Sdtt::DownloadContentDescriptor &content21 = elem1.contents[1];
	testSdttDownloadContentDesc( content21, 1, 0, 0x10, 0xB2, 36600000, 0, 0x10, true, false );
	ASSERT_FALSE( HasFatalFailure() );

	ASSERT_TRUE( content21.compatibilities.size( ) == 1 );
	const tuner::dsmcc::compatibility::Descriptor &compatibility21 = content21.compatibilities[0];
	testSdttCompatibilityDesc( compatibility21, 0x10F3, 0x02AB, 0xFF819282, 0x01 );
	ASSERT_FALSE( HasFatalFailure() );
}

TEST_F( SdttTest, sdtt_syntax_indicator_0 ) {
	probeSection( "sdtt_ver0_cni1_synt.sec" );
	ASSERT_FALSE( _sdtt );
}


TEST_F( SdttTest, sdtt_invalid_tableid ) {
	probeSection( "sdtt_invalid_tid.sec" );
	ASSERT_FALSE( _sdtt );
}
