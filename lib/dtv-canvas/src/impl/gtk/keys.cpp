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
#include <gdk/gdkkeysyms.h>
#include <glib.h>

namespace canvas {
namespace gtk {

util::key::type getKeyCode( guint symbol ) {
	util::key::type key;

	switch (symbol) {
		case GDK_KEY_0: key = (util::key::number_0); break;
		case GDK_KEY_1: key = (util::key::number_1); break;
		case GDK_KEY_2: key = (util::key::number_2); break;
		case GDK_KEY_3: key = (util::key::number_3); break;
		case GDK_KEY_4: key = (util::key::number_4); break;
		case GDK_KEY_5: key = (util::key::number_5); break;
		case GDK_KEY_6: key = (util::key::number_6); break;
		case GDK_KEY_7: key = (util::key::number_7); break;
		case GDK_KEY_8: key = (util::key::number_8); break;
		case GDK_KEY_9: key = (util::key::number_9); break;

		case GDK_KEY_a: key = (util::key::small_a); break;
		case GDK_KEY_b: key = (util::key::small_b); break;
		case GDK_KEY_c: key = (util::key::small_c); break;
		case GDK_KEY_d: key = (util::key::small_d); break;
		case GDK_KEY_e: key = (util::key::small_e); break;
		case GDK_KEY_f: key = (util::key::small_f); break;
		case GDK_KEY_g: key = (util::key::small_g); break;
		case GDK_KEY_h: key = (util::key::small_h); break;
		case GDK_KEY_i: key = (util::key::small_i); break;
		case GDK_KEY_j: key = (util::key::small_j); break;
		case GDK_KEY_k: key = (util::key::small_k); break;
		case GDK_KEY_l: key = (util::key::small_l); break;
		case GDK_KEY_m: key = (util::key::small_m); break;
		case GDK_KEY_n: key = (util::key::small_n); break;
		case GDK_KEY_o: key = (util::key::small_o); break;
		case GDK_KEY_p: key = (util::key::small_p); break;
		case GDK_KEY_q: key = (util::key::small_q); break;
		case GDK_KEY_r: key = (util::key::small_r); break;
		case GDK_KEY_s: key = (util::key::small_s); break;
		case GDK_KEY_t: key = (util::key::small_t); break;
		case GDK_KEY_u: key = (util::key::small_u); break;
		case GDK_KEY_v: key = (util::key::small_v); break;
		case GDK_KEY_w: key = (util::key::small_w); break;
		case GDK_KEY_x: key = (util::key::small_x); break;
		case GDK_KEY_y: key = (util::key::small_y); break;
		case GDK_KEY_z: key = (util::key::small_z); break;

		case GDK_KEY_A: key = (util::key::capital_a); break;
		case GDK_KEY_B: key = (util::key::capital_b); break;
		case GDK_KEY_C: key = (util::key::capital_c); break;
		case GDK_KEY_D: key = (util::key::capital_d); break;
		case GDK_KEY_E: key = (util::key::capital_e); break;
		case GDK_KEY_F: key = (util::key::capital_f); break;
		case GDK_KEY_G: key = (util::key::capital_g); break;
		case GDK_KEY_H: key = (util::key::capital_h); break;
		case GDK_KEY_I: key = (util::key::capital_i); break;
		case GDK_KEY_J: key = (util::key::capital_j); break;
		case GDK_KEY_K: key = (util::key::capital_k); break;
		case GDK_KEY_L: key = (util::key::capital_l); break;
		case GDK_KEY_M: key = (util::key::capital_m); break;
		case GDK_KEY_N: key = (util::key::capital_n); break;
		case GDK_KEY_O: key = (util::key::capital_o); break;
		case GDK_KEY_P: key = (util::key::capital_p); break;
		case GDK_KEY_Q: key = (util::key::capital_q); break;
		case GDK_KEY_R: key = (util::key::capital_r); break;
		case GDK_KEY_S: key = (util::key::capital_s); break;
		case GDK_KEY_T: key = (util::key::capital_t); break;
		case GDK_KEY_U: key = (util::key::capital_u); break;
		case GDK_KEY_V: key = (util::key::capital_v); break;
		case GDK_KEY_W: key = (util::key::capital_w); break;
		case GDK_KEY_X: key = (util::key::capital_x); break;
		case GDK_KEY_Y: key = (util::key::capital_y); break;
		case GDK_KEY_Z: key = (util::key::capital_z); break;

		case GDK_KEY_Page_Down: key = (util::key::page_down); break;
		case GDK_KEY_Page_Up  : key = (util::key::page_up); break;

		case GDK_KEY_F1 : key = (util::key::f1); break;
		case GDK_KEY_F2 : key = (util::key::f2); break;
		case GDK_KEY_F3 : key = (util::key::f3); break;
		case GDK_KEY_F4 : key = (util::key::f4); break;
		case GDK_KEY_F5 : key = (util::key::f5); break;
		case GDK_KEY_F6 : key = (util::key::f6); break;
		case GDK_KEY_F7 : key = (util::key::f7); break;
		case GDK_KEY_F8 : key = (util::key::f8); break;
		case GDK_KEY_F9 : key = (util::key::f9); break;
		case GDK_KEY_F10: key = (util::key::f10); break;
		case GDK_KEY_F11: key = (util::key::f11); break;
		case GDK_KEY_F12: key = (util::key::f12); break;

		case GDK_KEY_plus : key = (util::key::plus_sign); break;
		case GDK_KEY_minus: key = (util::key::minus_sign); break;

		case GDK_KEY_asterisk   : key = (util::key::asterisk); break;
		case GDK_KEY_numbersign : key = (util::key::number_sign); break;
		case GDK_KEY_period: key = (util::key::period); break;

		case GDK_KEY_Print: key = (util::key::printscreen); break;

		case GDK_KEY_Super_L:
		case GDK_KEY_Super_R:
			key = (util::key::super); break;
			
		case GDK_KEY_Menu : key = (util::key::menu); break;
		case GDK_KEY_Red   : key = (util::key::red); break;
		case GDK_KEY_Green : key = (util::key::green); break;
		case GDK_KEY_Yellow: key = (util::key::yellow); break;
		case GDK_KEY_Blue  : key = (util::key::blue); break;
		case GDK_KEY_Down : key = (util::key::cursor_down); break;
		case GDK_KEY_Left : key = (util::key::cursor_left); break;
		case GDK_KEY_Right: key = (util::key::cursor_right); break;
		case GDK_KEY_Up   : key = (util::key::cursor_up); break;

		case GDK_KEY_Return: key = (util::key::enter); break;

		case GDK_KEY_BackSpace: key = (util::key::backspace); break;
		case GDK_KEY_Escape: key = (util::key::escape); break;

		case GDK_KEY_PowerOff : key = (util::key::power); break;
		case GDK_KEY_Pause : key = (util::key::pause); break;

		case GDK_KEY_greater: key = (util::key::greater_than_sign); break;
		case GDK_KEY_less   : key = (util::key::less_than_sign); break;

		default:
			key = (util::key::null);
			break;
	};

	return key;
}

}
}

