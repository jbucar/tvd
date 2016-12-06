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
#include "../../../../../src/service/extension/player/streaminfo.h"
#include "../../../../../src/service/extension/player/player.h"
#include "../../../../../src/service/extension/player/delegate.h"
#include "../../../../../src/service/extension/player/tsdelegate.h"
#include "../../../../../src/service/extension/player/esplayer.h"
#include "../../../../../src/muxer/ts/out/rtp.h"
#include <util/mcr.h>
#include <gtest/gtest.h>

namespace tv = tuner::player;

class EsMyDelegate : public tv::Delegate {
public:
	//	Streams
	virtual bool startStream( tuner::ID /*pid*/, tuner::pes::type::type /*esType*/ ) { return true; }
	virtual void stopStream( tuner::ID /*pid*/ ) {}
};

class InitFailDelegate : public tv::Delegate {
public:
	//	Streams
	virtual bool startStream( tuner::ID /*pid*/, tuner::pes::type::type /*esType*/ ) { return false; }
	virtual void stopStream( tuner::ID /*pid*/ ) {}

protected:
	virtual bool init() { return false; }
};

class StartFailDelegate : public tv::Delegate {
public:
	//	Streams
	virtual bool startStream( tuner::ID /*pid*/, tuner::pes::type::type /*esType*/ ) { return false; }
	virtual void stopStream( tuner::ID /*pid*/ ) {}
};

class MyTsDelegate : public tv::ts::Delegate {
public:
	MyTsDelegate( int streamID, tuner::muxer::ts::Output *out ) : tv::ts::Delegate(streamID,out) {}
	//	Streams
	virtual bool startStream( tuner::ID /*pid*/, tuner::pes::type::type /*esType*/ ) {
		return true;
	}
	virtual void stopStream( tuner::ID /*pid*/ ) {}
};

class MyTsDelegateFail : public tv::ts::Delegate {
public:
	MyTsDelegateFail( int streamID, tuner::muxer::ts::Output *out ) : tv::ts::Delegate(streamID,out) {}
	//	Streams
	virtual bool startStream( tuner::ID /*pid*/, tuner::pes::type::type /*esType*/ ) {
		return false;
	}
	virtual void stopStream( tuner::ID /*pid*/ ) {}
};

class EsCountDelegate : public tv::Delegate {
public:
	//	Streams
	virtual bool startStream( tuner::ID pid, tuner::pes::type::type /*esType*/ ) {
		_starts.push_back( pid );
		return true;
	}
	virtual void stopStream( tuner::ID pid ) {
		_stopped.push_back( pid );
	}

	std::vector<tuner::ID> _starts;
	std::vector<tuner::ID> _stopped;
};

class EsPlayerATest : public testing::Test {
public:
	EsPlayerATest() {
		_tvPlayer = NULL;
		_player = NULL;
		_sInfo = NULL;
	}

	virtual void SetUp() {
		_tvPlayer = new tv::Player(new EsMyDelegate());
		ASSERT_TRUE(_tvPlayer->initialize());
	}

	virtual void TearDown() {
		delete _player;

		_tvPlayer->finalize();
		delete _tvPlayer;

		delete _sInfo;
	}

	virtual void initPlayer( tv::Delegate *dlg=NULL, tuner::pes::type::type esType=tuner::pes::type::audio ) {
		createPlayer( dlg, esType );
		ASSERT_TRUE( _player->initialize(_tvPlayer) );
	}

	virtual void createPlayer( tv::Delegate *dlg=NULL, tuner::pes::type::type esType=tuner::pes::type::audio ) {
		if (!dlg) {
			_player = new tv::es::Player( esType );
		}
		else {
			_player = new tv::es::Player( dlg, esType );
		}
	}

	virtual void finPlayer() {
		_player->finalize();
		delete _player; _player = NULL;
	}

	virtual void addStream() {
		//	Add stream
		tuner::ElementaryInfo eInfo;
		eInfo.streamType = 0x11;
		eInfo.pid = 0x100;
		_sInfo = new tuner::player::StreamInfo("AAC",tuner::pes::type::audio,0x11);
		_player->addStream( _sInfo, eInfo, 0xFF );
	}

	tuner::player::StreamInfo *_sInfo;
	tv::Player *_tvPlayer;
	tv::es::Player *_player;
};

TEST_F( EsPlayerATest, constructor_video ) {
	initPlayer( new EsMyDelegate(), tuner::pes::type::video );
	finPlayer();
}

TEST_F( EsPlayerATest, constructor_audio ) {
	initPlayer( new EsMyDelegate(), tuner::pes::type::audio );
	finPlayer();
}

TEST_F( EsPlayerATest, constructor_subtitle ) {
	initPlayer( new EsMyDelegate(), tuner::pes::type::subtitle );
	finPlayer();
}

TEST_F( EsPlayerATest, basic ) {
	EsCountDelegate *dlg = new EsCountDelegate();
	initPlayer( dlg );
	addStream();

	_player->start();
	ASSERT_TRUE( dlg->_starts.size() == 1 );
	ASSERT_TRUE( dlg->_starts[0] == 0x100 );
	ASSERT_TRUE( dlg->_stopped.size() == 0 );
	_player->stop(true);
	ASSERT_TRUE( dlg->_starts.size() == 1 );
	ASSERT_TRUE( dlg->_starts[0] == 0x100 );
	ASSERT_TRUE( dlg->_stopped.size() == 1 );
	ASSERT_TRUE( dlg->_stopped[0] == 0x100 );

	finPlayer();
}

TEST_F( EsPlayerATest, forward_delegate ) {
	initPlayer();
	addStream();
	_player->start();
	_player->stop(true);
	finPlayer();
}

