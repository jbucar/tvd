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
#include "../extension.h"
#include "../../../../../src/service/extension/player/esplayer.h"
#include "../../../../../src/service/extension/player/delegate.h"
#include "../../../../../src/service/extension/player/extension.h"
#include "../../../../../src/service/extension/player/player.h"
#include "../../../../../src/demuxer/ts.h"

namespace tv = tuner::player;


static bool findVideoStream( const tuner::player::Stream *sInfo ) {
	return sInfo->info.streamType == 0x1b && sInfo->info.pid == 1001;
}

static bool findAudioStream( const tuner::player::Stream *sInfo ) {
	return sInfo->info.streamType == 0x11 && sInfo->info.pid == 1002;
}

class PlayerInitFailed : public tuner::player::Player {
public:
	PlayerInitFailed( tuner::player::Delegate *delegate ) : tuner::player::Player(delegate) {}

protected:
	virtual bool initialize() { return false; }
};

class PlayerTest : public ServiceManagerTest {
public:
	PlayerTest() {}
	virtual ~PlayerTest() {}

	virtual tv::Delegate *createDelegate() const {
		return new tv::Delegate();
	}
};

TEST_F( PlayerTest, constructor ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	delete player;
}

TEST_F( PlayerTest, basic ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	ASSERT_TRUE( player->initialize() );
	initMgr( player->extension() );
	finMgr( player->extension() );
	player->finalize();
	delete player;
}

TEST_F( PlayerTest, initialize_twice ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	ASSERT_TRUE( player->initialize() );
	ASSERT_DEATH( player->initialize(), "" );
	player->finalize();
	delete player;
}

TEST_F( PlayerTest, initialize_and_subclass_fail ) {
	tuner::player::Player *player = new PlayerInitFailed( createDelegate() );
	ASSERT_FALSE( player->initialize() );
	delete player;
}

TEST_F( PlayerTest, finalize_without_initialize ) {
	tuner::player::Player *player = new PlayerInitFailed( createDelegate() );
	player->finalize();
	delete player;
}

TEST_F( PlayerTest, attach_extension_without_initialize ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	ASSERT_DEATH( initMgr( player->extension() ), "" );
	delete player;
}

TEST_F( PlayerTest, finalize_without_detach ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	ASSERT_TRUE( player->initialize() );
	initMgr( player->extension() );
	ASSERT_DEATH( player->finalize(), "" );
	finMgr( player->extension() );
	player->finalize();
	delete player;
}

TEST_F( PlayerTest, playservice_without_initialize ) {
	tuner::player::Player *player = new tuner::player::Player( createDelegate() );
	ASSERT_DEATH( player->playService( 0x10 ), "" );
	delete player;
}

class TvPlayerTest : public PlayerTest {
public:
	TvPlayerTest() {
		_player = NULL;
	}
	virtual ~TvPlayerTest() {}

	virtual void SetUp() {
		PlayerTest::SetUp();
		setupPlayer();
		ASSERT_TRUE( _player->initialize() );
		initMgr( _player->extension() );
	}

	virtual void TearDown() {
		mgr()->cleanAll();
		finMgr( _player->extension() );
		_player->finalize();
		delete _player;
		_player = NULL;
		PlayerTest::TearDown();
	}

	virtual tv::Delegate *createDelegate() const {
		return new tv::Delegate();
	}

	virtual void setupPlayer() {
		tv::Delegate *dlg = createDelegate();
		_player = new tv::Player( dlg );
		_player->addDefaultsStreams();
		_player->addPlayer( new tv::es::Player( tuner::pes::type::video) );
		_player->addPlayer( new tv::es::Player( tuner::pes::type::audio) );
		_player->addPlayer( new tv::es::Player( tuner::pes::type::subtitle, -1 ) );
	}

