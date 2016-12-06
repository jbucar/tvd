/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "imageplayer.h"
#include "../../util.h"
#include "../../../../src/player/imageplayer.h"
#include "../../../../src/system.h"
#include <canvas/system.h>
#include <util/types.h>
#include <boost/thread.hpp>

class ImagePlayerExplicitDuration : public ImagePlayer {
protected:
    std::string property() const { return "explicitDuration"; };
};

static void cb_wakeup( boost::condition_variable *wakeup) {
	wakeup->notify_one();
}

static void cb_wakeup2( boost::condition_variable *wakeup, boost::mutex *mutex, bool *end ) {
	mutex->lock();
	*end = true;
	mutex->unlock();
	wakeup->notify_one();
}

// Test setting explicitDur to 2ms and call a timer to check the image after that time
TEST_F( ImagePlayerExplicitDuration, duration_of_1ms ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", 1000 ) );


	ASSERT_TRUE ( play(_player) );

	// Assert that the player is initially playing
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

	boost::condition_variable wakeup;
	boost::mutex mutex;
	boost::unique_lock<boost::mutex> lock( mutex );
	_player->onStopped( boost::bind(&cb_wakeup, &wakeup) );
	wakeup.wait(lock);

	// Assert that the player is not playing after 1000 ms
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_finish") ) );
}

// Tests setting explicitDur to 0
TEST_F( ImagePlayerExplicitDuration, duration_of_0ms ) {
    ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
    ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
    ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", 0 ) );
    ASSERT_TRUE ( play(_player) );

    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );
}

// Tests setting explicitDur to an invalid value
TEST_F( ImagePlayerExplicitDuration, set_negative_value ) {
    ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
    ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
    ASSERT_FALSE( setProperty<util::DWORD>( _player, "explicitDur", -1 ) );
    ASSERT_TRUE ( play(_player) );
    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );
}

// Test unsetting explicitDur
TEST_F( ImagePlayerExplicitDuration, unsetting_explicitDur ) {
    ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
    ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
    ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", 100 ) );
    ASSERT_TRUE ( play(_player) );
    ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", 0 ) );

    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

    boost::condition_variable wakeup;
    boost::mutex mutex;
    boost::unique_lock<boost::mutex> lock( mutex );

    registerTimer(300, boost::bind(&cb_wakeup, &wakeup));
    wakeup.wait(lock);

    // Assert that the player is still playing after 1000 ms beacuse the explicitDur was unsetted
    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );
}

// Test setting explicitDur dinamically with a time smaller than the uptime
TEST_F( ImagePlayerExplicitDuration, set_dinamically_lt_uptime ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );

	bool end=false;
	boost::mutex mutex;
	boost::condition_variable wakeup;
	_player->onStopped( boost::bind(&cb_wakeup2, &wakeup, &mutex, &end) );

	ASSERT_TRUE ( play(_player) );

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

	int uptime = _player->uptime();
	ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", uptime ) );

	bool fin = false;
    while (!fin) {
		boost::unique_lock<boost::mutex> lock( mutex );
		while (!end) {
			wakeup.wait(lock);
		}
		fin = end;
	}

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_finish") ) );
}

// Test setting explicitDur dinamically with a time greater than the uptime
TEST_F( ImagePlayerExplicitDuration, set_dinamically_gt_uptime ) {
    ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
    ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );

    ASSERT_TRUE ( play(_player) );

    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

    int uptime = _player->uptime();
    ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", uptime + 500  ) );

    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

    boost::condition_variable wakeup;
    boost::mutex mutex;
    boost::unique_lock<boost::mutex> lock( mutex );
    _player->onStopped( boost::bind(&cb_wakeup, &wakeup) );
    wakeup.wait(lock);

    ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_finish") ) );
}

// Test setting explicitDur dinamically with a time greater than the uptime
TEST_F( ImagePlayerExplicitDuration, set_dinamically_not_enough_gt_uptime ) {
	boost::condition_variable wakeup;

	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	_player->onStopped( boost::bind(&cb_wakeup, &wakeup) );

	ASSERT_TRUE ( play(_player) );

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_finish") ) );

	int uptime = _player->uptime();
	ASSERT_TRUE ( setProperty<util::DWORD>( _player, "explicitDur", uptime + 80  ) );

	boost::mutex mutex;
	boost::unique_lock<boost::mutex> lock( mutex );
	wakeup.wait(lock);

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_finish") ) );
}
