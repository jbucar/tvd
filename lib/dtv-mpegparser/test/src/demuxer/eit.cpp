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
#include "eit.h"
#include "../../../src/time.h"
#include "../../../src/demuxer/psi/psi.h"
#include "../../../src/demuxer/psi/eitdemuxer.h"
#include "../../../src/demuxer/descriptors/demuxers.h"
#include <util/buffer.h>
#include <util/string.h>
#include <util/mcr.h>
#include <boost/bind.hpp>

EitTest::EitTest() {
}

EitTest::~EitTest() {
}

void EitTest::onEit( const boost::shared_ptr<tuner::Eit> &eit ) {
	_eits.push_back( eit );
}

tuner::PSIDemuxer *EitTest::createDemux() {
	return tuner::createDemuxer<tuner::EITDemuxer, tuner::Eit > (
			pid(),
			boost::bind( &EitTest::onEit, this, _1 ),
			boost::bind( &SectionDemuxerTest::onExpired, this, _1 ));
}

void EitTest::cleanProbe() {
	SectionDemuxerTest::cleanProbe();
	_eits.clear();
}

tuner::ID EitTest::pid() const {
	return TS_PID_EIT;
}

void EitTest::updateTime( int firstValidSegment ) {
	//	2011-01-01 00:00:00
	tuner::pt::ptime in2011(
		boost::gregorian::date(2011,1,1),
		tuner::pt::time_duration( 0, 0, 0 ) );
	in2011 += tuner::pt::seconds(firstValidSegment*3*3600);
	tuner::time::clock.update( in2011 );
}

TEST_F( EitTest, create_demux ) {
}

