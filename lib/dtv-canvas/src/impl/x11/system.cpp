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

#include "system.h"
#include "window.h"
#include "../../inputmanager.h"
#include <util/log.h>
#include <util/io/dispatcher.h>
#include <boost/bind.hpp>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

namespace canvas {
namespace x11 {

System::System()
{
	_display = NULL;
}

System::~System()
{
}

//	Instance creation
canvas::Window *System::createWindow() const {
	return new Window( _display );
}

//	Initialization
bool System::init() {
	LINFO("x11::System", "Init events");

	//	Initialize X11 threads
	if (!XInitThreads()) {
		LWARN("x11::System", "cannot init threads");
		return false;
	}

	//	Open display
	_display=XOpenDisplay(getenv("DISPLAY"));
	if (!_display) {
		LWARN("x11::System", "cannot open display");
		return false;
	}

	//	Start listen of X11 connection
	_xConnection = io()->addIO( XConnectionNumber(_display), boost::bind(&System::onEvent,this) );
	if (!util::id::isValid(_xConnection)) {
		LWARN("x11::System", "cannot register connection io");
		return false;
	}

	return true;
}

void System::fin() {
	LINFO("x11::System", "Fin events");

	//	Stop listen of X11 connection
	io()->stopIO( _xConnection );

	//	Close display
	XSync(_display, False);
	XCloseDisplay(_display);
}

void System::onEvent() {
	while (XPending(_display) > 0) {
		XEvent ev;
		XNextEvent(_display, &ev);

		if (ev.type == KeyPress || ev.type == KeyRelease) {
			onKeyEvent( ev.xkey.keycode, ev.type == KeyRelease, ev.xkey.state );
		}
		else if (ev.type == ButtonPress || ev.type == ButtonRelease) {
			if (ev.xbutton.button <= Button5) {
				input::ButtonEvent evt;
				evt.button = ev.xbutton.button;
				evt.x = ev.xbutton.x;
				evt.y = ev.xbutton.y;
				evt.isPress = (ev.type == ButtonPress) ? true : false;
				input::dispatchButtonEvent( this, &evt );
			}
		}
		else if (ev.type == ClientMessage) {
			Atom wm_delete = XInternAtom(_display, "WM_DELETE_WINDOW", true);
			if ((Atom)ev.xclient.data.l[0] == wm_delete) {
				dispatchShutdown();
			}
		}
		else {
			dynamic_cast<Window*>(window())->onEvent( ev );
		}
	}
}

void System::onKeyEvent( unsigned int xKey, bool isUp, unsigned int state ) {
	util::key::type key = util::key::null;

	unsigned int mod;
	KeySym sym;
	if (XkbLookupKeySym( _display, (KeyCode)xKey, state, &mod, &sym ) == False) {
		LWARN("x11::System", "Symbol not found: xKey=0x%lx", xKey);
		return;
	}

	if (sym >= XK_F1 && sym <= XK_F12) {
		key = static_cast<util::key::type> ( util::key::f1 + ( sym - XK_F1 ));
	} else if (sym >= XK_0 && sym <= XK_9) {
		key = static_cast<util::key::type> ( util::key::number_0 + ( sym - XK_0 ));
	} else if (sym >= XK_a && sym <= XK_z) {
		key = static_cast<util::key::type> ( util::key::small_a + ( sym - XK_a ));
	} else if (sym >= XK_A && sym <= XK_Z) {
		key = static_cast<util::key::type> ( util::key::capital_a + ( sym - XK_A ));
	} else {
		switch (sym) {
			case XK_space:
				key = util::key::backspace;
				break;
			case XK_Left:
				key = util::key::cursor_left;
				break;
			case XK_Up:
				key = util::key::cursor_up;
				break;
			case XK_Right:
				key = util::key::cursor_right;
				break;
			case XK_Down:
				key = util::key::cursor_down;
				break;
			case XK_Page_Up:
				key = util::key::page_up;
				break;
			case XK_Page_Down:
				key = util::key::page_down;
				break;
			case XK_Pause:
				key = util::key::pause;
				break;
			case XK_Menu:
				key = util::key::menu;
				break;
			case XK_Escape:
				key = util::key::escape;
				break;
			case XK_Return:
				key = util::key::enter;
				break;
			case XK_BackSpace:
				key = util::key::back;
				break;
			case XK_numbersign:
				key = util::key::number_sign;
				break;
			default:
				LWARN("x11::System", "key not mapped: sym=0x%lx", sym);
				break;
		};
	}

	input()->dispatchKey( key, isUp );
}

}
}
