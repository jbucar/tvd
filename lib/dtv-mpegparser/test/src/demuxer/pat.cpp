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
#include "pat.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/pat.h"
#include "../../../src/demuxer/psi/patdemuxer.h"
#include <util/buffer.h>
#include <boost/bind.hpp>

PatTest::PatTest( void )
{
}

PatTest::~PatTest( void )
{
}

tuner::PSIDemuxer *PatTest::createDemux() {
	return tuner::createDemuxer<tuner::PATDemuxer,tuner::Pat>(
		TS_PID_PAT,
		boost::bind( &PatTest::onPat, this, _1 ),
		boost::bind( &SectionDemuxerTest::onExpired, this, _1 ) );
}

void PatTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_pat.reset();
}

void PatTest::onPat( const boost::shared_ptr<tuner::Pat> &pat ) {
	ASSERT_FALSE( _pat );
	_pat = pat;
}

TEST_F( PatTest, create_demux ) {
}

TEST_F( PatTest, pat_nit_ver0_cni1 ) {
	//	Probe basic pat
	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 1 );
	tuner::Pat::ProgramInfo &info = _pat->programs()[0];
	ASSERT_TRUE( info.program == 0xe740 );
	ASSERT_TRUE( info.pid == 0x407 );
}

TEST_F( PatTest, pat_nit_ver1_cni1 ) {
	//	Probe basic pat
	probeSection( "pat_nit_ver1_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 1 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 1 );
	tuner::Pat::ProgramInfo &info = _pat->programs()[0];
	ASSERT_TRUE( info.program == 0xe740 );
	ASSERT_TRUE( info.pid == 0x407 );
}

TEST_F( PatTest, pat_nit_sprog ) {
	//	Probe basic pat
	probeSection( "pat_nit_sprog.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 1 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );
	ASSERT_TRUE( _pat->programs().size() == 0 );
}

TEST_F( PatTest, pat_snit ) {
	//	Probe basic pat
	probeSection( "pat_snit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73b );
	ASSERT_TRUE( _pat->nitPid() == TS_PID_NULL );

	ASSERT_TRUE( _pat->programs().size() == 1 );
	tuner::Pat::ProgramInfo &info = _pat->programs()[0];
	ASSERT_TRUE( info.program == 0xe740 );
	ASSERT_TRUE( info.pid == 0x407 );
}

TEST_F( PatTest, pat_2nit_ver0_cni1 ) {
	//	Probe basic pat
	probeSection( "pat_2nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 1 );
	tuner::Pat::ProgramInfo &info = _pat->programs()[0];
	ASSERT_TRUE( info.program == 0xe740 );
	ASSERT_TRUE( info.pid == 0x407 );
}

TEST_F( PatTest, pat_snit_sprog ) {
	//	Probe basic pat
	probeSection( "pat_snit_sprog.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 1 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == TS_PID_NULL );
	ASSERT_TRUE( _pat->programs().size() == 0 );
}

TEST_F( PatTest, pat_err_tableid8 ) {
	//	Probe basic pat
	probeSection( "pat_err_tableid8.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( PatTest, pat_err_syntax ) {
	//	Probe basic pat
	probeSection( "pat_err_syntax.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( PatTest, pat_err_crc ) {
	//	Probe basic pat
	probeSection( "pat_err_crc.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( PatTest, pat_repeated_programs ) {
	//	Probe basic pat
	probeSection( "pat_repeated_programs.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 1 );
	tuner::Pat::ProgramInfo &info = _pat->programs()[0];
	ASSERT_TRUE( info.program == 0xe740 );
	ASSERT_TRUE( info.pid == 0x407 );
}

TEST_F( PatTest, pat_repeated_programs2 ) {
	//	Probe basic pat
	probeSection( "pat_repeated_programs2.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 2 );
	tuner::Pat::ProgramInfo &info1 = _pat->programs()[0];
	ASSERT_TRUE( info1.program == 0xe740 );
	ASSERT_TRUE( info1.pid == 0x407 );

	tuner::Pat::ProgramInfo &info2 = _pat->programs()[1];
	ASSERT_TRUE( info2.program == 0xe741 );
	ASSERT_TRUE( info2.pid == 0x408 );
}

TEST_F( PatTest, basic_repeat ) {
	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( PatTest, change_version ) {
	//	Version=0, apply=1
	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_EQ( _pat->programs().size(), 1 );

	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Version=1, apply=1
	probeSection( "pat_nit_ver1_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _pat->version() == 1 );
	ASSERT_EQ( _pat->programs().size(), 1 );

	probeSection( "pat_nit_ver1_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( PatTest, change_version_cni ) {
	probeSection( "pat_nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 0 );

	probeSection( "pat_nit_ver1_cni0.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 1 );

	probeSection( "pat_nit_ver1_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 1 );
}

TEST_F( PatTest, pat_multiple_seccions ) {
	probeSection( "pat_sec0_nit_ver0_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "pat_sec1_nit_ver0_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "pat_sec2_nit_ver0_cni1.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "pat_sec3_nit_ver0_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 5 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->nitPid() == 0x10 );

	ASSERT_TRUE( _pat->programs().size() == 3 );
	tuner::Pat::ProgramInfo &info1 = _pat->programs()[0];
	ASSERT_TRUE( info1.program == 0xe740 );
	ASSERT_TRUE( info1.pid == 0x407 );

	tuner::Pat::ProgramInfo &info2 = _pat->programs()[1];
	ASSERT_TRUE( info2.program == 0xe741 );
	ASSERT_TRUE( info2.pid == 0x407 );

	tuner::Pat::ProgramInfo &info3 = _pat->programs()[2];
	ASSERT_TRUE( info3.program == 0xe742 );
	ASSERT_TRUE( info3.pid == 0x407 );
}

TEST_F( PatTest, pat_ver0_cni1_size1021 ) {
	//	Probe basic pat
	probeSection( "pat_ver0_cni1_size1021.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	//	Check PAT values
	ASSERT_TRUE( _pat->version() == 0 );
	ASSERT_TRUE( _pat->tsID() == 0x73a );
	ASSERT_TRUE( _pat->programs().size() == 253 );
}


TEST_F( PatTest, pat_tc_cinco ) {
	probeSection( "tc_cinco_pat1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired( ) == 0 );
	ASSERT_TRUE( _pat->version( ) == 1 );
	ASSERT_TRUE( _pat->tsID( ) == 0x073b );
	ASSERT_TRUE( _pat->nitPid( ) == 16 );
	ASSERT_TRUE( _pat->programs( ).size( ) == 5 );

	tuner::Pat::ProgramInfo &program_sd1 = _pat->programs( )[0];
	ASSERT_TRUE( program_sd1.program == 0xe760 );
	ASSERT_TRUE( program_sd1.pid == 1031 );

	tuner::Pat::ProgramInfo &program_sd2 = _pat->programs( )[1];
	ASSERT_TRUE( program_sd2.program == 0xe761 );
	ASSERT_TRUE( program_sd2.pid == 1032 );

	tuner::Pat::ProgramInfo &program_sd3 = _pat->programs( )[2];
	ASSERT_TRUE( program_sd3.program == 0xe762 );
	ASSERT_TRUE( program_sd3.pid == 1033 );

	tuner::Pat::ProgramInfo &program_sd4 = _pat->programs( )[3];
	ASSERT_TRUE( program_sd4.program == 0xe763 );
	ASSERT_TRUE( program_sd4.pid == 1034 );

	tuner::Pat::ProgramInfo &program_sd5 = _pat->programs( )[4];
	ASSERT_TRUE( program_sd5.program == 0xe764 );
	ASSERT_TRUE( program_sd5.pid == 1035 );
}

TEST_F( PatTest, pat_errpid ) {
	probeSection( "pat_errpid.sec" );
	// pat_errpid.sec tiene varios programas pero con pids invalidos
	// { 0, 10, 1fff, 11, 12, 26, 27, 13, 14, 22, 24, 25 }
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( _pat->programs( ).size( ) == 2 );

	{
		tuner::Pat::ProgramInfo &info = _pat->programs( )[0];
		ASSERT_TRUE( info.pid == 0xAA );
		ASSERT_TRUE( info.program == 0x10 );
	}

	{
		tuner::Pat::ProgramInfo &info = _pat->programs( )[1];
		ASSERT_TRUE( info.pid == 0x10 );
		ASSERT_TRUE( info.program == 0x1fff );
	}
}

TEST_F( PatTest, pat_syntax_indicator_0 ) {
	probeSection( "pat_synt.sec" );
	ASSERT_FALSE( _pat );
}

TEST_F( PatTest, pat_invalid_tableid ) {
	probeSection( "pat_invalid_tid.sec" );
	ASSERT_FALSE( _pat );
}

TEST_F( PatTest, pat_version_change_3_sec ) {
	probeSection( "pat_ver5_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 5 );

	probeSection( "pat_ver6_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _pat->version() == 6 );

	probeSection( "pat_ver7_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _pat->version() == 7 );
}

TEST_F( PatTest, pat_version_change_3_sec_cni0 ) {
	probeSection( "pat_ver5_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 5 );

	probeSection( "pat_ver6_cni0.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 1 );

	probeSection( "pat_ver6_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 6 );

	probeSection( "pat_ver7_cni0.sec" );
	ASSERT_FALSE( _pat );
	ASSERT_TRUE( wasExpired() == 1 );

	probeSection( "pat_ver7_cni1.sec" );
	ASSERT_TRUE( _pat.get() != NULL );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _pat->version() == 7 );
}
