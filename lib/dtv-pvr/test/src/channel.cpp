/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-pvr implementation.

    DTV-pvr is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-pvr is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-pvr.

    DTV-pvr es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-pvr se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "types.h"
#include "../../src/time.h"
#include <util/serializer/binary.h>
#include <util/serializer/xml.h>
#include <util/buffer.h>
#include <gtest/gtest.h>

namespace bpt=boost::posix_time;

class ChannelTest : public testing::Test {
public:
	ChannelTest() { _changed = 0; }
	virtual ~ChannelTest() {}

	virtual void SetUp() {
		ch.onChanged().connect( boost::bind(&ChannelTest::onChanged,this,_1) );
	}

	virtual void TearDown() {
	}

	void onChanged( pvr::Channel * /*ch*/ ) {
		_changed++;
	}

	int _changed;
	pvr::Channel ch;
};

class ChannelTestOnline : public ChannelTest {
public:
	virtual void SetUp() {
		delegate = createDelegate();
		ChannelTest::SetUp();
		ch.channelID(10);
		ASSERT_TRUE( tuner.initialize(delegate) );
		ch.tuner(&tuner);
		ASSERT_TRUE( ch.isOnline() );
	}

	virtual void TearDown() {
		delegate->session()->check( "pepe" ); // Unblock session
		ChannelTest::TearDown();
		tuner.finalize();
		delete delegate;
	}

	virtual BasicTunerDelegate *createDelegate() {
		return (BasicTunerDelegate *) new BasicTunerDelegateImpl();
	}

	void enableSession() {
		delegate->session()->pass( "pepe" );
	}

	void expireSession() {
		delegate->session()->expire();
	}

	BasicTuner tuner;
	BasicTunerDelegate *delegate;
};

class ChannelTestPersistence : public ChannelTestOnline {
public:
	ChannelTestPersistence() {
		_settings = util::Settings::create("test", "memory");
		_settings->initialize();
		_settings->clear();

		_session = new pvr::parental::Session(_settings);
	}
	~ChannelTestPersistence() {
		delete _session;
		_settings->finalize();
		delete _settings;
	}

	virtual void SetUp() {
		ChannelTestOnline::SetUp();
		_save = 0;
	}

	virtual BasicTunerDelegate *createDelegate() {
		return new BasicTunerDelegate(NULL, new PersistenceChannels(_save, _session));
	}

	int _save;
	util::Settings *_settings;
	pvr::parental::Session *_session;
};

class TunerLogo : public pvr::Tuner {
public:
	TunerLogo() : pvr::Tuner("prueba") {}

protected:
	virtual pvr::TunerPlayer *allocPlayer() const { return NULL; }
};

static boost::gregorian::date createDate( int day, int month, int year ) {
	return boost::gregorian::date( year, month, day );
}

static bpt::time_duration createTime( int hours, int min, int secs ) {
	return bpt::time_duration( hours, min, secs );
}

static pvr::bpt::time_period periodBetween( const bpt::time_duration &start_dur, const bpt::time_duration &end_dur ) {
	boost::gregorian::date d = createDate(1,1,2013);
	bpt::ptime start(d, start_dur);
	bpt::ptime end(d, end_dur);
	return pvr::bpt::time_period(start, end);
}

TEST( Channel, default_constructor ) {
	pvr::Channel ch;
	ASSERT_EQ( ch.channelID(), -1 );
	ASSERT_EQ( ch.name(), "Invalid" );
	ASSERT_EQ( ch.channel(), "99.99" );
	ASSERT_EQ( ch.tunerName(), "" );
	ASSERT_EQ( ch.uri(), "" );
	ASSERT_EQ( ch.logo(), "" );
	ASSERT_FALSE( ch.isMobile() );
	ASSERT_TRUE( ch.isFree() );
	ASSERT_FALSE( ch.isFavorite() );
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_FALSE( ch.parentalControl() == NULL );
	ASSERT_FALSE( ch.isOnline() );
	ASSERT_FALSE( ch.isProtected() );
	ASSERT_EQ( ch.category(), pvr::category::unknown );
}

