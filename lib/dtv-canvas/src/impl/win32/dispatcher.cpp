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

#include "dispatcher.h"
#include "window.h"
#include "system.h"
#include <util/task/dispatcherimpl.h>
#include <util/id/pool.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/bind.hpp>

#define WM_TASK_ADD_MSG (WM_USER+0)

namespace canvas {
namespace win32 {

namespace impl {

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	Dispatcher *disp = (Dispatcher *)::GetWindowLong( hWnd, GWL_USERDATA );
	if (!disp)
		return DefWindowProc( hWnd, msg, wParam, lParam );

	return disp->wndProc( hWnd, msg, wParam, lParam );
}

}

Dispatcher::Dispatcher( System *sys )
	: _sys(sys)
{
	_pool = new util::id::Pool("win32");

	// Create task dispatcher
	_tasks = new util::task::DispatcherImpl();
	_tasks->onPost( boost::bind(&Dispatcher::onPost,this) );
}

Dispatcher::~Dispatcher()
{
	delete _pool;
	delete _tasks;
}

//	Initialization
bool Dispatcher::initialize() {
	_tasks->registerTarget( this, "canvas::win32::System" );
	return true;
}

void Dispatcher::finalize() {
	_tasks->runAll();
	_tasks->unregisterTarget( this );
}

//	Run/exit io event notification loop
void Dispatcher::run() {
	LINFO("win32", "Main loop begin");

	MSG msg;
	BOOL bRet;
	while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			break;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	LINFO("win32", "Main loop end");
}

void Dispatcher::exit() {
	//	Wakeup thread
	_tasks->post( this, boost::bind(&Dispatcher::onExit,this) );
}

//	Dispatcher
util::task::Dispatcher *Dispatcher::dispatcher() const {
	return _tasks;
}

void Dispatcher::onExit() {
	PostQuitMessage(0);
}

void Dispatcher::onTaskEvent() {
	_tasks->runAll();
}

void Dispatcher::onPost() {
	if (PostMessage( _win->handle(), WM_TASK_ADD_MSG, 0, 0 ) == 0) {
		LWARN( "win32", "Cannot post task" );
	}
}

//	Timer events
util::id::Ident Dispatcher::addTimer( int ms, const util::io::Callback &callback ) {
	Timer *ev = new Timer(_pool->alloc(), callback, this);
	UINT_PTR id = (UINT_PTR)ev->id()->getID();
	int r = SetTimer( _win->handle(), id, ms, (TIMERPROC) NULL );
	return startEvent( ev, r != 0, id, _timerList );
}

void Dispatcher::stopTimer( util::id::Ident &id ) {
	UINT_PTR timerID;
	if (stopEvent( id, _timerList, timerID )) {
		KillTimer( _win->handle(), timerID );
	}
}

void Dispatcher::onDispatchTimer( long int timerId ) {
	//	Two pass, becouse in dispatch callback can unregister timer!
	util::id::Ident id=dispatchEventByValue( _timerList, timerId );
	stopTimer( id );
}

//	IO events
util::id::Ident Dispatcher::addIO( util::DESCRIPTOR /*fd*/, const util::io::Callback & /*callback*/ ) {
	util::id::Ident id;
	return id;
}

void Dispatcher::stopIO( util::id::Ident & /*id*/ ) {
}

//	Socket events
util::id::Ident Dispatcher::addSocket( util::SOCKET /*sock*/, const util::io::Callback & /*callback*/ ) {
	util::id::Ident id;
	return id;
}

void Dispatcher::stopSocket( util::id::Ident & /*id*/ ) {
}

//	Implementation
void Dispatcher::setupWindow( Window *win ) {
	//	Setup user data
	_win = win;
	::SetWindowLong( _win->handle(), GWL_USERDATA, (LONG)((void*)this) );
}

System *Dispatcher::sys() const {
	return _sys;
}

LRESULT Dispatcher::wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch (msg) {
		case WM_TASK_ADD_MSG:
			onTaskEvent();
			break;
		case WM_TIMER:
			LDEBUG( "win32", "On timer, id=%d", wParam );
			onDispatchTimer( (long int)wParam );
			break;
		case WM_CLOSE: {
			LDEBUG( "win32", "On close event" );
			_sys->dispatchShutdown();
			return 0;
		}
		case WM_DESTROY:
			// we dont assume we will ever expect a windows to actually close;
			// a tipical scenario leaves the windows opened;
			// stops processing window messages; and quits the application
			// leaving the window object as a memory leak
			break;
		default: {
			if (_win) {
				return _win->wndProc( hWnd, msg, wParam, lParam );
			}
		}
	};

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

}
}

