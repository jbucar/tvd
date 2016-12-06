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

#include "mediaplayer.h"

static void mpCallback( int *count ) {
	(*count)++;
}

TEST_F( PlayerTest, basic ) {
}

TEST_F( PlayerTest, init_ok ) {
	ASSERT_TRUE( _player->initialize() );
	_player->finalize();
	ASSERT_EQ( _player->_init, 1 );
	ASSERT_EQ( _player->_fin, 1 );
}

TEST_F( PlayerTest, init_ok_init ) {
	ASSERT_TRUE( _player->initialize() );
	ASSERT_DEATH( _player->initialize(), "" );
	_player->finalize();
	ASSERT_EQ( _player->_init, 1 );
	ASSERT_EQ( _player->_fin, 1 );
}

TEST_F( PlayerTestInitFail, init_fail ) {
	ASSERT_FALSE( _player->initialize() );
	_player->finalize();
	ASSERT_EQ( _player->_init, 0 );
	ASSERT_EQ( _player->_fin, 0 );
}

TEST_F( PlayerTest, create_ok ) {
	ASSERT_TRUE( _player->initialize() );
	canvas::MediaPlayer *mp = _player->create( _sys );
	ASSERT_TRUE( mp ? true : false );
	_player->destroy( mp );
	_player->finalize();
}

TEST_F( PlayerTest, create_without_init_ok ) {
	ASSERT_DEATH( _player->create( _sys ), "" );
}

TEST_F( PlayerTest, create_with_sys_nil ) {
	ASSERT_TRUE( _player->initialize() );
	ASSERT_DEATH( _player->create( NULL ), "" );
	_player->finalize();
}

TEST_F( PlayerTestCreateFail, create_fail ) {
	ASSERT_TRUE( _player->initialize() );
	ASSERT_DEATH( _player->create( _sys ), "" );
	_player->finalize();
}

TEST_F( MediaPlayerTest, basic ) {
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "");
	ASSERT_EQ( _testData.nPause, 0 );
}

TEST_F( MediaPlayerTest, play_empty_url ) {
	ASSERT_FALSE(_mp->play(""));
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
}

TEST_F( MediaPlayerTest, play_url_ok ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "test.avi");
}

TEST_F( MediaPlayerTest, play_twice ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "test.avi");
	ASSERT_EQ( count, 0 );
	ASSERT_TRUE(_mp->play("test2.avi"));
	ASSERT_EQ( count, 1 );
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "test2.avi");
}

TEST_F( MediaPlayerTestFailPlay, play_empty_url ) {
	ASSERT_FALSE(_mp->play("test.avi"));
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
}

TEST_F( MediaPlayerTest, pause ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 1 );
}

TEST_F( MediaPlayerTest, unpause ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 1 );
	_mp->unpause();
	ASSERT_EQ( _testData.nPause, 0 );
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
}

TEST_F( MediaPlayerTest, pause_not_playing ) {
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
}

TEST_F( MediaPlayerTest, pause_twice ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 1 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 1 );
}

TEST_F( MediaPlayerTest, unpause_not_playing ) {
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->unpause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
}

TEST_F( MediaPlayerTest, unpause_playing ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->unpause();
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
}

TEST_F( MediaPlayerTest, unpause_twice ) {
	ASSERT_TRUE(_mp->play("test.avi"));
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 0 );
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ( _testData.nPause, 1 );
	_mp->unpause();
	ASSERT_EQ( _testData.nPause, 0 );
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->unpause();
	ASSERT_EQ( _testData.nPause, 0 );
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
}

TEST_F( MediaPlayerTest, stop_not_playing ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->stop();
	ASSERT_EQ(count, 0);
	ASSERT_EQ(_testData.nPause, 0);
}

TEST_F( MediaPlayerTest, stop_playing ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 0);
}

TEST_F( MediaPlayerTest, stop_loop ) {
	int stop = 0;
	int start = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );
	_mp->onStart().connect( boost::bind( mpCallback, &start ) );
	ASSERT_FALSE(_mp->isLooping());
	_mp->loop(true);
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isLooping());
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->stop();
	ASSERT_FALSE(_mp->isLooping());
	ASSERT_EQ(stop, 1);
	ASSERT_EQ(start, 1);
	ASSERT_EQ(_testData.nPause, 0);
}

TEST_F( MediaPlayerTest, stop_twice ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 0);
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 0);
}

TEST_F( MediaPlayerTest, stop_paused ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(_testData.nPause, 0);
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ(count, 0);
	ASSERT_EQ(_testData.nPause, 1);
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 1);
}

TEST_F( MediaPlayerTest, stop_unpaused ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ(_testData.nPause, 1);
	_mp->unpause();
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(count, 0);
	ASSERT_EQ(_testData.nPause, 0);
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 0);
}

TEST_F( MediaPlayerTest, unpause_stop ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->pause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isPaused());
	ASSERT_EQ(_testData.nPause, 1);
	_mp->stop();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(count, 1);
	_mp->unpause();
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(count, 1);
	ASSERT_EQ(_testData.nPause, 1);
}

TEST_F( MediaPlayerTest, stop_play_stop ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->stop();
	ASSERT_EQ(count, 1);
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->play("test.avi");
	ASSERT_EQ(count, 1);
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	_mp->stop();
	ASSERT_EQ(count, 2);
	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
}

TEST_F( MediaPlayerTest, complete ) {
	int count = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &count ) );
	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(count, 0);

	// Simulate media eof!
	static_cast<OkMediaPlayer*>(_mp)->signalCompleted();

	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(count, 1);
}

TEST_F( MediaPlayerTest, set_unset_loop ) {

	_mp->play("test.avi");
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isLooping());

	_mp->loop(true);
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_TRUE(_mp->isLooping());

	_mp->loop(false);
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isLooping());
}

TEST_F( MediaPlayerTest, complete_looping ) {
	int completed = 0;
	int restarted = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &completed ) );
	_mp->onStart().connect( boost::bind( mpCallback, &restarted ) );
	_mp->play("test.avi");
	_mp->loop(true);
	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(completed, 0);
	ASSERT_EQ(restarted, 1);

	static_cast<OkMediaPlayer*>(_mp)->signalCompleted();

	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "test.avi");
	ASSERT_EQ(completed, 1);
	ASSERT_EQ(restarted, 2);

	static_cast<OkMediaPlayer*>(_mp)->signalCompleted();

	ASSERT_TRUE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_STREQ(_mp->url().c_str(), "test.avi");
	ASSERT_EQ(completed, 2);
	ASSERT_EQ(restarted, 3);

	_mp->loop(false);
	static_cast<OkMediaPlayer*>(_mp)->signalCompleted();

	ASSERT_FALSE(_mp->isPlaying());
	ASSERT_FALSE(_mp->isPaused());
	ASSERT_EQ(completed, 3);
	ASSERT_EQ(restarted, 3);
}