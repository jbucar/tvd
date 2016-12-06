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
#include <boost/math/special_functions/round.hpp>

static void mpCallback( int *count ) {
	(*count)++;
}

canvas::MediaPlayer *SinglePlayerTest::createMP() {
	canvas::MediaPlayer *tmp = NULL;
	createMpImpl(&tmp);
	return tmp;
}

void SinglePlayerTest::createMpImpl( canvas::MediaPlayer **mp ) {
	*mp = _player->create(_sys);
	ASSERT_TRUE( *mp != NULL );
	ASSERT_TRUE( realMP() != *mp );
	ASSERT_EQ( 1, nMP() );
}

canvas::MediaPlayer *SinglePlayer::_singleMP = NULL;
int SinglePlayer::_nMP = 0;
TestData SinglePlayer::_testData;

TEST_F( SinglePlayerTest, basic ) {
	ASSERT_TRUE( realMP() == NULL );
	ASSERT_EQ( 0, nMP() );
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, create_destroy ) {
	canvas::MediaPlayer *mp = createMP();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_EQ(0, enqueued());

	destroyMP(mp);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, create_destroy_2 ) {
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(0, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, destroy_playing ) {
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp1->play("test1.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(0, enqueued());

	mp2->play("test2.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(1, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, destroy_reverse_order ) {
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp1->play("test1.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(0, enqueued());

	mp2->play("test2.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(1, enqueued());

	destroyMP(mp2);
	destroyMP(mp1);
	ASSERT_EQ(0, enqueued());
}


TEST_F( SinglePlayerTest, play ) {
	int start = 0;
	int singleStart = 0;
	canvas::MediaPlayer *mp = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	mp->onStart().connect( boost::bind( mpCallback, &singleStart ) );

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(1, singleStart);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, play_check_url ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_STREQ("test.avi", realMP()->url().c_str());
	mp2->play("test2.avi");
	ASSERT_STREQ("test2.avi", realMP()->url().c_str());
	mp2->stop();
	ASSERT_STREQ("test.avi", realMP()->url().c_str());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, play_twice ) {
	int start = 0;
	int stop = 0;
	int singleStart = 0;
	int singleStop = 0;
	canvas::MediaPlayer *mp = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp->onStart().connect( boost::bind( mpCallback, &singleStart ) );
	mp->onStop().connect( boost::bind( mpCallback, &singleStop ) );

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(1, singleStart);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(0, singleStop);
	ASSERT_EQ(0, enqueued());

	mp->play("test2.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(2, singleStart);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStop);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, play_twice_2 ) {
	int start = 0;
	int stop = 0;
	int singleStart = 0;
	int singleStop = 0;
	int singleStart2 = 0;
	int singleStop2 = 0;
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp->onStart().connect( boost::bind( mpCallback, &singleStart ) );
	mp->onStop().connect( boost::bind( mpCallback, &singleStop ) );
	mp2->onStart().connect( boost::bind( mpCallback, &singleStart2 ) );
	mp2->onStop().connect( boost::bind( mpCallback, &singleStop2 ) );

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(1, singleStart);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(0, singleStop);
	ASSERT_EQ(0, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, enqueued());

	mp2->play("test2.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart);
	ASSERT_EQ(1, singleStop);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(1, enqueued());

	mp->play("test3.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_EQ(3, start);
	ASSERT_EQ(2, stop);
	ASSERT_EQ(2, singleStart);
	ASSERT_EQ(1, singleStop);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(1, singleStop2);
	ASSERT_EQ(1, enqueued());

	destroyMP(mp);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, play_2 ) {
	int start = 0;
	int stop = 0;
	int singleStart1 = 0;
	int singleStop1 = 0;
	int singleStart2 = 0;
	int singleStop2 = 0;
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp1->onStart().connect( boost::bind( mpCallback, &singleStart1 ) );
	mp1->onStop().connect( boost::bind( mpCallback, &singleStop1 ) );
	mp2->onStart().connect( boost::bind( mpCallback, &singleStart2 ) );
	mp2->onStop().connect( boost::bind( mpCallback, &singleStop2 ) );

	mp1->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(0, singleStop1);
	ASSERT_EQ(0, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, enqueued());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(1, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, stop ) {
	int start = 0;
	int stop = 0;
	int singleStart1 = 0;
	int singleStop1 = 0;
	canvas::MediaPlayer *mp1 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp1->onStart().connect( boost::bind( mpCallback, &singleStart1 ) );
	mp1->onStop().connect( boost::bind( mpCallback, &singleStop1 ) );

	mp1->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(0, singleStop1);
	ASSERT_EQ(0, enqueued());

	mp1->stop();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp1);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, stop_2 ) {
	int start = 0;
	int stop = 0;
	int singleStart1 = 0;
	int singleStop1 = 0;
	int singleStart2 = 0;
	int singleStop2 = 0;
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp1->onStart().connect( boost::bind( mpCallback, &singleStart1 ) );
	mp1->onStop().connect( boost::bind( mpCallback, &singleStop1 ) );
	mp2->onStart().connect( boost::bind( mpCallback, &singleStart2 ) );
	mp2->onStop().connect( boost::bind( mpCallback, &singleStop2 ) );

	mp1->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(0, singleStop1);
	ASSERT_EQ(0, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, enqueued());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(1, enqueued());

	mp2->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(3, start);
	ASSERT_EQ(2, stop);
	ASSERT_EQ(2, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(1, singleStop2);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, stop_not_current ) {
	int start = 0;
	int stop = 0;
	int singleStart1 = 0;
	int singleStop1 = 0;
	int singleStart2 = 0;
	int singleStop2 = 0;
	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp1->onStart().connect( boost::bind( mpCallback, &singleStart1 ) );
	mp1->onStop().connect( boost::bind( mpCallback, &singleStop1 ) );
	mp2->onStart().connect( boost::bind( mpCallback, &singleStart2 ) );
	mp2->onStop().connect( boost::bind( mpCallback, &singleStop2 ) );

	mp1->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(0, singleStop1);
	ASSERT_EQ(0, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, enqueued());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(1, enqueued());

	mp1->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, stop_3 ) {
	int start=0, singleStart1=0, singleStart2=0, singleStart3=0;
	int stop=0, singleStop1=0, singleStop2=0, singleStop3=0;

	canvas::MediaPlayer *mp1 = createMP();
	canvas::MediaPlayer *mp2 = createMP();
	canvas::MediaPlayer *mp3 = createMP();

	realMP()->onStart().connect( boost::bind( mpCallback, &start ) );
	realMP()->onStop().connect( boost::bind( mpCallback, &stop ) );
	mp1->onStart().connect( boost::bind( mpCallback, &singleStart1 ) );
	mp1->onStop().connect( boost::bind( mpCallback, &singleStop1 ) );
	mp2->onStart().connect( boost::bind( mpCallback, &singleStart2 ) );
	mp2->onStop().connect( boost::bind( mpCallback, &singleStop2 ) );
	mp3->onStart().connect( boost::bind( mpCallback, &singleStart3 ) );
	mp3->onStop().connect( boost::bind( mpCallback, &singleStop3 ) );

	mp1->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_EQ(1, start);
	ASSERT_EQ(0, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(0, singleStop1);
	ASSERT_EQ(0, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, singleStart3);
	ASSERT_EQ(0, singleStop3);
	ASSERT_EQ(0, enqueued());

	mp2->play("test2.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_EQ(2, start);
	ASSERT_EQ(1, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(0, singleStop2);
	ASSERT_EQ(0, singleStart3);
	ASSERT_EQ(0, singleStop3);
	ASSERT_EQ(1, enqueued());

	mp3->play("test3.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(mp3->isPlaying());
	ASSERT_EQ(3, start);
	ASSERT_EQ(2, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(1, singleStop2);
	ASSERT_EQ(1, singleStart3);
	ASSERT_EQ(0, singleStop3);
	ASSERT_EQ(2, enqueued());

	mp2->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(mp3->isPlaying());
	ASSERT_EQ(3, start);
	ASSERT_EQ(2, stop);
	ASSERT_EQ(1, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(1, singleStop2);
	ASSERT_EQ(1, singleStart3);
	ASSERT_EQ(0, singleStop3);
	ASSERT_EQ(1, enqueued());

	mp3->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp1->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(mp3->isPlaying());
	ASSERT_EQ(4, start);
	ASSERT_EQ(3, stop);
	ASSERT_EQ(2, singleStart1);
	ASSERT_EQ(1, singleStop1);
	ASSERT_EQ(1, singleStart2);
	ASSERT_EQ(1, singleStop2);
	ASSERT_EQ(1, singleStart3);
	ASSERT_EQ(1, singleStop3);
	ASSERT_EQ(0, enqueued());

	destroyMP(mp1);
	destroyMP(mp2);
	destroyMP(mp3);
	ASSERT_EQ(0, enqueued());
}

TEST_F( SinglePlayerTest, pause_unpause ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());

	mp->pause();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(realMP()->isPaused());
	ASSERT_TRUE(mp->isPaused());

	mp->unpause();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, pause_unpause_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->pause();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_TRUE(mp2->isPaused());

	mp2->unpause();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, pause_unpause_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp->pause();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp->unpause();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_paused_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp->pause();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isPaused());
	ASSERT_TRUE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->stop();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isPaused());
	ASSERT_TRUE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_paused_unpause_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp->pause();
	ASSERT_FALSE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isPaused());
	ASSERT_TRUE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp->unpause();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	mp2->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isPaused());
	ASSERT_FALSE(mp->isPaused());
	ASSERT_FALSE(mp2->isPaused());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, set_unset_loop ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());

	mp->loop(true);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_TRUE(realMP()->isLooping());
	ASSERT_TRUE(mp->isLooping());

	mp->loop(false);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, set_unset_loop_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->loop(true);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_TRUE(mp2->isLooping());

	mp2->loop(false);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, set_unset_loop_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp->loop(true);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp->loop(false);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_set_loop_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp->loop(true);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isLooping());
	ASSERT_TRUE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isLooping());
	ASSERT_TRUE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_set_unset_loop_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp->loop(true);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_TRUE(realMP()->isLooping());
	ASSERT_TRUE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->play("test.avi");
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp->loop(false);
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_FALSE(mp->isPlaying());
	ASSERT_TRUE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	mp2->stop();
	ASSERT_TRUE(realMP()->isPlaying());
	ASSERT_TRUE(mp->isPlaying());
	ASSERT_FALSE(mp2->isPlaying());
	ASSERT_FALSE(realMP()->isLooping());
	ASSERT_FALSE(mp->isLooping());
	ASSERT_FALSE(mp2->isLooping());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, streams_info ) {
	int parsed = 0;
	canvas::MediaPlayer *mp = createMP();
	mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	realMP()->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	mp->play("test.avi");
	static_cast<OkMediaPlayer*>(realMP())->signalMediaChanged(true);
	ASSERT_EQ(2, parsed);

	const canvas::StreamsInfo &streams = mp->streamsInfo();

	ASSERT_EQ(3, streams.videoCount());
	ASSERT_EQ(streams.currentVideo(), 0);

	ASSERT_EQ(2, streams.audioCount());
	ASSERT_EQ(streams.currentAudio(), 0);

	ASSERT_EQ(4, streams.subtitleCount());
	ASSERT_EQ(streams.currentSubtitle(), 0);

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, streams_info_stop ) {
	int parsed = 0;
	canvas::MediaPlayer *mp = createMP();
	mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	realMP()->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	mp->play("test.avi");
	static_cast<OkMediaPlayer*>(realMP())->signalMediaChanged(true);
	ASSERT_EQ(2, parsed);

	{
		const canvas::StreamsInfo &streams = mp->streamsInfo();

		ASSERT_EQ(3, streams.videoCount());
		ASSERT_EQ(streams.currentVideo(), 0);

		ASSERT_EQ(2, streams.audioCount());
		ASSERT_EQ(streams.currentAudio(), 0);

		ASSERT_EQ(4, streams.subtitleCount());
		ASSERT_EQ(streams.currentSubtitle(), 0);
	}

	mp->stop();
	{
		const canvas::StreamsInfo &streams = mp->streamsInfo();

		ASSERT_EQ(0, streams.videoCount());
		ASSERT_EQ(-1, streams.currentVideo());

		ASSERT_EQ(0, streams.audioCount());
		ASSERT_EQ(-1, streams.currentAudio());

		ASSERT_EQ(0, streams.subtitleCount());
		ASSERT_EQ(-1, streams.currentSubtitle());
	}

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, switch_video_stream ) {
	int parsed = 0;
	canvas::MediaPlayer *mp = createMP();
	
	mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(mp->switchVideoStream(2));

	mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(mp->switchVideoStream(2));

	static_cast<OkMediaPlayer*>(realMP())->signalMediaChanged( true );
	ASSERT_EQ(1, parsed);

	{
		const canvas::StreamsInfo &streams = mp->streamsInfo();
		ASSERT_EQ(0, streams.currentVideo());
		ASSERT_TRUE(mp->switchVideoStream(2));
		ASSERT_EQ(2, streams.currentVideo());
	}

	{
		const canvas::StreamsInfo &streams = realMP()->streamsInfo();
		ASSERT_EQ(2, streams.currentVideo());
	}

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, switch_audio_stream ) {
	int parsed = 0;
	canvas::MediaPlayer *mp = createMP();
	
	mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(mp->switchAudioStream(1));

	mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(mp->switchAudioStream(1));

	static_cast<OkMediaPlayer*>(realMP())->signalMediaChanged( true );
	ASSERT_EQ(1, parsed);

	{
		const canvas::StreamsInfo &streams = mp->streamsInfo();
		ASSERT_EQ(0, streams.currentAudio());
		ASSERT_TRUE(mp->switchAudioStream(1));
		ASSERT_EQ(1, streams.currentAudio());
	}

	{
		const canvas::StreamsInfo &streams = realMP()->streamsInfo();
		ASSERT_EQ(1, streams.currentAudio());
	}

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, switch_subtitle_stream ) {
	int parsed = 0;
	canvas::MediaPlayer *mp = createMP();
	
	mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );

	ASSERT_FALSE(mp->switchSubtitleStream(3));

	mp->play("test.avi");
	ASSERT_EQ(0, parsed);
	ASSERT_FALSE(mp->switchSubtitleStream(3));

	static_cast<OkMediaPlayer*>(realMP())->signalMediaChanged( true );
	ASSERT_EQ(1, parsed);

	{
		const canvas::StreamsInfo &streams = mp->streamsInfo();
		ASSERT_EQ(0, streams.currentSubtitle());
		ASSERT_TRUE(mp->switchSubtitleStream(3));
		ASSERT_EQ(3, streams.currentSubtitle());
	}

	{
		const canvas::StreamsInfo &streams = realMP()->streamsInfo();
		ASSERT_EQ(3, streams.currentSubtitle());
	}

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, mute_unmute ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());

	mp->mute( true );
	ASSERT_TRUE(realMP()->mute());
	ASSERT_TRUE(mp->mute());

	mp->mute( false );
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, mute_unmute_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->mute( true );
	ASSERT_TRUE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_TRUE(mp2->mute());

	mp2->mute( false );
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, mute_unmute_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp->mute( true );
	ASSERT_FALSE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp->mute( false );
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_muted_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp->mute(true);
	ASSERT_TRUE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->stop();
	ASSERT_TRUE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, enqueue_mute_unmute_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp->mute(true);
	ASSERT_TRUE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->play("test.avi");
	ASSERT_FALSE(realMP()->mute());
	ASSERT_TRUE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp->mute(false);
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	mp2->stop();
	ASSERT_FALSE(realMP()->mute());
	ASSERT_FALSE(mp->mute());
	ASSERT_FALSE(mp2->mute());

	destroyMP(mp);
	destroyMP(mp2);
}

#define DEFAULT_VOL boost::math::iround(float(MAX_VOLUME+MIN_VOLUME)/2.0f)
TEST_F( SinglePlayerTest, volume ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test.avi");

	ASSERT_EQ( DEFAULT_VOL, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );

	mp->volume( MAX_VOLUME );
	ASSERT_EQ( MAX_VOLUME, mp->volume() );
	ASSERT_EQ( MAX_VOLUME, realMP()->volume() );

	mp->volume( MIN_VOLUME );
	ASSERT_EQ( MIN_VOLUME, mp->volume() );
	ASSERT_EQ( MIN_VOLUME, realMP()->volume() );

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, volume_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test1.avi");
	mp2->play("test2.avi");

	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	mp2->volume( MAX_VOLUME );
	ASSERT_EQ( MAX_VOLUME, mp2->volume() );
	ASSERT_EQ( MAX_VOLUME, realMP()->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );

	mp2->volume( MIN_VOLUME );
	ASSERT_EQ( MIN_VOLUME, mp2->volume() );
	ASSERT_EQ( MIN_VOLUME, realMP()->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );

	mp2->stop();
	ASSERT_EQ( MIN_VOLUME, mp2->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, volume_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test1.avi");
	mp2->play("test2.avi");

	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	mp->volume( MAX_VOLUME );
	ASSERT_EQ( MAX_VOLUME, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );

	mp->volume( MIN_VOLUME );
	ASSERT_EQ( MIN_VOLUME, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );

	mp2->stop();
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );
	ASSERT_EQ( MIN_VOLUME, mp->volume() );
	ASSERT_EQ( MIN_VOLUME, realMP()->volume() );

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, enqueue_set_vol_dequeue ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test.avi");
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	mp->volume(MAX_VOLUME);
	ASSERT_EQ( MAX_VOLUME, realMP()->volume() );
	ASSERT_EQ( MAX_VOLUME, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	mp2->play("test.avi");
	ASSERT_EQ( DEFAULT_VOL, realMP()->volume() );
	ASSERT_EQ( MAX_VOLUME, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	mp2->stop();
	ASSERT_EQ( MAX_VOLUME, realMP()->volume() );
	ASSERT_EQ( MAX_VOLUME, mp->volume() );
	ASSERT_EQ( DEFAULT_VOL, mp2->volume() );

	destroyMP(mp);
	destroyMP(mp2);
}
#undef DEFAULT_VOL

TEST_F( SinglePlayerTest, move_resize ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test1.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp->move( canvas::Point(10,10) );
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,720,576) );

	mp->resize( canvas::Size(200,200) );
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,200,200) );

	mp->moveResize( canvas::Rect(5,5,100,100) );
	ASSERT_EQ( testData().bounds, canvas::Rect(5,5,100,100) );

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, move_resize_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test1.avi");
	mp2->play("test2.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp2->move( canvas::Point(10,10) );
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,720,576) );

	mp2->resize( canvas::Size(200,200) );
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,200,200) );

	mp2->moveResize( canvas::Rect(5,5,100,100) );
	ASSERT_EQ( testData().bounds, canvas::Rect(5,5,100,100) );

	mp2->stop();
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, move_resize_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->play("test1.avi");
	mp2->play("test2.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp->move( canvas::Point(10,10) );
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp->resize( canvas::Size(200,200) );
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp->moveResize( canvas::Rect(5,5,100,100) );
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp2->stop();
	ASSERT_EQ( testData().bounds, canvas::Rect(5,5,100,100) );

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, fullscreen ) {
	canvas::MediaPlayer *mp = createMP();

	mp->moveResize( canvas::Rect(10,10,100,100) );
	mp->play("test1.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,100,100) );

	mp->setFullScreen();
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	destroyMP(mp);
}

TEST_F( SinglePlayerTest, fullscreen_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->moveResize( canvas::Rect(10,10,100,100) );
	mp2->moveResize( canvas::Rect(20,20,150,150) );
	mp->play("test1.avi");
	mp2->play("test1.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(20,20,150,150) );

	mp2->setFullScreen();
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	mp2->stop();
	ASSERT_EQ( testData().bounds, canvas::Rect(10,10,100,100) );

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, fullscreen_not_current ) {
	canvas::MediaPlayer *mp = createMP();
	canvas::MediaPlayer *mp2 = createMP();

	mp->moveResize( canvas::Rect(10,10,100,100) );
	mp2->moveResize( canvas::Rect(20,20,150,150) );
	mp->play("test1.avi");
	mp2->play("test1.avi");
	ASSERT_EQ( testData().bounds, canvas::Rect(20,20,150,150) );

	mp->setFullScreen();
	ASSERT_EQ( testData().bounds, canvas::Rect(20,20,150,150) );

	mp2->stop();
	ASSERT_EQ( testData().bounds, canvas::Rect(0,0,720,576) );

	destroyMP(mp);
	destroyMP(mp2);
}

TEST_F( SinglePlayerTest, changeMode ) {
	canvas::MediaPlayer *mp = createMP();

	mp->play("test1.avi");
	ASSERT_TRUE( _sys->screen()->mode( _sys->screen()->connector(), canvas::mode::m1080i_60 ) );
	ASSERT_EQ( testData().nMoveResize, 2 );

	destroyMP(mp);
}