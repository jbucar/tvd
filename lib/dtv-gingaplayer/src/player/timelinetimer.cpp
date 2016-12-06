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

#include "timelinetimer.h"
#include "../system.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace player {

namespace impl {

class Timer {
public:
	Timer( util::DWORD ms, const impl::TimerCallback &fnc )
		: _ms(ms), _fnc(fnc) { DTV_ASSERT(!fnc.empty()); }
	virtual ~Timer() {}

	util::DWORD ms() const { return _ms; }
	void dispatch() { _fnc(); }

private:
	util::DWORD _ms;
	impl::TimerCallback _fnc;
};
	
bool CompareTimers::operator()( impl::Timer *x, impl::Timer *y ) { 
	return x->ms() > y->ms(); 
}

}	//	namespace impl


TimeLineTimer::TimeLineTimer( System *sys )
	: _sys( sys )
{
	_state = impl::state::stopped;
	_elapsed = 0;
	std::make_heap( _timers.begin(), _timers.end(), _cmp );
}

TimeLineTimer::~TimeLineTimer()
{
	stop();
	cleanTimers();
}

//	Methods
void TimeLineTimer::addTimer( util::DWORD ms, const impl::TimerCallback &fnc ) {
	cancelTimer();
	
	//	Add timer
	_timers.push_back( new impl::Timer(ms,fnc) );
	std::push_heap( _timers.begin(), _timers.end(), _cmp );
	
	startTimer();
}

void TimeLineTimer::start() {
	if (_state != impl::state::running) {
		_state = impl::state::running;
		_elapsed = 0;
		startTimer();
	}
}

void TimeLineTimer::stop() {
	if (_state > impl::state::stopped) {
		cancelTimer();
		_state = impl::state::stopped;
		_elapsed = 0;
		cleanTimers();
	}
}

void TimeLineTimer::pause() {
	if (_state == impl::state::running) {
		cancelTimer();
		_state = impl::state::paused;
	}
}

void TimeLineTimer::unpause() {
	if (_state == impl::state::paused) {
		_state = impl::state::running;
		startTimer();
	}
}

//	Getters
bool TimeLineTimer::isRunning() const {
	return _state == impl::state::running;
}

util::DWORD TimeLineTimer::uptime() const {
	LDEBUG("TimeLineTimer", "uptime: state=%d, _elapsed=%d", _state, _elapsed);
	if (_state == impl::state::running) {
		//	Running
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();
		DTV_ASSERT( now >= _startTimer );
		impl::pt::time_duration d=now - _startTimer;
		return _elapsed + (util::DWORD) d.total_milliseconds();
	}
	else if (_state == impl::state::paused) {
		return _elapsed;
	}
	else {
		return 0;
	}
}

//	Aux
void TimeLineTimer::cleanTimers() {
	while (!_timers.empty()) {
		removeTimer( _timers.front(), false );
	}
}

void TimeLineTimer::startTimer() {
	if (isRunning()) {
		//	Start timer
		_startTimer = impl::pt::microsec_clock::local_time();

		if (!util::id::isValid(_timerID)) {
			while (!_timers.empty()) {
				//	Get minimum timer
				impl::Timer *timer=_timers.front();

				//	Is timer expired?
				if (_elapsed > timer->ms()) {
					//	Dispatch timer callback and remove from list
					removeTimer( timer, true );
				}
				else {
					//	Start system timer
					util::DWORD min = timer->ms() - _elapsed;
					LINFO("TimeLineTimer", "Timer: min=%d", min);
					_timerID = _sys->registerTimer(
						min,
						boost::bind(&TimeLineTimer::onExpired,this,_1) );
					break;
				}
			}
		}
	}
}

void TimeLineTimer::cancelTimer() {
	if (isRunning()) {
		//	Cancel system timer
		_sys->unregisterTimer( _timerID );

		//	Add elapsed time
		impl::pt::ptime now = impl::pt::microsec_clock::local_time();
		_elapsed = _elapsed + (util::DWORD) (now - _startTimer).total_milliseconds();
	}
}

void TimeLineTimer::removeTimer( impl::Timer *timer, bool dispatch ) {
	DTV_ASSERT(timer);
	
	//	Remove from list
	std::pop_heap( _timers.begin(), _timers.end(), _cmp );
	_timers.pop_back();

	//	Dispatch
	if (dispatch) {
		timer->dispatch();
	}

	//	Clean
	delete timer;
}

void TimeLineTimer::onExpired( util::id::Ident &id ) {
	DTV_ASSERT( util::id::isValid(id) );
	id.reset();

	LINFO("TimeLineTimer", "Timer expired: uptime=%d", uptime());
	
	//	Dispatch
	removeTimer( _timers.front(), true );

	//	Start timer again
	cancelTimer();
	startTimer();
}

}
