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

static void mpCallback2( int *count ) {
	(*count)++;
}

TEST_F( MediaPlayerTest, signal_start ) {
	int start = 0;
	_mp->onStart().connect( boost::bind( mpCallback, &start ) );

	ASSERT_EQ(0, start);
	_mp->play("test.avi");
	ASSERT_EQ(1, start);
}

TEST_F( MediaPlayerTest, signal_start_double ) {
	int start = 0;
	_mp->onStart().connect( boost::bind( mpCallback, &start ) );
	_mp->onStart().connect( boost::bind( mpCallback2, &start ) );

	ASSERT_EQ(0, start);
	_mp->play("test.avi");
	ASSERT_EQ(2, start);
}

TEST_F( MediaPlayerTest, signal_start_stop ) {
	int start = 0;
	_mp->onStart().connect( boost::bind( mpCallback, &start ) );

	ASSERT_EQ(0, start);
	_mp->play("test.avi");
	_mp->stop();
	ASSERT_EQ(1, start);
}

TEST_F( MediaPlayerTest, signal_stop ) {
	int stop = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, stop);
	_mp->stop();
	ASSERT_EQ(1, stop);
}

TEST_F( MediaPlayerTest, signal_stop_double ) {
	int stop = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );
	_mp->onStop().connect( boost::bind( mpCallback2, &stop ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, stop);
	_mp->stop();
	ASSERT_EQ(2, stop);
}

TEST_F( MediaPlayerTest, signal_stop_invalid ) {
	int stop = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );

	ASSERT_EQ(0, stop);
	_mp->stop();
	ASSERT_EQ(0, stop);
}

TEST_F( MediaPlayerTest, signal_stop_invalid2 ) {
	int stop = 0;
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, stop);
	_mp->pause();
	ASSERT_EQ(0, stop);
}

TEST_F( MediaPlayerTest, signal_error ) {
	int err = 0;
	_mp->onError().connect( boost::bind( mpCallback, &err ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, err);
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error");
	ASSERT_EQ(1, err);
}

TEST_F( MediaPlayerTest, signal_error_twice ) {
	int err = 0;
	_mp->onError().connect( boost::bind( mpCallback, &err ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, err);
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error");
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error2");
	ASSERT_EQ(2, err);
}

TEST_F( MediaPlayerTest, signal_error_double_twice ) {
	int err = 0;
	_mp->onError().connect( boost::bind( mpCallback, &err ) );
	_mp->onError().connect( boost::bind( mpCallback2, &err ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, err);
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error");
	ASSERT_EQ(2, err);
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error2");
	ASSERT_EQ(4, err);
}

TEST_F( MediaPlayerTest, signal_error_stop ) {
	int err = 0;
	int stop = 0;
	_mp->onError().connect( boost::bind( mpCallback, &err ) );
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, err);
	static_cast<OkMediaPlayer*>(_mp)->signalError("Testing error");
	ASSERT_EQ(1, err);
	ASSERT_EQ(1, stop);
}

TEST_F( MediaPlayerTest, signal_all ) {
	int start = 0;
	int stop = 0;
	_mp->onStart().connect( boost::bind( mpCallback, &start ) );
	_mp->onStop().connect( boost::bind( mpCallback, &stop ) );

	_mp->play("test.avi");
	ASSERT_EQ(1, start);
	
	_mp->pause();
	_mp->unpause();

	_mp->stop();
	ASSERT_EQ(1, stop);
}

TEST_F( MediaPlayerTest, signal_media_changed ) {
	int mediaChanged = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &mediaChanged ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, mediaChanged);

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged();
	ASSERT_EQ(1, mediaChanged);

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged();
	ASSERT_EQ(2, mediaChanged);
}

TEST_F( MediaPlayerTest, signal_media_stop_start ) {
	int mediaChanged = 0;
	int parsed = 0;
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &parsed ) );
	_mp->onMediaChanged().connect( boost::bind( mpCallback, &mediaChanged ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, mediaChanged);

	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged( true );
	ASSERT_EQ(1, mediaChanged);
	ASSERT_EQ(1, parsed);
	_mp->stop();
	const canvas::StreamsInfo &streams = _mp->streamsInfo();
	ASSERT_EQ(-1, streams.currentVideo());
	ASSERT_EQ(0, streams.videoCount());
	
	_mp->play("test.avi");
	const canvas::StreamsInfo &streams2 = _mp->streamsInfo();
	ASSERT_EQ(-1, streams2.currentVideo());
	ASSERT_EQ(0, streams2.videoCount());
}
