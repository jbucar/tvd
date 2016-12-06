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

#include "../player.h"
#include "../../../src/player/timelinetimer.h"
#include "../../../src/system.h"
#include <canvas/system.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

class TimeLineTimerTest : public Player {
public:
	TimeLineTimerTest() {}
	virtual ~TimeLineTimerTest() {}

	void startTimer() {
		enqueue( boost::bind(&TimeLineTimerTest::doStartTimer, this) );
	}

	void stopTimer() {
		enqueue( boost::bind(&TimeLineTimerTest::doStopTimer,this) );
	}

	void addTimer( int ms, int *value ) {
		enqueue( boost::bind(&TimeLineTimerTest::doAddTimer, this, ms, value), false );
	}

	void pauseTimer( bool pause ) {
		enqueue( boost::bind(&TimeLineTimerTest::doPauseTimer, this, pause) );
	}

	void enqueue( const boost::function<void (void)> &fnc, bool needWait=true ) {
		ref();
		system()->enqueue( fnc );
		if (needWait)
			wait();
	}

	void onTimerExpiredDoEnd( int *value ) {
		(*value)++;
		unref();
	}

protected:
	virtual void SetUp( void ) {
		Player::SetUp();
		uptime = 0;
		_timer = createTimeLineTimer();
	}

	virtual void TearDown( void ) {
		delete _timer;
		Player::TearDown();
	}

	std::string playerType() const {
	    return "ImagePlayer";
	}

	std::string property() const {
	    return "basic";
	}

	player::type::type type() const {
		return player::type::image;
	}


	void doStartTimer() {
		_timer->start();
		unref();
	}

	void doStopTimer() {
		_timer->stop();
		unref();
	}

	void doPauseTimer( bool pause ) {
		if (pause) {
			_timer->pause();
		}
		else {
			_timer->unpause();
		}
		unref();
	}

	void doAddTimer( int ms, int *value ) {
		_timer->addTimer( ms, boost::bind(&TimeLineTimerTest::onTimerExpiredDoEnd,this,value) );
	}

	util::DWORD checkUptime() {
		uptime = 0;
		enqueue( boost::bind(&TimeLineTimerTest::doGetUptime, this) );
		wait();
		return uptime;
	}

	void doGetUptime() {
		uptime = _timer->uptime();
		unref();
	}

	util::DWORD uptime;
	player::TimeLineTimer *_timer;
};

TEST_F( TimeLineTimerTest, basic ) {
	ASSERT_EQ( checkUptime(), 0 );
}

TEST_F( TimeLineTimerTest, check_basic_uptime ) {
	startTimer();
	boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
	ASSERT_TRUE( checkUptime() != 0 );
	stopTimer();
	ASSERT_EQ( checkUptime(), 0 );
}

void do_test1( TimeLineTimerTest *test, player::TimeLineTimer *timer, int *value ) {
	timer->addTimer( 10, boost::bind(&TimeLineTimerTest::onTimerExpiredDoEnd,test,value) );
	timer->start();
}

TEST_F( TimeLineTimerTest, add_timer_before_start ) {
	int value=0;
	enqueue( boost::bind(&do_test1,this,_timer,&value) );
	stopTimer();
	ASSERT_TRUE( value == 1 );
}

void do_test2( TimeLineTimerTest *test, player::TimeLineTimer *timer, int *value ) {
	timer->start();
	timer->addTimer( 10, boost::bind(&TimeLineTimerTest::onTimerExpiredDoEnd,test,value) );
}

TEST_F( TimeLineTimerTest, add_timer_after_start ) {
	int value=0;
	enqueue( boost::bind(&do_test2,this,_timer,&value) );
	stopTimer();
	ASSERT_TRUE( value == 1 );
}

TEST_F( TimeLineTimerTest, pause_basic ) {
	//	Start timer
	startTimer();
	boost::this_thread::sleep( boost::posix_time::milliseconds(20) );
	util::DWORD up_before_pause = checkUptime();

	//	pause
	boost::this_thread::sleep( boost::posix_time::milliseconds(20) );
	pauseTimer(true);
	util::DWORD up_after_pause = checkUptime();

	//	wait and check uptime again
	boost::this_thread::sleep( boost::posix_time::milliseconds(20) );
	util::DWORD up_after_pause_and_20ms = checkUptime();
	ASSERT_TRUE( up_before_pause < up_after_pause );
	ASSERT_TRUE( up_after_pause == up_after_pause_and_20ms );

	pauseTimer(false);
	boost::this_thread::sleep( boost::posix_time::milliseconds(20) );
	ASSERT_TRUE( checkUptime() > up_after_pause );

	stopTimer();
}

TEST_F( TimeLineTimerTest, two_timers ) {
	int value1=0;
	int value2=0;
	std::vector<int> values;

	startTimer();
	addTimer( 10, &value1 );
	addTimer( 15, &value2 );
	wait();
	stopTimer();

	ASSERT_TRUE( value1 == 1 );
 	ASSERT_TRUE( value2 == 1 );
}