TEST( Channel, constructor ) {
	pvr::Channel ch("name","01.10", "net1");
	ASSERT_EQ( ch.channelID(), -1 );
	ASSERT_EQ( ch.name(), "name" );
	ASSERT_EQ( ch.channel(), "01.10" );
	ASSERT_EQ( ch.tunerName(), "" );
	ASSERT_EQ( ch.uri(), "" );
	ASSERT_EQ( ch.logo(), "" );
	ASSERT_FALSE( ch.isMobile() );
	ASSERT_TRUE( ch.isFree() );
	ASSERT_FALSE( ch.isFavorite() );
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_FALSE( ch.parentalControl() == NULL );
	ASSERT_FALSE( ch.isOnline() );
	ASSERT_FALSE( ch.isProtected() );
	ASSERT_EQ( ch.category(), pvr::category::unknown );
}

TEST( Channel, storage ) {
	util::Buffer buf;

	{	//	Save
		pvr::Channel ch("name1","01.10", "net1");
		ch.channelID( 5 );
		ch.toggleFavorite();
		ch.toggleBlocked();
		ch.mobile(true);
		ch.isFree(false);
		ch.uri( "impl0" );
		util::serialize::Binary ser(buf);
		ASSERT_TRUE( ser.save( ch ) );
	}

	//	Load
	pvr::Channel ch;
	util::serialize::Binary ser(buf);
	ser.load( ch );

	ASSERT_EQ( ch.name(), "name1" );
	ASSERT_EQ( ch.channel(), "01.10" );
	ASSERT_EQ( ch.network(), "net1" );
	ASSERT_EQ( ch.channelID(), 5 );
	ASSERT_TRUE( ch.isFavorite() );
	ASSERT_TRUE( ch.isBlocked() );
	ASSERT_TRUE( ch.isMobile() );
	ASSERT_FALSE( ch.isFree() );
	//ASSERT_EQ( ch.tunerName(), "tuner0" );
	ASSERT_EQ( ch.uri(), "impl0" );
}

TEST_F( ChannelTest, channelID ) {
	ASSERT_EQ( ch.channelID(), -1 );
	ch.channelID( 10 );
	ASSERT_EQ( ch.channelID(), 10 );
}

TEST_F( ChannelTest, uri ) {
	ASSERT_EQ( ch.uri(), "" );
	ch.uri( "pepe1" );
	ASSERT_EQ( ch.uri(), "pepe1" );
}

TEST_F( ChannelTest, show ) {
	boost::gregorian::date d = createDate(1,1,2013);

	{
		pvr::Show *s;
		bpt::ptime start( d, createTime(5,0,0) );
		bpt::ptime end( d, createTime(6,0,0) );
		pvr::bpt::time_period r(start,end);
		s = new pvr::Show( "name1", "desc1", r );
		ch.addShow( s );
		ASSERT_EQ( _changed, 0 );
	}

	{	//	Time in show period
		bpt::ptime t( d, createTime(5,30,0) );
		pvr::Show *s = ch.getShow(t);
		ASSERT_TRUE(s);
		ASSERT_EQ( s->name(), "name1" );
	}

	{	//	Time not in show period
		bpt::ptime t( d, createTime(6,30,0) );
		pvr::Show *s = ch.getShow(t);
		ASSERT_TRUE(s);
		ASSERT_NE( s->name(), "name1" );
	}
}

TEST_F( ChannelTestOnline, show_online ) {
	boost::gregorian::date d = createDate(1,1,2013);
	{
		pvr::Show *s;
		bpt::ptime start( d, createTime(5,0,0) );
		bpt::ptime end( d, createTime(6,0,0) );
		pvr::bpt::time_period r(start,end);
		s = new pvr::Show( "name1", "desc1", r );
		ch.addShow( s );
		ASSERT_EQ( _changed, 0 );
	}
}

