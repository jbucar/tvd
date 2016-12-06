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
#include "dispatcher.h"
#include "player.h"
#include "../egl/windowimpl.h"
#include "../../inputmanager.h"
#include <util/android/android.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/math/special_functions.hpp>
#include <android/input.h>

namespace canvas {
namespace android {

System::System()
{
}

System::~System()
{
}

//	Initialization
bool System::init() {
	LINFO("android", "Init");
	//	Setup input callback
	util::android::glue()->setInputCallback( boost::bind(&System::onInput,this,_1) );
	return true;
}

void System::fin() {
	LINFO("android", "Fin");
	util::android::Glue::InputCallback empty;
	util::android::glue()->setInputCallback( empty );
}

//	Instance creation
util::io::Dispatcher *System::createIO() const {
	return new Dispatcher();
}

canvas::Player *System::createPlayer() const {
	return new Player();
}

WebViewer *System::createWebViewer( Surface * ) {
	return NULL;
}

canvas::Window *System::createWindow() const {
	egl::NativeTypes native;
	native.display = EGL_DEFAULT_DISPLAY;
	native.window = util::android::glue()->window();
	native.width = ANativeWindow_getWidth( util::android::glue()->window() );
	native.height = ANativeWindow_getHeight( util::android::glue()->window() );
	return new egl::WindowImpl( native );
}

//	Implementation
int32_t System::onInput( AInputEvent *event ) {
	int32_t type = AInputEvent_getType(event);
	if (type == AINPUT_EVENT_TYPE_KEY) {
		int32_t action = AKeyEvent_getAction(event);
		if (action == AKEY_EVENT_ACTION_DOWN || action == AKEY_EVENT_ACTION_UP) {
			bool isUp = (action == AKEY_EVENT_ACTION_UP);
			int32_t keyCode = AKeyEvent_getKeyCode(event);

			util::key::type key;
			if (keyCode >= AKEYCODE_0 && keyCode <= AKEYCODE_9) {
				key = (util::key::type) (util::key::number_0 + (keyCode - AKEYCODE_0));
			}
			else if (keyCode >= AKEYCODE_A && keyCode <= AKEYCODE_Z) {
				int32_t mod = AKeyEvent_getMetaState(event);
				if (mod & AMETA_SHIFT_ON) {
					key = (util::key::type) (util::key::capital_a + (keyCode - AKEYCODE_A));
				}
				else {
					key = (util::key::type) (util::key::small_a + (keyCode - AKEYCODE_A));
				}
			}
			else if (keyCode >= 131 && keyCode <= 140) {
				key = (util::key::type) (util::key::f1 + (keyCode - 131));
			}
			else if (keyCode == AKEYCODE_ENTER || keyCode == AKEYCODE_DPAD_CENTER) {
				key = util::key::enter;
			}
			else if (keyCode == AKEYCODE_MENU) {
				key = util::key::menu;
			}
			else if (keyCode == AKEYCODE_POWER) {
				key = util::key::power;
			}
			else if (keyCode == AKEYCODE_BACK) {
				key = util::key::back;
			}
			else if (keyCode == AKEYCODE_VOLUME_UP) {
				key = util::key::volume_up;
			}
			else if (keyCode == AKEYCODE_VOLUME_DOWN) {
				key = util::key::volume_down;
			}
			else if (keyCode == AKEYCODE_SPACE) {
				key = util::key::backspace;
			}
			else if (keyCode == AKEYCODE_PAGE_UP) {
				key = util::key::page_up;
			}
			else if (keyCode == AKEYCODE_PAGE_DOWN) {
				key = util::key::page_down;
			}
			else if (keyCode == AKEYCODE_PLUS) {
				key = util::key::plus_sign;
			}
			else if (keyCode == AKEYCODE_MINUS) {
				key = util::key::minus_sign;
			}
			else if (keyCode == AKEYCODE_PERIOD) {
				key = util::key::period;
			}
			else if (keyCode == AKEYCODE_NUM) {
				key = util::key::number_sign;
			}
			else if (keyCode == AKEYCODE_DPAD_UP) {
				key = util::key::cursor_up;
			}
			else if (keyCode == AKEYCODE_DPAD_DOWN) {
				key = util::key::cursor_down;
			}
			else if (keyCode == AKEYCODE_DPAD_LEFT) {
				key = util::key::cursor_left;
			}
			else if (keyCode == AKEYCODE_DPAD_RIGHT) {
				key = util::key::cursor_right;
			}
			else {
				key = util::key::null;
				LWARN( "android", "Input key not maped: code=%d", keyCode );
			}

			if (key != util::key::null) {
				input()->dispatchKey( key, isUp );
			}
		}
	}
	else if (type == AINPUT_EVENT_TYPE_MOTION) {
		int32_t source = AInputEvent_getSource(event);
		if (source == AINPUT_SOURCE_MOUSE || source == AINPUT_SOURCE_TOUCHSCREEN) {
			int32_t action = AMotionEvent_getAction(event);
			if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_UP) {
				input::ButtonEvent evt;
				evt.button = 1;
				evt.x = boost::math::iround(AMotionEvent_getX(event,0));
				evt.y = boost::math::iround(AMotionEvent_getY(event,0));
				evt.isPress = (action == AMOTION_EVENT_ACTION_DOWN);
				input::dispatchButtonEvent( this, &evt );
			}
		}
	}

	return 0;
}

}
}
