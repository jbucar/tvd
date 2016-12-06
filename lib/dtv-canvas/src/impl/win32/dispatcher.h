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

#include "../../event.h"
#include <util/io/dispatcher.h>
#include <windows.h>

namespace util {
namespace task {
	class DispatcherImpl;
}
}

namespace canvas {
namespace win32 {

class System;
class Window;

class Dispatcher : public util::io::Dispatcher {
public:
	explicit Dispatcher( System *sys );
	virtual ~Dispatcher();

	//	Initialization
	virtual bool initialize();
	virtual void finalize();

	//	Run/exit io event notification loop
	virtual void run();
	virtual void exit();

	//	Dispatcher
	virtual util::task::Dispatcher *dispatcher() const;

	//	Timer events
	virtual util::id::Ident addTimer( int ms, const util::io::Callback &callback );
	virtual void stopTimer( util::id::Ident &id );

	//	IO events
	virtual util::id::Ident addIO( util::DESCRIPTOR fd, const util::io::Callback &callback );
	virtual void stopIO( util::id::Ident &id );

	//	Socket events
	virtual util::id::Ident addSocket( util::SOCKET sock, const util::io::Callback &callback );
	virtual void stopSocket( util::id::Ident &id );

	//	Aux
	typedef Event<int> Timer;
	typedef std::vector<Timer *> TimerList;
	void onDispatchTimer( long int timerId );

	//	Implementation
	void setupWindow( Window *_win );
	LRESULT wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	System *sys() const;

protected:
	void onPost();
	void onTaskEvent();
	void onExit();

private:
	System *_sys;
	Window *_win;
	util::id::Pool *_pool;
	util::task::DispatcherImpl *_tasks;
	TimerList _timerList;
};

}
}

