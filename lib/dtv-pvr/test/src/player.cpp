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
#include <util/settings/settings.h>
#include <gtest/gtest.h>

class PlayerTest : public testing::Test {
public:
	PlayerTest() { _settings = NULL; _player = NULL; }
	virtual ~PlayerTest() {}

	virtual void SetUp() {
		_changed = 0;
		_mediaStarted = 0;
		_mediaStopped = 0;
		_blocked = 0;

		_settings = util::Settings::create("test", "memory");
		ASSERT_TRUE( _settings->initialize() );
		_settings->clear();

		_tuner=createTuner();
		ASSERT_TRUE(_tuner);

		_tunerDelegate = new BasicTunerDelegateImpl();
		ASSERT_TRUE( _tuner->initialize(_tunerDelegate) );
		createPlayer();
	}

	virtual void TearDown() {
		stopPlayer();
		_tuner->finalize();
		delete _tuner;
		delete _tunerDelegate;
		_settings->finalize();
		delete _settings;
	}

	virtual pvr::Tuner *createTuner() {
		return new BasicTuner();
	}

	void createPlayer() {
		stopPlayer();
		_player = new PlayerImpl(chs(),_settings,_tunerDelegate->session());
		_player->onChangeChannel().connect( boost::bind(&PlayerTest::onChangeChannel,this,_1,_2) );
		_player->onChannelProtected().connect( boost::bind(&PlayerTest::onProtected,this,_1) );
		_player->onMediaChanged( boost::bind(&PlayerTest::onMediaChanged,this,_1,_2) );
		_player->start();
	}

	void stopPlayer() {
		if (_player) {
			_player->stop();
			delete _player;
		}
	}

	void enableSession() {
		_tunerDelegate->session()->pass( "pepe" );
	}

	void expireSession( bool expire ) {
		if (expire) {
			_tunerDelegate->session()->expire();
		}
		else {
			_tunerDelegate->session()->check( "pepe" );
		}
	}

	void onChangeChannel( int /*chID*/, pvr::Channel * /*ch*/ ) {
		_changed++;
	}

	void onMediaChanged( const std::string & /*url*/, bool started ) {
		if (started) {
			_mediaStarted++;
		}
		else {
			_mediaStopped++;
		}
	}

	void onProtected( bool /*blocked*/ ) {
		_blocked++;
	}

	int addChannel( const std::string &name, const std::string &net="net1" ) {
		pvr::Channel *ch = new pvr::Channel( name,"01.10", net);
		ch->tuner( _tuner );
		return chs()->add( ch );
	}

	pvr::Channels *chs() {
		return _tunerDelegate->channels();
	}

	int _changed;
	int _mediaStarted, _mediaStopped;
	int _blocked;
	util::Settings *_settings;
	pvr::TunerDelegate *_tunerDelegate;
	pvr::Player *_player;
	pvr::Tuner *_tuner;
};

TEST_F( PlayerTest, basic ) {
	ASSERT_EQ( _player->current(), -1 );
	ASSERT_EQ( _player->showMobile(), false );
}

TEST_F( PlayerTest, mobile_basic ) {
	ASSERT_EQ( _player->showMobile(), false );
	_player->showMobile(true);
	ASSERT_EQ( _player->showMobile(), true );
	_player->showMobile(false);
	ASSERT_EQ( _player->showMobile(), false );
}

TEST_F( PlayerTest, get_current_network ) {
	pvr::Channel *ch = new pvr::Channel( "name", "01.10", "someNet" );
	ch->tuner( _tuner );
	int id = chs()->add( ch );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _player->currentNetworkName(), "someNet" );
}

TEST_F( PlayerTest, mobile_basic_settings ) {
	_player->showMobile(true);
	ASSERT_EQ( _player->showMobile(), true );

	//	Destroy/create/start
	createPlayer();

	ASSERT_EQ( _player->showMobile(), true );
}

TEST_F( PlayerTest, test_ok_filter ) {
	int calls=0;
	OkTestFilter filter(calls,true);
	ASSERT_EQ( calls, 0 );

	pvr::Channel ch("name","01.10", "net1");
	ASSERT_TRUE( filter.filter( &ch ) );
	ASSERT_EQ( calls, 1 );
}

TEST_F( PlayerTest, test_fail_filter ) {
	int calls=0;
	FailTestFilter filter(calls,true);
	ASSERT_EQ( calls, 0 );

	pvr::Channel ch("name","01.10", "net1");
	ASSERT_FALSE( filter.filter( &ch ) );
	ASSERT_EQ( calls, 1 );
}

