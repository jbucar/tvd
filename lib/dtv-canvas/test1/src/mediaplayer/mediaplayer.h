/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#pragma once

#include "../screen.h"
#include "../../../src/audio.h"
#include "../../../src/player.h"
#include "../../../src/impl/dummy/player.h"
#include "../../../src/impl/dummy/mediaplayer.h"
#include "../../../src/point.h"
#include "../../../src/mediaplayer.h"
#include "../../../src/streaminfo.h"
#include "../../../src/factory.h"
#include "../../../src/system.h"
#include <util/mcr.h>
#include <util/log.h>
#include <boost/thread.hpp>
#include <gtest/gtest.h>

class CustomFactory : public canvas::Factory {
public:
	virtual canvas::Screen *createScreen() const {
		return new ScreenOk();
	}
	virtual canvas::Audio *createAudio() const {
		return canvas::Audio::create("dummy");
	}
};

//	Players
class BasicPlayer : public canvas::Player {
public:
	BasicPlayer() : _init(0), _fin(0) {}
	virtual ~BasicPlayer() {}

	int _init, _fin;

protected:
	virtual bool init() { _init++; return true; }
	virtual void fin() { _fin++; }

	virtual canvas::MediaPlayer *createMediaPlayer( canvas::System *sys ) const {
		return new canvas::dummy::MediaPlayer( sys );
	}
};

class PlayerInitFail : public BasicPlayer {
protected:
	virtual bool init() { return false; }
};

class PlayerCreateFail : public BasicPlayer {
protected:
	virtual canvas::MediaPlayer *createMediaPlayer( canvas::System * /*sys*/ ) const { return NULL; }
};


class PlayerTest : public testing::Test {
public:
	PlayerTest() {
		canvas::Factory *factory = new CustomFactory();
		_sys = factory->createSystem();
		_sys->initialize(factory);
	}
	virtual ~PlayerTest() {
		_sys->finalize();
		delete _sys;
	}

protected:
	virtual void SetUp() {
		_player = createPlayer();
		ASSERT_TRUE(_player ? true : false);
	}

	virtual void TearDown() {
		DEL(_player);
		util::log::flush();
	}

	virtual BasicPlayer *createPlayer() {
		return new BasicPlayer();
	}

	BasicPlayer *_player;
	canvas::System *_sys;
};

class PlayerTestInitFail : public PlayerTest {
protected:
	virtual BasicPlayer *createPlayer() { return new PlayerInitFail(); }
};

class PlayerTestCreateFail : public PlayerTest {
protected:
	virtual BasicPlayer *createPlayer() { return new PlayerCreateFail(); }
};

struct TestData {
	int nPause;
	int nMuted;
	int nVol;
	int nRefresh;
	int nMoveResize;
	int videoSwitched;
	int audioSwitched;
	int subtitleSwitched;
	canvas::Rect bounds;
};

class OkStreamsInfo : public canvas::StreamsInfo {
public:
	explicit OkStreamsInfo( TestData &data );
	virtual ~OkStreamsInfo();

	void doParse( bool parse );


protected:
	virtual void parseImpl();
	virtual void refreshImpl();

private:
	bool _doParse;
	TestData &_testData;
};

class OkMediaPlayer : public canvas::MediaPlayer {
public:
	explicit OkMediaPlayer( canvas::System *sys, TestData &data ) : canvas::MediaPlayer(sys), _testData(data) {}
	virtual ~OkMediaPlayer() {}

	// Simulate media events
	void signalCompleted();
	void signalMediaChanged( bool addStreams=false );
	void signalError( const std::string &err );

	TestData &_testData;

protected:
	virtual bool startImpl( const std::string &url );
	virtual void stopImpl();
	virtual void pauseImpl( bool needPause );
	virtual void muteImpl( bool needsMute );
	virtual void volumeImpl( canvas::Volume vol );

	virtual void moveResizeImpl( const canvas::Rect &rect );