TEST_F( EsPlayerATest, ts_delegate ) {
	int streamID = 0;
	tuner::muxer::ts::Output *out = new tuner::muxer::ts::RTPOutput( "127.0.0.1", 5000+streamID );
	tuner::player::Delegate *dlg = new MyTsDelegate( streamID, out );
	initPlayer( dlg );
	addStream();
	_player->start();
	_player->stop(true);
	finPlayer();
}

TEST_F( EsPlayerATest, ts_delegate_fail ) {
	int streamID = 0;
	tuner::muxer::ts::Output *out = new tuner::muxer::ts::RTPOutput( "127.0.0.1", 5000+streamID );
	tuner::player::Delegate *dlg = new MyTsDelegateFail( streamID, out );
	initPlayer( dlg );
	addStream();
	_player->start();
	_player->stop(true);
	finPlayer();
}

TEST_F( EsPlayerATest, init_dlg_fail ) {
	createPlayer( new InitFailDelegate() );
	addStream();
	ASSERT_FALSE( _player->initialize( _tvPlayer ) );
}

TEST_F( EsPlayerATest, init_dlg_fail_and_finalize ) {
	createPlayer( new InitFailDelegate() );
	addStream();
	ASSERT_FALSE( _player->initialize( _tvPlayer ) );
	_player->finalize();
}

TEST_F( EsPlayerATest, init_with_empty_streams ) {
	createPlayer( new EsMyDelegate() );
	ASSERT_TRUE( _player->initialize( _tvPlayer ) );
	_player->start();
	_player->stop(true);
	_player->finalize();
}

TEST_F( EsPlayerATest, start_stream_fail ) {
	initPlayer( new StartFailDelegate() );
	_player->start();
	_player->stop(true);
	_player->finalize();
}


class EsPlayerTest : public EsPlayerATest {
public:
	EsPlayerTest() {
		_sp = NULL;
	}

	virtual void SetUp() {
		EsPlayerATest::SetUp();
		_sp = new EsCountDelegate();
		initPlayer( _sp );
	}

	virtual void TearDown() {
		finPlayer();
		EsPlayerATest::TearDown();
	}

	EsCountDelegate *_sp;
};

TEST_F( EsPlayerTest, base ) {}

TEST_F( EsPlayerTest, add_empty_info ) {
	//	Check before add
	ASSERT_TRUE( _player->count() == 0 );
	const tuner::player::Stream *get = _player->get( 0 );
	ASSERT_TRUE( get == NULL );

	addStream();

	//	Check after
	ASSERT_TRUE( _player->count() == 1 );
	{
		const tuner::player::Stream *get = _player->get( 0 );
		ASSERT_TRUE( get != NULL );
		ASSERT_TRUE( get->info.pid == 0x100 );
	}
	{
		const tuner::player::Stream *get = _player->get( 2 );
		ASSERT_TRUE( get == NULL );
	}
}

TEST_F( EsPlayerTest, start_must_call_delegate ) {
	addStream();

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );
}

TEST_F( EsPlayerTest, next_same_stream ) {
	addStream();

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->next();
	ASSERT_TRUE( _sp->_stopped.size() == 0 );
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );
}

TEST_F( EsPlayerTest, next_more_streams ) {
	tuner::ElementaryInfo eInfo1;
	eInfo1.streamType = 0x11;
	eInfo1.pid = 0x100;
	tuner::player::StreamInfo sInfo1("AAC",tuner::pes::type::audio,0x11);
	_player->addStream( &sInfo1, eInfo1, 0xFF );

	tuner::ElementaryInfo eInfo2;
	eInfo2.streamType = 0x12;
	eInfo2.pid = 0x101;
	tuner::player::StreamInfo sInfo2("AAC",tuner::pes::type::audio,0x12);
	_player->addStream( &sInfo2, eInfo2, 0xFF );

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->next();
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );
	ASSERT_TRUE( _sp->_starts.size() == 2 );
	ASSERT_TRUE( _sp->_starts[1] == 0x101 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 2 );
	ASSERT_TRUE( _sp->_stopped[1] == 0x101 );
}

TEST_F( EsPlayerTest, must_sort_streams_by_tag ) {
	tuner::ElementaryInfo eInfo1;
	eInfo1.streamType = 0x11;
	eInfo1.pid = 0x100;
	tv::StreamInfo sInfo1("AAC",tuner::pes::type::audio,0x11);
	tv::Stream s;
	s.info = eInfo1;
	s.sInfo = &sInfo1;
	s.tag = 10;
	_player->addStream( s );

	tuner::ElementaryInfo eInfo2;
	eInfo2.streamType = 0x12;
	eInfo2.pid = 0x101;
	tv::StreamInfo sInfo2("AAC",tuner::pes::type::audio,0x11);
	tv::Stream s2;
	s2.info = eInfo2;
	s2.sInfo = &sInfo2;
	s2.tag = 9;
	_player->addStream( s2 );

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x101 );

	_player->next();
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x101 );
	ASSERT_TRUE( _sp->_starts.size() == 2 );
	ASSERT_TRUE( _sp->_starts[1] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 2 );
	ASSERT_TRUE( _sp->_stopped[1] == 0x100 );
}

TEST_F( EsPlayerTest, start_twitch ) {
	addStream();

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	ASSERT_DEATH( _player->start(), "" );
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );
}

TEST_F( EsPlayerTest, stop_twitch ) {
	addStream();

	_player->start();
	ASSERT_TRUE( _sp->_starts.size() == 1 );
	ASSERT_TRUE( _sp->_starts[0] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );

	_player->stop(false);
	ASSERT_TRUE( _sp->_stopped.size() == 1 );
	ASSERT_TRUE( _sp->_stopped[0] == 0x100 );
}