TEST_F( PlayerTest, basic_filter_showMobile_false_ch_not_mobile ) {
	pvr::filter::Basic filter(false);

	pvr::Channel ch("name","01.10", "net1");
	ASSERT_TRUE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, basic_filter_showMobile_false_ch_mobile ) {
	pvr::filter::Basic filter(false);

	pvr::Channel ch("name","01.10", "net1");
	ch.mobile(true);
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, basic_filter_show_protected_false ) {
	enableSession();

	int id = addChannel("name");
	pvr::Channel *ch = chs()->get( id );

	pvr::filter::Basic filter(true);

	ch->toggleBlocked();
	expireSession(true);

	ASSERT_FALSE( filter.filter( ch ) );

	expireSession(false);
	ch->toggleBlocked();
	expireSession(true);
	ASSERT_TRUE( filter.filter( ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_false_ch_mobile_and_favorite ) {
	pvr::filter::Favorite filter(false);
	pvr::Channel ch("name","01.10", "net1");
	ch.mobile(true);
	ch.toggleFavorite();
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_false_ch_mobile_and_not_favorite ) {
	pvr::filter::Favorite filter(false);
	pvr::Channel ch("name","01.10", "net1");
	ch.mobile(true);
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_false_ch_not_mobile_and_favorite ) {
	pvr::filter::Favorite filter(false);
	pvr::Channel ch("name","01.10", "net1");
	ch.toggleFavorite();
	ASSERT_TRUE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_false_ch_not_mobile_and_not_favorite ) {
	pvr::filter::Favorite filter(false);
	pvr::Channel ch("name","01.10", "net1");
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_ch_mobile_and_favorite ) {
	pvr::filter::Favorite filter(true);
	pvr::Channel ch("name","01.10", "net1");
	ch.mobile(true);
	ch.toggleFavorite();
	ASSERT_TRUE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_ch_not_mobile_and_favorite ) {
	pvr::filter::Favorite filter(true);
	pvr::Channel ch("name","01.10", "net1");
	ch.toggleFavorite();
	ASSERT_TRUE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_ch_mobile_and_not_favorite ) {
	pvr::filter::Favorite filter(true);
	pvr::Channel ch("name","01.10", "net1");
	ch.mobile(true);
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, favorite_filter_showMobile_ch_not_mobile_and_not_favorite ) {
	pvr::filter::Favorite filter(true);
	pvr::Channel ch("name","01.10", "net1");
	ASSERT_FALSE( filter.filter( &ch ) );
}

TEST_F( PlayerTest, next_empty ) {
	int calls=0;
	BasicTestFilter filter(calls,true);
	ASSERT_EQ( _player->next(INVALID_CHANNEL,1,&filter), INVALID_CHANNEL );
	ASSERT_EQ( calls, 0 );
}

TEST_F( PlayerTest, next_one_channel ) {
	int id = addChannel( "name1" );
	int calls=0;
	OkTestFilter filter(calls,true);
	int factor;

	factor=1;
	ASSERT_EQ( _player->next(INVALID_CHANNEL,factor,&filter), id );
	ASSERT_EQ( calls, 1 );
	ASSERT_EQ( _player->next(id,factor,&filter), id );
	ASSERT_EQ( calls, 2 );

	factor=-1;
	calls=0;
	ASSERT_EQ( _player->next(INVALID_CHANNEL,factor,&filter), id );
	ASSERT_EQ( calls, 1 );
	ASSERT_EQ( _player->next(id,factor,&filter), id );
	ASSERT_EQ( calls, 2 );
}

TEST_F( PlayerTest, next_two_channel ) {
	int id1 = addChannel( "name1" );
	int id2 = addChannel( "name2" );
	int calls=0;
	OkTestFilter filter(calls,true);
	int nextID = INVALID_CHANNEL;

	//	Factor +1
	int factor = 1;
	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id1 );
	ASSERT_EQ( calls, 1 );

	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id2 );
	ASSERT_EQ( calls, 2 );

	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id1 );
	ASSERT_EQ( calls, 3 );

	//	Factor -1
	calls = 0;
	nextID = INVALID_CHANNEL;
	factor = -1;
	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id2 );
	ASSERT_EQ( calls, 1 );

	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id1 );
	ASSERT_EQ( calls, 2 );

	nextID = _player->next(nextID,factor,&filter);
	ASSERT_EQ( nextID, id2 );
	ASSERT_EQ( calls, 3 );
}