TEST_F( EitTest, bad_crc ) {
	probeSection( "eit_badcrc.sec" );
	ASSERT_TRUE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, empty ) {
	probeSection( "eit_empty_act-p-f_sn1_ver3.sec" );
	ASSERT_TRUE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, eit_ver0_incomplete ) {
	probeSection( "eit_ver0.sec" );
	ASSERT_TRUE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, eit_basic_test_ver1 ) {
	probeSection( "eit_sn0_sn1_ver0.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	eit->show();

	ASSERT_TRUE( eit->version() == 1 );
	ASSERT_TRUE( eit->tableID() == 0x4E );
	ASSERT_TRUE( eit->serviceID() == 0xe750 );
	ASSERT_TRUE( eit->tsID() == 0x073a );
	ASSERT_TRUE( eit->nitID() == 0x073a );

	const tuner::eit::Events &events=eit->events();
	ASSERT_TRUE( events.size() == 2 );

	{
		//	Event1
		const tuner::eit::Event *ev = events[0];
		ASSERT_TRUE( ev->eventID == 1 );
		ASSERT_TRUE( ev->status == 4 );
		ASSERT_TRUE( ev->caMode == 0 );

		tuner::MJDate start = ev->startTime;
		ASSERT_TRUE( start.year() == 2011 );
		ASSERT_TRUE( start.month() == 2 );
		ASSERT_TRUE( start.day() == 28 );
		ASSERT_TRUE( start.hours() == 10 );
		ASSERT_TRUE( start.minutes() == 20 );
		ASSERT_TRUE( start.seconds() == 30 );

		tuner::MJDate duration = ev->duration;
		ASSERT_TRUE( duration.year() == 0 );
		ASSERT_TRUE( duration.month() == 0 );
		ASSERT_TRUE( duration.day() == 0 );
		ASSERT_TRUE( duration.hours() == 1 );
		ASSERT_TRUE( duration.minutes() == 30 );
		ASSERT_TRUE( duration.seconds() == 45 );
	}

	{
		//	Event2
		const tuner::eit::Event *ev = events[1];
		ASSERT_TRUE( ev->eventID == 1 );
		ASSERT_TRUE( ev->status == 1 );
		ASSERT_TRUE( ev->caMode == 0 );

		tuner::MJDate start = ev->startTime;
		ASSERT_TRUE( start.year() == 2011 );
		ASSERT_TRUE( start.month() == 2 );
		ASSERT_TRUE( start.day() == 28 );
		ASSERT_TRUE( start.hours() == 11 );
		ASSERT_TRUE( start.minutes() == 50 );
		ASSERT_TRUE( start.seconds() == 56 );

		tuner::MJDate duration = ev->duration;
		ASSERT_TRUE( duration.year() == 0 );
		ASSERT_TRUE( duration.month() == 0 );
		ASSERT_TRUE( duration.day() == 0 );
		ASSERT_TRUE( duration.hours() == 1 );
		ASSERT_TRUE( duration.minutes() == 30 );
		ASSERT_TRUE( duration.seconds() == 45 );
	}
}

TEST_F( EitTest, epg_complete ) {
	char buf[64];
	for (int i=0; i<51; i++) {
		snprintf( buf, 64, "epg_complete_%02d.sec", i );
		probeSection( buf, false );
	}
	ASSERT_TRUE( _eits.size() == 2 );
	ASSERT_TRUE( wasExpired() == 0 );

	BOOST_FOREACH( const EitPtr eit, _eits ) {
		eit->show();
	}
}

TEST_F( EitTest, eit_running_status_present_following ) {
	probeSection( "eit_ver1_sn0_rs4.sec" );
	probeSection( "eit_ver1_sn1_rs1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0);

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events=eit->events();
	ASSERT_TRUE( events.size() == 2 );

	//	checking running status for present and following event.
	ASSERT_TRUE( events[0]->status == 4 );
	ASSERT_TRUE( events[1]->status == 1 );

	const tuner::desc::Descriptors &desc0 = events[0]->descs;
	{
		//	first event - Short descriptor
		tuner::desc::ShortEventDescriptor se_desc;
		ASSERT_TRUE( DESC_PARSE( desc0, short_event, se_desc ) );
		const tuner::Language lang = se_desc.language;
		ASSERT_TRUE( std::string(lang.code()) == "spa" );
		ASSERT_TRUE( se_desc.event == "Event running" );
		ASSERT_TRUE( se_desc.text ==  "Present event - Actual TS - with running_status=4" );
	}

	{
		//	first event - Parental rating descriptor
		tuner::desc::ParentalRatingDescriptor pr_desc;
		ASSERT_TRUE( DESC_PARSE( desc0, parental_rating, pr_desc ) );
		ASSERT_TRUE( pr_desc.size() == 1 );
		const tuner::Language lang = pr_desc[0].language;
		ASSERT_TRUE( std::string(lang.code()) == "BRA" );
		ASSERT_TRUE( pr_desc[0].rating == 3 );
	}

	const tuner::desc::Descriptors &desc1 = events[1]->descs;
	{
		//	second event - short descriptor
		tuner::desc::ShortEventDescriptor se_desc;
		ASSERT_TRUE( DESC_PARSE( desc1, short_event, se_desc ) );
		const tuner::Language lang= se_desc.language;
		ASSERT_TRUE( std::string(lang.code()) == "spa" );
		ASSERT_TRUE( se_desc.event == "Event not running" );
		ASSERT_TRUE( se_desc.text ==  "Following event - Actual TS - with running_status=1" );
	}

	{
		//	second event - Parental rating descriptor
		tuner::desc::ParentalRatingDescriptor pr_desc;
		ASSERT_TRUE( DESC_PARSE( desc1, parental_rating, pr_desc ) );
		ASSERT_TRUE( pr_desc.size() == 1 );
		const tuner::Language lang = pr_desc[0].language;
		ASSERT_TRUE( std::string(lang.code()) == "BRA" );
		ASSERT_TRUE( pr_desc[0].rating == 3 );
	}
}

TEST_F( EitTest, invalid_table_id ) {
	probeSection( "eit_invalid_tid.sec" );
	ASSERT_TRUE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, invalid_table_sn00 ) {
	probeSection( "eit_invalid_sn00.sec" );
	ASSERT_TRUE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

//	Complete EPG with first missing sections added from
//	broadcaster 'TV Publica - Argentina'
TEST_F( EitTest, epg_tvp_all ) {
	updateTime( 4 );
	probeSection( "epg_tvp_all.sec" );
	ASSERT_FALSE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
	BOOST_FOREACH( const EitPtr &eit, _eits ) {
		eit->show();
	}
}

//	EPG from brazilian broadcaster
TEST_F( EitTest, epg_brazil ) {
	probeSection( "sbt.0x12.sec" );
	ASSERT_FALSE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, DISABLED_eit_p_f_not_chronologic ) {

	// 	This test is currently DISABLED because actual implementation
	//	doesn't check chronological order of events

	//	ETSI EN 300 468 V1.11.1 (2010-04) page 24

	probeSection( "eit_actual_sn0_rs4.sec" );
	//	using ACTUAL = { date: 2011-03-16, start_time: 19:00:00, duration: 00:45:00, }

	probeSection( "eit_foll_past_sn1_rs1.sec" );
	//	using FOLLOWING = { date: 2011-03-15, start_time: 10:00:00, duration: 01:30:00, }
	//	this event is 1 day in the past, it is not in chronological order

	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events = eit->events();
	ASSERT_TRUE( events.size() == 1 );  //	second event should be discarded
}


TEST_F( EitTest, eit_following_empty ) {

	// 	ETSI TR 101 211: page 14 - item 'g)': if no following event exists, an empty section 1
	// 	of the EIT Present/Following shall be transmitted

	probeSection( "eit_ver7_sn0_rs4.sec" );
	probeSection( "eit_ver7_sn1_rs1_empty.sec");

	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events = eit->events();
	ASSERT_TRUE( events.size() == 1 );

	ASSERT_TRUE( eit->version() == 7 );
	ASSERT_TRUE( eit->tableID() == 0x4E );
	ASSERT_TRUE( eit->serviceID() == 0xe700 );
	ASSERT_TRUE( eit->tsID() == 0x04f0 );
	ASSERT_TRUE( eit->nitID() == 0x04f0 );

	//	Single event (present only)
	const tuner::eit::Event *ev = events[0];
	ASSERT_TRUE( ev->eventID == 1 );
	ASSERT_TRUE( ev->status == 4 );
	ASSERT_TRUE( ev->caMode == 0 );

	tuner::MJDate start = ev->startTime;
	ASSERT_TRUE( start.year() == 2011 );
	ASSERT_TRUE( start.month() == 3 );
	ASSERT_TRUE( start.day() == 30 );
	ASSERT_TRUE( start.hours() == 20 );
	ASSERT_TRUE( start.minutes() == 30 );
	ASSERT_TRUE( start.seconds() == 0 );

	tuner::MJDate duration = ev->duration;
	ASSERT_TRUE( duration.year() == 0 );
	ASSERT_TRUE( duration.month() == 0 );
	ASSERT_TRUE( duration.day() == 0 );
	ASSERT_TRUE( duration.hours() == 0 );
	ASSERT_TRUE( duration.minutes() == 45 );
	ASSERT_TRUE( duration.seconds() == 0 );
}


TEST_F( EitTest, DISABLED_eit_2_following_events ) {
	//	DISABLED: Functionality not yet implemented
	//
	// 	ETSI TR 101 211: page 14
	//	e) at each point in time, there shall be at most one following event;

	probeSection( "eit_ver8_sn0_rs4.sec" );
	probeSection( "eit_ver8_sn1_rs1.sec" );

	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events = eit->events();
	ASSERT_TRUE( eit->version() == 8 );
	ASSERT_TRUE( events.size() == 2 );

	{
		//	present event
		const tuner::eit::Event *ev = events[0];
		ASSERT_TRUE( ev->eventID == 1 );
		ASSERT_TRUE( ev->status == 4 );
		ASSERT_TRUE( ev->caMode == 0 );

		tuner::MJDate start = ev->startTime;
		ASSERT_TRUE( start.year() == 2011 );
		ASSERT_TRUE( start.month() == 3 );
		ASSERT_TRUE( start.day() == 30 );
		ASSERT_TRUE( start.hours() == 20 );
		ASSERT_TRUE( start.minutes() == 0 );
		ASSERT_TRUE( start.seconds() == 0 );

		tuner::MJDate duration = ev->duration;
		ASSERT_TRUE( duration.year() == 0 );
		ASSERT_TRUE( duration.month() == 0 );
		ASSERT_TRUE( duration.day() == 0 );
		ASSERT_TRUE( duration.hours() == 1 );
		ASSERT_TRUE( duration.minutes() == 0 );
		ASSERT_TRUE( duration.seconds() == 0 );
	}

	{
		//	following event
		const tuner::eit::Event *ev = events[1];
		ASSERT_TRUE( ev->eventID == 2 );
		ASSERT_TRUE( ev->status == 1 );
		ASSERT_TRUE( ev->caMode == 0 );

		tuner::MJDate start = ev->startTime;
		ASSERT_TRUE( start.year() == 2011 );
		ASSERT_TRUE( start.month() == 3 );
		ASSERT_TRUE( start.day() == 30 );
		ASSERT_TRUE( start.hours() == 21 );
		ASSERT_TRUE( start.minutes() == 0 );
		ASSERT_TRUE( start.seconds() == 0 );

		tuner::MJDate duration = ev->duration;
		ASSERT_TRUE( duration.year() == 0 );
		ASSERT_TRUE( duration.month() == 0 );
		ASSERT_TRUE( duration.day() == 0 );
		ASSERT_TRUE( duration.hours() == 1 );
		ASSERT_TRUE( duration.minutes() == 0 );
		ASSERT_TRUE( duration.seconds() == 0 );
	}
}

TEST_F( EitTest, eit_change_version ) {
	probeSection( "eit_ver5_sn0_id1.sec" );
	probeSection( "eit_ver5_sn1_id2.sec" );

	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "eit_ver5_sn0_id1.sec" );
	probeSection( "eit_ver5_sn1_id2.sec" );

	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "eit_ver6_sn0_id3.sec" );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver6_sn1_id4.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
}

//  EPG from brazilian broadcaster 'Globo'
TEST_F( EitTest, epg_globo ) {
	updateTime( 0x10 );
	probeSection( "globo_test.ts_0.h-eit.sec" );
	ASSERT_FALSE( _eits.empty() );
	ASSERT_TRUE( wasExpired() == 0 );
	BOOST_FOREACH( const EitPtr &eit, _eits ) {
		eit->show();
	}
}

// 	Secciones disponibles para tests de cambio de version y current next indicator:
//		eit_ver12_sn0_cni0.sec  eit_ver12_sn1_cni0.sec
//		eit_ver11_sn1_cni1.sec  eit_ver12_sn0_cni1.sec
//		eit_ver11_sn0_cni0.sec	eit_ver12_sn1_cni1.sec
//		eit_ver11_sn0_cni1.sec 	eit_ver11_sn1_cni0.sec
//
//	current_next_indicator: This 1-bit indicator, when set to "1" indicates that the sub_table is the currently applicable
//	sub_table. When the bit is set to "0", it indicates that the sub_table sent is not yet applicable and shall be the next
//	sub_table to be valid.

TEST_F( EitTest, eit_current_next_indicator_00 ) {
	probeSection( "eit_ver11_sn0_cni0.sec" );
	probeSection( "eit_ver11_sn0_cni1.sec" );
	probeSection( "eit_ver11_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events = eit->events();
	ASSERT_TRUE( events.size() == 2 );
	ASSERT_TRUE( eit->version() == 11 );

	ASSERT_TRUE( eit->tableID() == 0x4e );
	ASSERT_TRUE( eit->serviceID() == 0xe750 );
	ASSERT_TRUE( eit->tsID() == 0x073a );
	ASSERT_TRUE( eit->nitID() == 0x073a );
}

TEST_F( EitTest, eit_current_next_indicator_01 ) {
	probeSection( "eit_ver11_sn0_cni1.sec" );
	probeSection( "eit_ver11_sn1_cni0.sec" );
	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, eit_current_next_indicator_02 ) {
	probeSection( "eit_ver11_sn0_cni0.sec" );
	probeSection( "eit_ver11_sn1_cni0.sec" );
	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, eit_change_version_00 ){
	probeSection( "eit_ver11_sn0_cni0.sec" );
	probeSection( "eit_ver11_sn1_cni0.sec" );
	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "eit_ver11_sn0_cni1.sec" );
	probeSection( "eit_ver11_sn1_cni0.sec" );

	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );
	probeSection( "eit_ver11_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 11 );
	}
	probeSection( "eit_ver12_sn0_cni1.sec" );

	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 1 );

	probeSection( "eit_ver12_sn0_cni1.sec" );
	probeSection( "eit_ver12_sn1_cni1.sec" );

	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 12 );
	}
}

