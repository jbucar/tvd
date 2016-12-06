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
#include <windows.h>

namespace canvas {
namespace win32 {

namespace impl {

static inline bool getShiftPressed() {
	return ::GetKeyState (VK_SHIFT  ) < 0;
}

static inline bool getControlPressed() {
	return ::GetKeyState (VK_CONTROL) < 0;
}

static inline bool getAltPressed() {
	return ::GetKeyState (VK_MENU   ) < 0;
}

static inline bool getCapsToggled() {
	return ::GetKeyState (VK_CAPITAL) & 1;
}

};

//util::key::type getKeyCode( unsigned int wParam );
/*
util::key::type getKeyCode( util::DWORD wParam ) {
	util::key::type key;
	// unsigned int flags = 0;
	// flags |= impl::getShiftPressed()   ? K_SHIFT_DOWN   : 0;
	// flags |= impl::getControlPressed() ? K_CTRL_DOWN    : 0;
	// flags |= impl::getAltPressed()     ? K_ALT_DOWN     : 0;
	// flags |= impl::getCapsToggled()    ? K_CAPS_TOGGLED : 0;
	switch (wParam) {
		case VK_NUMPAD0        :
		case VK_NUMPAD1        :
		case VK_NUMPAD2        :
		case VK_NUMPAD3        :
		case VK_NUMPAD4        :
		case VK_NUMPAD5        :
		case VK_NUMPAD6        :
		case VK_NUMPAD7        :
		case VK_NUMPAD8        :
		case VK_NUMPAD9        :
		case VK_F1             :
		case VK_F2             :
		case VK_F3             :
		case VK_F4             :
		case VK_F5             :
		case VK_F6             :
		case VK_F7             :
		case VK_F8             :
		case VK_F9             :
		case VK_F10            :
		case VK_F11            :
		case VK_F12            :
		case VK_LEFT           :
		case VK_UP             :
		case VK_RIGHT          :
		case VK_DOWN           :
		case VK_CAPITAL        :
		case VK_END            :
		case VK_HOME           :
		case VK_INSERT         :
		case VK_DELETE         :
		case VK_PRIOR          :
		case VK_NEXT           :
			//flags |= K_EXTENDED;
			key = getKeyCode ( (unsigned int) wParam );
			break;
		default:
			//	Ignoring defaults.	Will be caught by WM_CHAR
			key = (util::key::null);
			break;
	};
	return key;
}
*/

util::key::type getKeyCode( util::DWORD wParam ) {
	util::key::type key;

	bool isCapsOn = impl::getCapsToggled();
	bool mayus = (( impl::getShiftPressed() && !isCapsOn ) || ( !impl::getShiftPressed() && isCapsOn ));

	switch (wParam) {
		case 0x30: key = (util::key::number_0); break;
		case 0x31: key = (util::key::number_1); break;
		case 0x32: key = (util::key::number_2); break;
		case 0x33: key = (util::key::number_4); break;
		case 0x34: key = (util::key::number_4); break;
		case 0x35: key = (util::key::number_5); break;
		case 0x36: key = (util::key::number_6); break;
		case 0x37: key = (util::key::number_7); break;
		case 0x38: key = (util::key::number_8); break;
		case 0x39: key = (util::key::number_9); break;

		case 0x41:
			key = mayus ? util::key::capital_a : util::key::small_a;
			break;
		case 0x42:
			key = mayus ? util::key::capital_b : util::key::small_b;
			break;
		case 0x43:
			key = mayus ? util::key::capital_c : util::key::small_c;
			break;
		case 0x44:
			key = mayus ? util::key::capital_d : util::key::small_d;
			break;
		case 0x45:
			key = mayus ? util::key::capital_e : util::key::small_e;
			break;
		case 0x46:
			key = mayus ? util::key::capital_f : util::key::small_f;
			break;
		case 0x47:
			key = mayus ? util::key::capital_g : util::key::small_g;
			break;
		case 0x48:
			key = mayus ? util::key::capital_h : util::key::small_h;
			break;
		case 0x49:
			key = mayus ? util::key::capital_i : util::key::small_i;
			break;
		case 0x4A:
			key = mayus ? util::key::capital_j : util::key::small_j;
			break;
		case 0x4B:
			key = mayus ? util::key::capital_k : util::key::small_k;
			break;
		case 0x4C:
			key = mayus ? util::key::capital_l : util::key::small_l;
			break;
		case 0x4D:
			key = mayus ? util::key::capital_m : util::key::small_m;
			break;
		case 0x4E:
			key = mayus ? util::key::capital_n : util::key::small_n;
			break;
		case 0x4F:
			key = mayus ? util::key::capital_o : util::key::small_o;
			break;
		case 0x50:
			key = mayus ? util::key::capital_p : util::key::small_p;
			break;
		case 0x51:
			key = mayus ? util::key::capital_q : util::key::small_q;
			break;
		case 0x52:
			key = mayus ? util::key::capital_r : util::key::small_r;
			break;
		case 0x53:
			key = mayus ? util::key::capital_s : util::key::small_s;
			break;
		case 0x54:
			key = mayus ? util::key::capital_t : util::key::small_t;
			break;
		case 0x55:
			key = mayus ? util::key::capital_u : util::key::small_u;
			break;
		case 0x56:
			key = mayus ? util::key::capital_v : util::key::small_v;
			break;
		case 0x57:
			key = mayus ? util::key::capital_w : util::key::small_w;
			break;
		case 0x58:
			key = mayus ? util::key::capital_x : util::key::small_x;
			break;
		case 0x59:
			key = mayus ? util::key::capital_y : util::key::small_y;
			break;
		case 0x5A:
			key = mayus ? util::key::capital_z : util::key::small_z;
			break;

		case VK_NEXT  : key = (util::key::page_down); break;
		case VK_PRIOR : key = (util::key::page_up); break;

		case VK_F1 : key = (util::key::f1); break;
		case VK_F2 : key = (util::key::f2); break;
		case VK_F3 : key = (util::key::f3); break;
		case VK_F4 : key = (util::key::f4); break;
		case VK_F5 : key = (util::key::f5); break;
		case VK_F6 : key = (util::key::f6); break;
		case VK_F7 : key = (util::key::f7); break;
		case VK_F8 : key = (util::key::f8); break;
		case VK_F9 : key = (util::key::f9); break;
		case VK_F10: key = (util::key::f10); break;
		case VK_F11: key = (util::key::f11); break;
		case VK_F12: key = (util::key::f12); break;

		case VK_OEM_PLUS  : key = (util::key::plus_sign); break;
		case VK_OEM_MINUS : key = (util::key::minus_sign); break;
		case VK_OEM_PERIOD: key = (util::key::period); break;

		case VK_PRINT: key = (util::key::printscreen); break;
			
		case VK_DOWN : key = (util::key::cursor_down); break;
		case VK_LEFT : key = (util::key::cursor_left); break;
		case VK_RIGHT: key = (util::key::cursor_right); break;
		case VK_UP   : key = (util::key::cursor_up); break;

		case VK_RETURN: key = (util::key::enter); break;

		case VK_BACK:   key = (util::key::backspace); break;
		case VK_ESCAPE: key = (util::key::escape); break;

		case VK_PAUSE : key = (util::key::pause); break;

		default:
			key = (util::key::null);
			break;
	};

	return key;
}

}
}