	tuner::Service *startService( tuner::ID srvID, const std::vector<tuner::ElementaryInfo> &elems ) {
		tuner::ID srvPID = 100;
		tuner::ID pcrPID = 1000;
		tuner::Pmt *pmt = test::createPmt( srvID, srvPID, pcrPID, elems );
		tuner::Service *srv = test::createService( srvID, srvPID, pmt );
		startSrv( srv );
		return srv;
	}

	tuner::Service *startService( tuner::ID srvID ) {
		std::vector<tuner::ElementaryInfo> elems;
		test::addStream( elems, 0x1b, 1001, 0x81 );
		test::addStream( elems, 0x11, 1002, 0x90 );
		return startService( srvID, elems );
	}

	void onStarted( bool started ) {
		if (started) {
			ASSERT_TRUE( _player->url() != "" );
		}

		_starts.push_back( started );
	}

	tv::Player *_player;
	std::vector<bool> _starts;
};

TEST_F( TvPlayerTest, constructor ) {
}

TEST_F( TvPlayerTest, stopservice_without_play ) {
	_player->stopService();
}

TEST_F( TvPlayerTest, getters_without_play ) {
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	{
		const tuner::player::Stream *stream = _player->findStream( findAudioStream );
		ASSERT_TRUE( stream == NULL );
	}
}

TEST_F( TvPlayerTest, play ) {
	tuner::ID srvID = 0xe764;
	_player->onStarted( boost::bind(&TvPlayerTest::onStarted,this,_1) );

	_player->playService( srvID );
	ASSERT_TRUE( _starts.size() == 0);
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );

	tuner::Service *srv=startService( srvID );
	ASSERT_TRUE( _starts.size() == 1);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _player->isStarted() == true );
	ASSERT_EQ( _player->url(), "decode://audioPID=1002,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );
	ASSERT_TRUE( _player->getAudio() != NULL );
	ASSERT_TRUE( _player->audioCount() == 1 );
	ASSERT_TRUE( _player->nextAudio() == 0 );
	ASSERT_TRUE( _player->getVideo() != NULL );
	ASSERT_TRUE( _player->videoCount() == 1 );
	ASSERT_TRUE( _player->nextVideo() == 0 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == 1000 );

	_player->stopService();
	ASSERT_TRUE( _starts.size() == 2);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );

	_player->playService( srvID );
	ASSERT_TRUE( _starts.size() == 3);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _starts[2] == true);
	ASSERT_TRUE( _player->isStarted() == true );
	ASSERT_TRUE( _player->url() == "decode://audioPID=1002,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );
	ASSERT_TRUE( _player->getAudio() != NULL );
	ASSERT_TRUE( _player->audioCount() == 1 );
	ASSERT_TRUE( _player->nextAudio() == 0 );
	ASSERT_TRUE( _player->getVideo() != NULL );
	ASSERT_TRUE( _player->videoCount() == 1 );
	ASSERT_TRUE( _player->nextVideo() == 0 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == 1000 );

	stopSrv(srv);
	ASSERT_TRUE( _starts.size() == 4);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _starts[2] == true);
	ASSERT_TRUE( _starts[3] == false);
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );
}