TEST_F( ChannelTestOnline, current_show_online ) {
	bpt::ptime now;
	pvr::time::clock()->get( now );
	pvr::bpt::time_period r(now, now + createTime(1,0,0));
	pvr::Show *s = new pvr::Show( "name1", "desc1", r );
	ch.addShow( s );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ(s, ch.getShow(now));
}

TEST_F( ChannelTestOnline, get_current_show_when_it_is_protected ) {
	bpt::ptime now;
	pvr::time::clock()->get( now );
	pvr::bpt::time_period r(now, now + createTime(1,0,0));

	pvr::Show *show = new pvr::Show( "name1", "desc1", r );
	show->parentalControl(sexControl());
	ch.addShow(show);

	{ // Restrict sex
		enableSession();
		delegate->session()->restrictSex(true);
		expireSession();
	}

	pvr::Show *s = ch.getCurrentShow();

	ASSERT_TRUE(s);
	ASSERT_NE(show, s);
	ASSERT_STREQ("", s->name().c_str());
	ASSERT_STREQ("", s->description().c_str());
	ASSERT_TRUE(s->timeRange().is_null());
}

TEST_F( ChannelTest, mobile ) {
	ASSERT_FALSE( ch.isMobile() );
	ch.mobile( true );
	ASSERT_TRUE( ch.isMobile() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, mobile_online ) {
	ASSERT_FALSE( ch.isMobile() );
	ch.mobile( true );
	ASSERT_TRUE( ch.isMobile() );
	ASSERT_EQ( _changed, 1 );
}

TEST_F( ChannelTest, free ) {
	ASSERT_TRUE( ch.isFree() );
	ch.isFree( false );
	ASSERT_FALSE( ch.isFree() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, free_online ) {
	ASSERT_TRUE( ch.isFree() );
	ch.isFree( false );
	ASSERT_FALSE( ch.isFree() );
	ASSERT_EQ( _changed, 1 );
}

TEST_F( ChannelTest, favorite ) {
	ASSERT_FALSE( ch.isFavorite() );
	ch.toggleFavorite();
	ASSERT_TRUE( ch.isFavorite() );
	ch.toggleFavorite();
	ASSERT_FALSE( ch.isFavorite() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, favorite_online ) {
	ASSERT_FALSE( ch.isFavorite() );
	ch.toggleFavorite();
	ASSERT_TRUE( ch.isFavorite() );
	ch.toggleFavorite();
	ASSERT_FALSE( ch.isFavorite() );
	ASSERT_EQ( _changed, 2 );
}

TEST_F( ChannelTest, blocked_unblock ) {
	ASSERT_FALSE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_TRUE( ch.isBlocked() );
	ch.unblock();
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, blocked_unblock_online ) {
	ASSERT_FALSE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_TRUE( ch.isBlocked() );
	ch.unblock();
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_EQ( _changed, 2 );
}

TEST_F( ChannelTest, blocked_toggle ) {
	ASSERT_FALSE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_TRUE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, blocked_toggle_online ) {
	ASSERT_FALSE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_TRUE( ch.isBlocked() );
	ch.toggleBlocked();
	ASSERT_FALSE( ch.isBlocked() );
	ASSERT_EQ( _changed, 2 );
}

TEST_F( ChannelTest, parental_set ) {
	ch.parentalControl( sexControl() );
	ASSERT_TRUE( ch.parentalControl()->hasSex() );
	ASSERT_EQ( _changed, 0 );
}

TEST_F( ChannelTestOnline, parental_set_online ) {
	pvr::parental::Control *c = sexControl();
	ch.parentalControl( c );
	ASSERT_TRUE( ch.parentalControl() == c );
	ASSERT_EQ( _changed, 1 );
}

TEST_F( ChannelTestOnline, parental_channel_empty_and_show_not_empty_result_show ) {
	pvr::parental::Control *c = ch.parentalControl();

	{
		pvr::Show *s;
		bpt::ptime start = bpt::microsec_clock::local_time();
		bpt::ptime end = bpt::microsec_clock::local_time()+bpt::hours(1);
		pvr::bpt::time_period r(start,end);
		s = new pvr::Show( "name1", "desc1", r );
		s->parentalControl( sexControl() );
		ch.addShow( s );
	}

	ASSERT_NE( c, ch.parentalControl() );
}

TEST_F( ChannelTestOnline, parental_channel_and_show_result_channel ) {
	pvr::parental::Control *c = sexControl();
	ch.parentalControl( c );

	{
		pvr::Show *s;
		bpt::ptime start = bpt::microsec_clock::local_time();
		bpt::ptime end = bpt::microsec_clock::local_time()+bpt::hours(1);
		pvr::bpt::time_period r(start,end);
		s = new pvr::Show( "name1", "desc1", r );
		s->parentalControl( sexControl() );
		ch.addShow( s );
	}

	ASSERT_EQ( c, ch.parentalControl() );
}

TEST_F( ChannelTestOnline, parental_set_attached_same_restrictions ) {
	ch.parentalControl( sexControl() );
	ASSERT_EQ( _changed, 1 );
	ch.parentalControl( sexControl() );
	ASSERT_EQ( _changed, 1 );
}

TEST_F( ChannelTestOnline, logo ) {
	ASSERT_EQ( ch.logo(), "" );

	delegate->logos()->add( new BasicLogosProvider() );
	delegate->logos()->start();
	ASSERT_EQ( ch.logo(), "pepe" );
	delegate->logos()->stop();
}

TEST_F( ChannelTestOnline, isProtected_blocked_session_disabled ) {
	ch.toggleBlocked();
	ASSERT_FALSE( ch.isProtected() );
}

TEST_F( ChannelTestOnline, isProtected_control_session_disabled ) {
	ch.parentalControl( sexControl() );
	ASSERT_FALSE( ch.isProtected() );
}

TEST_F( ChannelTestOnline, isProtected_blocked_session_enabled_not_active ) {
	//	Enable session
	delegate->session()->pass( "pepe" );
	ch.toggleBlocked();

	delegate->session()->expire();
	//	Test
	ASSERT_TRUE( ch.isProtected() );
}

TEST_F( ChannelTestOnline, isProtected_when_exists_parental_control_by_channel_and_session_exists_but_expired ) {
	{ // Configura session and expire it !
		delegate->session()->pass( "pepe" );
		delegate->session()->restrictSex(true);
		delegate->session()->expire();
	}

	ch.parentalControl( sexControl() );
	ASSERT_TRUE( ch.isProtected() );
}

TEST_F( ChannelTest, isProtected_blocked_session_enabled_active ) {
	BasicTuner tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_TRUE( tuner.initialize(&delegate) );
	ch.tuner(&tuner);

	//	Enable session (and activate!)
	delegate.session()->pass( "pepe" );

	//	Test
	ch.toggleBlocked();
	ASSERT_FALSE( ch.isProtected() );

	tuner.finalize();
}

TEST_F( ChannelTest, isProtected_parental_session_enabled_active ) {
	BasicTuner tuner;
	BasicTunerDelegateImpl delegate;
	ASSERT_TRUE( tuner.initialize(&delegate) );
	ch.tuner(&tuner);

	//	Enable session (and activate!)
	delegate.session()->pass( "pepe" );
	delegate.session()->restrictSex(true);

	//	Test
	ch.parentalControl( sexControl() );
	ASSERT_FALSE( ch.isProtected() );

	tuner.finalize();
}

TEST_F( ChannelTestOnline, isProtected_when_no_channel_control_and_show_control_exists_and_session_was_expired ) {
	{//	Configure session and expire it!
		delegate->session()->pass( "pepe" );
		delegate->session()->restrictSex(true);
		delegate->session()->expire();
	}

	{// Add protected show
		bpt::ptime start = bpt::microsec_clock::local_time();
		bpt::ptime end = bpt::microsec_clock::local_time() + bpt::hours(1);
		pvr::bpt::time_period r(start, end);
		pvr::Show *show = new pvr::Show( "name1", "desc1", r );
		show->parentalControl(sexControl());
		ch.addShow(show);
	}

	ASSERT_TRUE( ch.isProtected() );
}

TEST_F( ChannelTestOnline, get_show ) {
	boost::gregorian::date d = createDate(1,1,2013);
	bpt::ptime start( d, createTime(5,0,0) );
	bpt::ptime end( d, createTime(6,0,0) );
	pvr::bpt::time_period r(start,end);

	pvr::Show *show = new pvr::Show( "name1", "desc1", r );
	ch.addShow(show);

	ASSERT_EQ(show, ch.getShow(start));
}

TEST_F( ChannelTestOnline, get_show_when_it_is_protected ) {
	boost::gregorian::date d = createDate(1,1,2013);
	bpt::ptime start( d, createTime(5,0,0) );
	bpt::ptime end( d, createTime(6,0,0) );
	pvr::bpt::time_period r(start, end);

	pvr::Show *show = new pvr::Show( "name1", "desc1", r );
	show->parentalControl(sexControl());

	{ // Restrict sex
		delegate->session()->pass( "pepe" );
		delegate->session()->restrictSex(true);
		delegate->session()->expire();
	}

	ch.addShow(show);
	pvr::Show *s = ch.getShow(start);

	ASSERT_TRUE(s);
	ASSERT_NE(show, s);
	ASSERT_STREQ("", s->name().c_str());
	ASSERT_STREQ("", s->description().c_str());
	ASSERT_TRUE(s->timeRange().is_null());
}

TEST_F( ChannelTestOnline, get_show_when_there_is_not ) {
	boost::gregorian::date d = createDate(1,1,2013);
	bpt::ptime start( d, createTime(5,0,0) );
	bpt::ptime end( d, createTime(6,0,0) );
	pvr::bpt::time_period r(start, end);

	pvr::Show *show = new pvr::Show( "name1", "desc1", r );

	ch.addShow(show);
	pvr::Show *s = ch.getShow(bpt::ptime(d, createTime(7, 0, 0)));

	ASSERT_TRUE(s);
	ASSERT_NE(show, s);
	ASSERT_STREQ("", s->name().c_str());
	ASSERT_STREQ("", s->description().c_str());
	ASSERT_TRUE(s->timeRange().is_null());
}

TEST_F( ChannelTest, add_two_shows ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::bpt::time_period r2 = periodBetween(createTime(6,0,0), createTime(8,0,0));
	ch.addShow(new pvr::Show( "name2", "desc2", r2 ));

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r.span(r2));

	ASSERT_EQ(2, shows.size());
}

TEST_F( ChannelTest, add_show_that_overlaps_each_other ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::Show *secondShow = new pvr::Show( "name2", "desc2", r );
	ch.addShow(secondShow);

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r);

	ASSERT_EQ(1, shows.size());
	ASSERT_EQ(secondShow, shows[0]);
}

TEST_F( ChannelTest, add_show_that_partially_overlaps_each_other ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::bpt::time_period r2 = periodBetween(createTime(5,20,0), createTime(5,45,0));
	pvr::Show *secondShow = new pvr::Show( "name2", "desc2", r2 );
	ch.addShow(secondShow);

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r);

	ASSERT_EQ(1, shows.size());
	ASSERT_EQ(secondShow, shows[0]);
}