	virtual canvas::StreamsInfo *createStreamsInfo() {
		return new OkStreamsInfo(_testData);
	}
	virtual bool switchVideoStreamImpl( int id );
	virtual bool switchAudioStreamImpl( int id );
	virtual bool switchSubtitleStreamImpl( int id );
};

class MPFailPlay : public OkMediaPlayer {
public:
	explicit MPFailPlay( canvas::System *sys, TestData &data ) : OkMediaPlayer(sys, data) {}

protected:
	virtual bool startImpl( const std::string &/*url*/ ) { return false; }
};

class MediaPlayerTest : public testing::Test {
public:
	MediaPlayerTest() {
		_testData.nPause = 0;
		_testData.nMuted = 0;
		_testData.nVol = 0;
		_testData.nRefresh = 0;
		_testData.nMoveResize = 0;
		_testData.videoSwitched = 0;
		_testData.audioSwitched = 0;
		_testData.subtitleSwitched = 0;
		_testData.bounds = canvas::Rect(0,0,720,576);
		canvas::Factory *factory = new CustomFactory();
		_sys = factory->createSystem();
		_sys->initialize(factory);
	}
	virtual ~MediaPlayerTest() {
		_sys->finalize();
		delete _sys;
	}

	TestData _testData;

protected:
	virtual void SetUp() {
		_mp = createMediaPlayer();
		_mp->mainLoopThreadId(boost::this_thread::get_id());
		_mp->moveResize( _testData.bounds );
		ASSERT_TRUE( _mp != NULL );
	}

	virtual void TearDown() {
		DEL(_mp);
		util::log::flush();
	}

	virtual canvas::MediaPlayer *createMediaPlayer() {
		return new OkMediaPlayer( _sys, _testData );
	}

	canvas::MediaPlayer *_mp;
	canvas::System *_sys;
};

class MediaPlayerNoDestroyTest : public MediaPlayerTest {
	virtual void TearDown() {
		util::log::flush();
	}
};

class MediaPlayerTestFailPlay : public MediaPlayerTest {
	virtual canvas::MediaPlayer *createMediaPlayer() { return new MPFailPlay(_sys, _testData); }
};

class SinglePlayer : public BasicPlayer {
public:
	SinglePlayer() {
		_nMP=0;
		_testData.nPause = 0;
		_testData.nMuted = 0;
		_testData.nVol = 0;
		_testData.nRefresh = 0;
		_testData.nMoveResize = 0;
		_testData.videoSwitched = 0;
		_testData.audioSwitched = 0;
		_testData.subtitleSwitched = 0;
		_testData.bounds = canvas::Rect(0,0,720,576);
	}
	virtual ~SinglePlayer() {}
	
	virtual bool supportMultiplePlayers() const { return false; }
	size_t enqueued() const { return enqueuedCount(); }

	static int _nMP;
	static canvas::MediaPlayer *_singleMP;
	static TestData _testData;


protected:
	virtual canvas::MediaPlayer *createMediaPlayer( canvas::System *sys ) const {
		SinglePlayer::_nMP++;
		SinglePlayer::_singleMP = new OkMediaPlayer(sys, _testData);
		return SinglePlayer::_singleMP;
	}
};


class SinglePlayerTest : public PlayerTest {
public:
	SinglePlayerTest() {}
	virtual ~SinglePlayerTest() {};

protected:
	virtual void SetUp() {
		PlayerTest::SetUp();
		_player->initialize();
	}

	virtual void TearDown() {
		_player->finalize();
		PlayerTest::TearDown();
	}

	virtual SinglePlayer *createPlayer() { return new SinglePlayer(); }
	canvas::MediaPlayer *createMP();
	void createMpImpl( canvas::MediaPlayer **mp );

	void destroyMP( canvas::MediaPlayerPtr &mp ) {
		_player->destroy(mp);
	}

	canvas::MediaPlayer *realMP() { return SinglePlayer::_singleMP; }
	int nMP() { return SinglePlayer::_nMP; }
	int enqueued() { return static_cast<SinglePlayer*>(_player)->enqueued(); }
	const TestData &testData() { return SinglePlayer::_testData; }
};