TEST_F( TvPlayerTest, play_order ) {
	tuner::ID srvID = 0xe764;
	_player->onStarted( boost::bind(&TvPlayerTest::onStarted,this,_1) );
	_player->playService( srvID );

	tuner::Service *srv=startService( srvID );
	stopSrv(srv);
	ASSERT_TRUE( _starts.size() == 2);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );

	srv=startService( srvID );
	ASSERT_TRUE( _starts.size() == 3);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _starts[2] == true);
	ASSERT_TRUE( _player->isStarted() == true );
	ASSERT_TRUE( _player->url() == "decode://audioPID=1002,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );
	ASSERT_TRUE( _player->getAudio() != NULL );
	ASSERT_TRUE( _player->audioCount() == 1 );
	ASSERT_TRUE( _player->nextAudio() == 0 );
	ASSERT_TRUE( _player->getVideo() != NULL );
	ASSERT_TRUE( _player->videoCount() == 1 );
	ASSERT_TRUE( _player->nextVideo() == 0 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == 1000 );

	stopSrv(srv);
	ASSERT_TRUE( _starts.size() == 4);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _starts[2] == true);
	ASSERT_TRUE( _starts[3] == false);
	ASSERT_TRUE( _player->isStarted() == false );
	ASSERT_TRUE( _player->url() == "" );
	ASSERT_TRUE( _player->getAudio() == NULL );
	ASSERT_TRUE( _player->audioCount() == 0 );
	ASSERT_TRUE( _player->nextAudio() == -1 );
	ASSERT_TRUE( _player->getVideo() == NULL );
	ASSERT_TRUE( _player->videoCount() == 0 );
	ASSERT_TRUE( _player->nextVideo() == -1 );
	ASSERT_TRUE( _player->getSubtitle() == NULL );
	ASSERT_TRUE( _player->subtitleCount() == 0 );
	ASSERT_TRUE( _player->nextSubtitle() == -1 );
	ASSERT_TRUE( _player->pcrPID() == TS_PID_NULL );

	_player->stopService();
	ASSERT_TRUE( _starts.size() == 4);
}

TEST_F( TvPlayerTest, play_without_service ) {
	tuner::ID srvID = 0xe764;
	_player->onStarted( boost::bind(&TvPlayerTest::onStarted,this,_1) );
	tuner::Service *srv=startService( srvID );
	_player->playService( srvID );
	ASSERT_TRUE( _starts.size() == 1);
	ASSERT_TRUE( _starts[0] == true);
	stopSrv( srv );
}

TEST_F( TvPlayerTest, next_audio ) {
	tuner::ID srvID = 0xe764;
	_player->onStarted( boost::bind(&TvPlayerTest::onStarted,this,_1) );
	_player->playService( srvID );

	tuner::Service *srv;
	{
		std::vector<tuner::ElementaryInfo> elems;
		test::addStream( elems, 0x1b, 1001, 0x81 );
		test::addStream( elems, 0x11, 1002, 0x90 );
		test::addStream( elems, 0x11, 1003, 0x91 );
		srv = startService( srvID, elems );
	}

	ASSERT_EQ( _player->url(), "decode://audioPID=1002,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );
	ASSERT_TRUE( _starts.size() == 1);
	ASSERT_TRUE( _starts[0] == true);

	_player->nextAudio();
	ASSERT_TRUE( _starts.size() == 3);
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _starts[1] == false);
	ASSERT_TRUE( _starts[2] == true);
	ASSERT_EQ( _player->url(), "decode://audioPID=1003,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );

	_player->stopService();
	ASSERT_TRUE( _starts.size() == 4);
	stopSrv(srv);
}

TEST_F( TvPlayerTest, findStream ) {
	tuner::ID srvID = 0xe764;
	_player->playService( srvID );
	startService( srvID );

	{
		const tuner::player::Stream *stream = _player->findStream( findVideoStream );
		ASSERT_TRUE( stream != NULL );
		ASSERT_TRUE( findVideoStream(stream) );
	}

	{
		const tuner::player::Stream *stream = _player->findStream( findAudioStream );
		ASSERT_TRUE( stream != NULL );
		ASSERT_TRUE( findAudioStream(stream) );
	}
}

class AvPlayerTest : public TvPlayerTest {
public:
	virtual tv::Delegate *createDelegate() const {
		return new tv::AvDelegate();
	}
};

TEST_F( AvPlayerTest, play ) {
	tuner::ID srvID = 0xe764;
	_player->onStarted( boost::bind(&TvPlayerTest::onStarted,this,_1) );
	_player->playService( srvID );
	startService( srvID );
	ASSERT_TRUE( _starts[0] == true);
	ASSERT_TRUE( _player->url() == "srvdtv://audioPID=1002,audioType=17,pcrPID=1000,videoPID=1001,videoType=27" );
}
