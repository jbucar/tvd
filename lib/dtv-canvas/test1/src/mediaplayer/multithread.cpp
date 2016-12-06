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
#include <boost/thread.hpp>

static void mpCallback( int *count ) {
	(*count)++;
}

static void mpCallbackError( int *count, const std::string & /*msg*/ ) {
	(*count)++;
}

static void signalMediaChanged( OkMediaPlayer *mp, boost::condition_variable *cond ) {
	ASSERT_DEATH( mp->signalMediaChanged(), "" );
	cond->notify_one();
}

static void signalCompleted( OkMediaPlayer *mp, boost::condition_variable *cond ) {
	ASSERT_DEATH( mp->signalCompleted(), "" );
	cond->notify_one();
}

static void signalError( OkMediaPlayer *mp, boost::condition_variable *cond ) {
	ASSERT_DEATH( mp->signalError("MediaPlayer error"), "" );
	cond->notify_one();
}

TEST_F( MediaPlayerTest, correct_thread ) {
	int stop=0, error=0, changed=0;
	_mp->onStop().connect( boost::bind(&mpCallback, &stop) );
	_mp->onError().connect( boost::bind(&mpCallbackError, &error, _1) );
	_mp->onMediaChanged().connect( boost::bind(&mpCallback, &changed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, stop);
	ASSERT_EQ(0, error);
	ASSERT_EQ(0, changed);

	// Simulat callbacks called from correct thread
	static_cast<OkMediaPlayer*>(_mp)->signalMediaChanged();
	ASSERT_EQ(1, changed);

	static_cast<OkMediaPlayer*>(_mp)->signalCompleted();
	ASSERT_EQ(1, stop);

	static_cast<OkMediaPlayer*>(_mp)->signalError("A MediaPlayer error ocurred");
	ASSERT_EQ(1, error);
}

TEST_F( MediaPlayerTest, incorrect_thread ) {
	int stop=0, error=0, changed=0;
	_mp->onStop().connect( boost::bind(&mpCallback, &stop) );
	_mp->onError().connect( boost::bind(&mpCallbackError, &error, _1) );
	_mp->onMediaChanged().connect( boost::bind(&mpCallback, &changed ) );

	_mp->play("test.avi");
	ASSERT_EQ(0, stop);
	ASSERT_EQ(0, error);
	ASSERT_EQ(0, changed);

	boost::condition_variable wakeup;
	boost::mutex mutex;
	boost::unique_lock<boost::mutex> lock( mutex );

	// Simulat callbacks called from incorrect thread
	{
		boost::thread thread( boost::bind(&signalMediaChanged, static_cast<OkMediaPlayer*>(_mp), &wakeup) );
		wakeup.wait(lock);
		ASSERT_EQ(0, changed);
	}

	{
		boost::thread thread( boost::bind(&signalCompleted, static_cast<OkMediaPlayer*>(_mp), &wakeup) );
		wakeup.wait(lock);
		ASSERT_EQ(0, stop);
	}

	{
		boost::thread thread( boost::bind(&signalError, static_cast<OkMediaPlayer*>(_mp), &wakeup) );
		wakeup.wait(lock);
		ASSERT_EQ(0, error);
	}
}
