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
#include "tot.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/tot.h"
#include "../../../src/demuxer/psi/totdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include <boost/bind.hpp>
#include <util/buffer.h>
#include <time.h>


TotTest::TotTest()
{
}

TotTest::~TotTest()
{
}

tuner::ID TotTest::pid() const {
	return TS_PID_TOT;
}

tuner::PSIDemuxer *TotTest::createDemux() {
	tuner::TOTDemuxer *demux = new tuner::TOTDemuxer( TS_PID_TOT, "arg" );
	demux->onParsed( boost::bind( &TotTest::onTot, this, _1 ) );
	demux->onExpired( boost::bind( &SectionDemuxerTest::onExpired, this, _1 ) );
	return demux;
}

void TotTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_tot.reset();
}

void TotTest::onTot( const boost::shared_ptr<tuner::Tot> &tot ) {
	//	tot->show();
	ASSERT_FALSE( _tot );
	_tot = tot;
}

TEST_F( TotTest, create_demux ) {
}

TEST_F( TotTest, tot_0desc) {
	probeSection( "tot_ver0.sec" );
	ASSERT_TRUE( _tot != NULL );
	ASSERT_TRUE( _tot->descriptors().empty() );
}

TEST_F( TotTest, tot_1localTimeoffset) {
	probeSection( "_tot_1desc_ver0.sec" );
	ASSERT_TRUE( _tot != NULL );
	
	tuner::desc::Descriptors &descs = _tot->descriptors();
	tuner::desc::LocalTimeOffsetDescriptor localTimeoffsetDesc;
	ASSERT_TRUE( DESC_PARSE( descs, local_time_offset, localTimeoffsetDesc ) );
	ASSERT_TRUE (localTimeoffsetDesc.size()  == 1);
}

TEST_F( TotTest, tot_2localTimeoffset_ver0) {
	probeSection( "_tot_1desc_2offset_ver0.sec" );
	ASSERT_TRUE( _tot != NULL );
	
	tuner::desc::Descriptors &descs = _tot->descriptors();
	tuner::desc::LocalTimeOffsetDescriptor localTimeoffsetDesc;
	ASSERT_TRUE( DESC_PARSE( descs, local_time_offset, localTimeoffsetDesc ) );
	ASSERT_TRUE (localTimeoffsetDesc.size()  == 2);
}

TEST_F( TotTest, tot_2localTimeoffset_ver1) {
	probeSection( "_tot_1desc_2offset_ver1.sec" );
	ASSERT_TRUE( _tot != NULL );
	
	tuner::desc::Descriptors &descs = _tot->descriptors();
	tuner::desc::LocalTimeOffsetDescriptor localTimeoffsetDesc;
	ASSERT_TRUE( DESC_PARSE( descs, local_time_offset, localTimeoffsetDesc ) );
	ASSERT_TRUE (localTimeoffsetDesc.size()  == 2);
}

TEST_F( TotTest, tot_ver0 ) {
	//	Probe basic pat
	probeSection( "tot_ver0.sec" );
	ASSERT_TRUE( _tot != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	tuner::MJDate date = _tot->date();
	ASSERT_TRUE( date.year() == 2010 );
	ASSERT_TRUE( date.month() == 9 );
	ASSERT_TRUE( date.day() == 20 );
	ASSERT_TRUE( date.hours() == 15 );
	ASSERT_TRUE( date.minutes() == 44 );
	ASSERT_TRUE( date.seconds() == 21 );
	ASSERT_TRUE( _tot->descriptors().empty() );
}

TEST_F( TotTest, tot_descs ) {
	//	Probe basic tot
	probeSection( "tot_w_1desc_1loop_ver1.sec" );
	ASSERT_TRUE( _tot != NULL );
	ASSERT_TRUE( wasExpired() == 0 );

	{	//	Check date
		tuner::MJDate date = _tot->date();
		ASSERT_TRUE( date.year() == 2010 );
		ASSERT_TRUE( date.month() == 9 );
		ASSERT_TRUE( date.day() == 20 );
		ASSERT_TRUE( date.hours() == 15 );
		ASSERT_TRUE( date.minutes() == 44 );
		ASSERT_TRUE( date.seconds() == 21 );
	}

	{	//	Check descriptors
		tuner::desc::Descriptors &descs = _tot->descriptors();
		tuner::desc::LocalTimeOffsetDescriptor desc;
		ASSERT_TRUE( DESC_PARSE( descs, local_time_offset, desc ) );
		ASSERT_TRUE( desc.size() == 1 );
		const tuner::desc::LocalTimeOffset &timeOff = desc[0];
		ASSERT_TRUE( timeOff.countryCode == "arg" );
		ASSERT_TRUE( timeOff.countryRegion == 0 );
		ASSERT_TRUE( timeOff.localTimeOffsetPolarity == 1);
		ASSERT_TRUE( timeOff.localTimeOffset == 0x0300 );
		
		tuner::MJDate date( timeOff.changingDate, timeOff.timeOfChange );
		ASSERT_TRUE( date.year() == 2011 );
		ASSERT_TRUE( date.month() == 1 );
		ASSERT_TRUE( date.day() == 1 );
		ASSERT_TRUE( date.hours() == 0 );
		ASSERT_TRUE( date.minutes() == 0 );
		ASSERT_TRUE( date.seconds() == 0 );
		
		ASSERT_TRUE( timeOff.nextTimeOffset == 0 );
	}
}

TEST_F( TotTest, tot_syntax ) {
	//	Probe basic tot
	probeSection( "tot_ver0_syn1.sec" );
	ASSERT_FALSE( _tot );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( TotTest, tot_crc ) {
	//	Probe basic tot
	probeSection( "tot_ver0_badcrc.sec" );
	ASSERT_FALSE( _tot );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( TotTest, tot_invalid_tableid ) {
	probeSection( "tot_invalid_tid.sec" );
	ASSERT_FALSE( _tot );
}