TEST_F( EitTest, eit_change_version_01 ) {
 	probeSection( "eit_ver11_sn0_cni1.sec" );
 	probeSection( "eit_ver11_sn1_cni1.sec" );
 	ASSERT_TRUE( _eits.size() == 1 );
 	ASSERT_TRUE( wasExpired() == 0 );
 	{
 		EitPtr eit = _eits[0];
 		const tuner::eit::Events &events = eit->events();
 		ASSERT_TRUE( events.size() == 2 );
 		ASSERT_TRUE( eit->version() == 11 );
 	}

	probeSection( "eit_ver12_sn0_cni1.sec" );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver12_sn0_cni1.sec" );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver12_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 12 );
	}

	probeSection( "eit_ver12_sn0_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 0 );
	ASSERT_TRUE( wasExpired() == 0 );
}

TEST_F( EitTest, eit_change_version_02 ) {
	probeSection( "eit_ver11_sn0_cni1.sec" );
	probeSection( "eit_ver11_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 11 );
	}

	probeSection( "eit_ver12_sn0_cni0.sec" );
	ASSERT_TRUE( wasExpired() == 1 );
	probeSection( "eit_ver12_sn1_cni0.sec" );
	ASSERT_TRUE( wasExpired() == 0 );

	probeSection( "eit_ver12_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver12_sn0_cni1.sec" );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _eits.size() == 1 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 12 );
	}
}