TEST_F( ChannelTest, add_show_that_partially_overlaps_each_other_2 ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::bpt::time_period r2 = periodBetween(createTime(5,30,0), createTime(6,30,0));
	pvr::Show *secondShow = new pvr::Show( "name2", "desc2", r2 );
	ch.addShow(secondShow);

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r);

	ASSERT_EQ(1, shows.size());
	ASSERT_EQ(secondShow, shows[0]);
}

TEST_F( ChannelTest, add_show_that_partially_overlaps_each_other_3 ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::bpt::time_period r2 = periodBetween(createTime(4,30,0), createTime(6,30,0));
	pvr::Show *secondShow = new pvr::Show( "name2", "desc2", r2 );
	ch.addShow(secondShow);

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r);

	ASSERT_EQ(1, shows.size());
	ASSERT_EQ(secondShow, shows[0]);
}

TEST_F( ChannelTest, add_show_that_partially_overlaps_each_other_4 ) {
	pvr::bpt::time_period r = periodBetween(createTime(5,0,0), createTime(6,0,0));
	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	pvr::bpt::time_period r2 = periodBetween(createTime(4,30,0), createTime(5,30,0));
	pvr::Show *secondShow = new pvr::Show( "name2", "desc2", r2 );
	ch.addShow(secondShow);

	pvr::Channel::Shows shows;
	ch.getShowsBetween(shows, r);

	ASSERT_EQ(1, shows.size());
	ASSERT_EQ(secondShow, shows[0]);
}