TEST_F( PlayerTest, next_check_for_loop ) {
	addChannel( "name1" );
	addChannel( "name2" );
	int calls=0;
	FailTestFilter filter(calls,true);

	ASSERT_EQ( _player->next(INVALID_CHANNEL,1,&filter), INVALID_CHANNEL );
	ASSERT_EQ( calls, 2 );
}

TEST_F( PlayerTest, next_neg_factor_same_net ) {
	int calls=0;
	OkTestFilter filter(calls,true);
	int ch1 = addChannel( "name1" );
	addChannel( "name2" );
	int ch3 = addChannel( "name3");

	ASSERT_EQ( ch3, _player->next(ch1, -1, &filter));
}

TEST_F( PlayerTest, next_neg_factor_different_net ) {
	int calls=0;
	OkTestFilter filter(calls,true);
	int ch1 = addChannel( "name1" );
	addChannel( "name2" );
	int ch3 = addChannel( "name3", "net2" );

	ASSERT_EQ( ch3, _player->next(ch1, -1, &filter));
}

TEST_F( PlayerTest, change_to_invalid ) {
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( INVALID_CHANNEL ), INVALID_CHANNEL );
	ASSERT_EQ( _player->current(), INVALID_CHANNEL );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_unknown_channel ) {
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( 5 ), INVALID_CHANNEL );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_valid ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _player->current(), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
}

class PlayerTestCantReserve : public PlayerTest {
public:
	virtual pvr::Tuner *createTuner() {
		return new TunerCantReserve();
	}
};

TEST_F( PlayerTestCantReserve, change_to_tuner_without_player ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), INVALID_CHANNEL );
	ASSERT_EQ( _player->current(), INVALID_CHANNEL );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_channel_without_tuner ) {
	int id = chs()->add( new pvr::Channel( "algo","01.10", "net1") );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), INVALID_CHANNEL );
	ASSERT_EQ( _player->current(), INVALID_CHANNEL );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_valid_to_unknown ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );

	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	ASSERT_EQ( _player->change( 5 ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_valid_to_same ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );

	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_valid_to_invalid ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( INVALID_CHANNEL ), INVALID_CHANNEL );
	ASSERT_EQ( _changed, 2 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 1 );
}

TEST_F( PlayerTest, change_valid_to_other_valid ) {
	int id1 = addChannel( "name1" );
	int id2 = addChannel( "name2" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );

	//	Change from invalid->id1
	ASSERT_EQ( _player->change( id1 ), id1 );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	//	Change from id1 -> id2
	ASSERT_EQ( _player->change( id2 ), id2 );
	ASSERT_EQ( _changed, 2 );
	ASSERT_EQ( _mediaStarted, 2 );
	ASSERT_EQ( _mediaStopped, 1 );

	ASSERT_EQ( _tuner->allocatedPlayers(), 1 );
}

TEST_F( PlayerTest, change_valid_toogle_blocked_and_change_to_invalid ) {
	ASSERT_FALSE( _player->isProtected() );
	enableSession();

	int id = addChannel( "name1" );
	pvr::Channel *ch = chs()->get( id );

	ASSERT_EQ( _blocked, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_FALSE( _player->isProtected() );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	ch->toggleBlocked();
	expireSession(true);
	ASSERT_TRUE( _player->isProtected() );
	ASSERT_EQ( _blocked, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 1 );

	ASSERT_EQ( _player->change( INVALID_CHANNEL ), INVALID_CHANNEL );
	ASSERT_FALSE( _player->isProtected() );
	ASSERT_EQ( _blocked, 2 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 1 );
}

TEST_F( PlayerTest, change_channel_not_protected ) {
	ASSERT_FALSE( _player->isProtected() );
	int id = addChannel( "name1" );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _blocked, 0 );
	ASSERT_FALSE( _player->isProtected() );
}

TEST_F( PlayerTest, change_block_before_change ) {
	enableSession();
	ASSERT_FALSE( _player->isProtected() );

	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );

	int id = addChannel( "name1" );
	pvr::Channel *ch = chs()->get( id );
	ch->toggleBlocked();
	expireSession(true);
	ASSERT_TRUE( ch->isProtected() );

	ASSERT_EQ( _player->change( id ), id );

	ASSERT_TRUE( _player->isProtected() );
	ASSERT_EQ( _blocked, 1 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
}