TEST_F( EitTest, eit_change_version_03 ) {
	probeSection( "eit_ver12_sn0_cni1.sec" );
	ASSERT_TRUE( wasExpired() == 0 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver13_sn0_cni1.sec" );
	ASSERT_TRUE( wasExpired() == 1 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver12_sn1_cni1.sec");
	ASSERT_TRUE( wasExpired() ==  1 );
	ASSERT_TRUE( _eits.size() == 0 );

	probeSection( "eit_ver13_sn0_cni1.sec" );
	probeSection( "eit_ver13_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		eit->show();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 13 );
	}
}

TEST_F( EitTest, eit_change_version_04 ) {
	probeSection( "eit_ver12_sn0_cni1.sec" );
	probeSection( "eit_ver12_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 12 );
	}

	probeSection( "eit_ver11_sn0_cni1.sec" );
	probeSection( "eit_ver11_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 11 );
	}

	probeSection( "eit_ver13_sn0_cni1.sec" );
	probeSection( "eit_ver13_sn1_cni1.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );
	{
		EitPtr eit = _eits[0];
		const tuner::eit::Events &events = eit->events();
		ASSERT_TRUE( events.size() == 2 );
		ASSERT_TRUE( eit->version() == 13 );
	}
}

TEST_F( EitTest, eit_bad_formated ) {
	probeSection( "eit_sat.sec" );
	ASSERT_EQ( _eits.size(), 0 );
	ASSERT_EQ( wasExpired(), 0 );
}

TEST_F( EitTest, DISABLED_eit_basic_extended_descriptor ) {
	probeSection( "eit_extev_sn0_ver0.sec" );
	ASSERT_TRUE( _eits.size() == 1 );
	ASSERT_TRUE( wasExpired() == 0 );

	EitPtr eit = _eits[0];
	const tuner::eit::Events &events = eit->events();
	ASSERT_TRUE( events.size() == 1 );
	ASSERT_TRUE( eit->version() == 1 );

	const tuner::desc::Descriptors &desc0 = events[0]->descs;
	{
		//	first event - Short descriptor
		tuner::desc::ShortEventDescriptor se_desc;
		ASSERT_TRUE( DESC_PARSE( desc0, short_event, se_desc ) );
		const tuner::Language lang = se_desc.language;
		ASSERT_TRUE( std::string(lang.code()) == "spa" );
		ASSERT_TRUE( se_desc.event == "Terminator 2" );
		ASSERT_TRUE( se_desc.text ==  "Pelicula de ciencia ficcion donde 1 robot se envia..." );
	}

//	{
//		// first event - Extended descriptor
//		const tuner::desc::Descriptors::const_iterator it = findeDesc( desc0, tuner::desc::ident::extended_event );
//		ASSERT_TRUE( it != desc0.end() );
//		const tuner::desc::ExtendedEventDescriptor &xe_desc = (*it).second.get<tuner::desc::ExtendedEventDescriptor > ();
//
//	}

}