TEST_F( ChannelTestPersistence, set_mobile ) {
	ASSERT_FALSE(ch.isMobile());
	ASSERT_EQ(0, _save);
	ch.mobile(true);
	ASSERT_EQ(1, _save);
}

TEST_F( ChannelTestPersistence, set_mobile_wihout_change_state ) {
	ASSERT_FALSE(ch.isMobile());
	ASSERT_EQ(0, _save);
	ch.mobile(false);
	ASSERT_EQ(0, _save);
}

TEST_F( ChannelTestPersistence, set_free ) {
	ASSERT_TRUE(ch.isFree());
	ASSERT_EQ(0, _save);
	ch.isFree(false);
	ASSERT_EQ(1, _save);
}

TEST_F( ChannelTestPersistence, set_free_wihout_change_state ) {
	ASSERT_TRUE(ch.isFree());
	ASSERT_EQ(0, _save);
	ch.isFree(true);
	ASSERT_EQ(0, _save);
}

TEST_F( ChannelTestPersistence, toggleFavorite ) {
	ASSERT_EQ(0, _save);
	ch.toggleFavorite();
	ASSERT_EQ(1, _save);
}

TEST_F( ChannelTestPersistence, toggleBlock ) {
	ASSERT_EQ(0, _save);
	ch.toggleBlocked();
	ASSERT_EQ(1, _save);
}