TEST_F( PlayerTest, change_block_after_change ) {
	ASSERT_FALSE( _player->isProtected() );
	enableSession();

	int id = addChannel( "name1" );
	pvr::Channel *ch = chs()->get( id );

	ASSERT_EQ( _blocked, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_FALSE( _player->isProtected() );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	ch->toggleBlocked();
	expireSession(true);
	ASSERT_TRUE( _player->isProtected() );
	ASSERT_EQ( _blocked, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 1 );

	expireSession(false);
	ch->toggleBlocked();
	expireSession(true);
	ASSERT_FALSE( _player->isProtected() );
	ASSERT_EQ( _blocked, 2 );
	ASSERT_EQ( _mediaStarted, 2 );
	ASSERT_EQ( _mediaStopped, 1 );
}

TEST_F( PlayerTest, change_unblock_session_after_change ) {
	enableSession();
	int id = addChannel( "name1" );
	pvr::Channel *ch = chs()->get( id );
	ch->toggleBlocked();
	expireSession(true);

	ASSERT_EQ( _player->change( id ), id );
	ASSERT_TRUE( _player->isProtected() );
	ASSERT_EQ( _blocked, 1 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );

	//	Disable session
	ASSERT_TRUE( _tunerDelegate->session()->check("pepe") );

	ASSERT_FALSE( _player->isProtected() );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _blocked, 2 );
}

TEST_F( PlayerTest, remove_channel_played_with_only_one_channel ) {
	int id=addChannel( "name1" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 0 );

	chs()->remove( id );

	ASSERT_EQ( _player->current(), INVALID_CHANNEL );
	ASSERT_EQ( _mediaStarted, 1 );
	ASSERT_EQ( _mediaStopped, 1 );
}

TEST_F( PlayerTest, remove_channel_played_with_more_channels ) {
	int id=addChannel( "name1" );
	int id2=addChannel( "name2" );
	ASSERT_EQ( _changed, 0 );
	ASSERT_EQ( _mediaStarted, 0 );
	ASSERT_EQ( _mediaStopped, 0 );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _changed, 1 );

	chs()->remove( id );
	ASSERT_EQ( _player->current(), id2 );

	ASSERT_EQ( _mediaStarted, 2 );
	ASSERT_EQ( _mediaStopped, 1 );
}

//	status
TEST_F( PlayerTest, status_invalid ) {
	int sig, quality;
	_player->status( sig, quality );
	ASSERT_EQ( sig, -1 );
	ASSERT_EQ( quality, -1 );
}

TEST_F( PlayerTest, status_current_not_invalid ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _player->current(), id );

	{
		int sig, quality;
		_player->status( sig, quality );
		ASSERT_TRUE( sig >= 0 );
		ASSERT_TRUE( quality >= 0 );
	}
}

TEST_F( PlayerTest, isProtected_invalid ) {
	ASSERT_FALSE( _player->isProtected() );
}

//	player methods
TEST_F( PlayerTest, player_invalid ) {
	ASSERT_EQ( _player->nextVideo(), -1 );
	ASSERT_EQ( _player->videoCount(), 0 );
	{
		pvr::info::Basic info;
		_player->videoInfo(info);
		ASSERT_EQ( info.pid, -1 );
		ASSERT_EQ( info.codec, "Unknown" );
	}
	ASSERT_EQ( _player->nextAudio(), -1 );
	ASSERT_EQ( _player->audioCount(), 0 );
	{
		pvr::info::Audio info;
		_player->audioInfo(info);
		ASSERT_EQ( info.pid, -1 );
		ASSERT_EQ( info.codec, "Unknown" );
		ASSERT_EQ( info.lang, "Unknown" );
	}
	ASSERT_EQ( _player->nextSubtitle(), -1 );
	ASSERT_EQ( _player->subtitleCount(), 0 );
	{
		pvr::info::Basic info;
		_player->subtitleInfo(info);
		ASSERT_EQ( info.pid, -1 );
		ASSERT_EQ( info.codec, "Unknown" );
	}
}

TEST_F( PlayerTest, player_methods_not_invalid ) {
	int id = addChannel( "name1" );
	ASSERT_EQ( _player->change( id ), id );
	ASSERT_EQ( _player->current(), id );

	ASSERT_EQ( _player->nextVideo(), 1 );
	ASSERT_EQ( _player->videoCount(), 1 );
	ASSERT_EQ( _player->nextAudio(), 1 );
	ASSERT_EQ( _player->audioCount(), 1 );
	ASSERT_EQ( _player->nextSubtitle(), -1 );
	ASSERT_EQ( _player->subtitleCount(), 0 );
}

