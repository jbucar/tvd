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

#pragma once

#include <canvas/types.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <vector>

namespace player {

namespace impl {
	//	Types
	typedef boost::function<void (void)> TimerCallback;
	class Timer;
	struct CompareTimers : public std::binary_function<const Timer *, const Timer *, bool> {
        bool operator()( Timer *x, Timer *y );
	};

	namespace pt = boost::posix_time;

	namespace state {
		enum type { stopped, paused, running };
	}
}

class System;

class TimeLineTimer {
public:
	TimeLineTimer( System *sys );
	virtual ~TimeLineTimer();

	//	Methods
	void addTimer( util::DWORD ms, const impl::TimerCallback &fnc );
	void start();
	void stop();
	void pause();
	void unpause();

	//	Getters
	util::DWORD uptime() const;

protected:
	bool isRunning() const;
	void startTimer();
	void cancelTimer();
	void onExpired( util::id::Ident &id );
	void removeTimer( impl::Timer *timer, bool dispatch );
	void cleanTimers();
	
private:
	System *_sys;
	util::DWORD _elapsed;
	impl::state::type _state;
	impl::pt::ptime _startTimer;
	util::id::Ident _timerID;
	std::vector<impl::Timer *> _timers;
	impl::CompareTimers _cmp;
};

}