TEST_F( ChannelTestPersistence, unblock ) {
	ASSERT_EQ(0, _save);
	ch.toggleBlocked();
	ASSERT_EQ(1, _save);
	ch.unblock();
	ASSERT_EQ(2, _save);
}

TEST_F( ChannelTestPersistence, unblock_unblocked ) {
	ASSERT_EQ(0, _save);
	ch.unblock();
	ASSERT_EQ(0, _save);
}

TEST_F( ChannelTestPersistence, set_parental_control ) {
	ASSERT_EQ(0, _save);
	ch.parentalControl( sexControl() );
	ASSERT_EQ( _save, 0 );
}

TEST_F( ChannelTestPersistence, set_parental_control_without_change_state ) {
	ASSERT_EQ(0, _save);
	ch.parentalControl( sexControl() );
	ASSERT_EQ( _save, 0 );

	ch.parentalControl( sexControl() );
	ASSERT_EQ( _save, 0 );
}

TEST_F( ChannelTestPersistence, add_show ) {
	boost::gregorian::date d = createDate(1,1,2013);
	bpt::ptime start( d, createTime(5,0,0) );
	bpt::ptime end( d, createTime(6,0,0) );
	pvr::bpt::time_period r(start,end);

	ASSERT_EQ(0, _save);

	ch.addShow(new pvr::Show( "name1", "desc1", r ));

	ASSERT_EQ( _save, 0 );
}
