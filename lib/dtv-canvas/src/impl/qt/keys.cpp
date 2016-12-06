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

#include <util/keydefs.h>
#include <QKeyEvent>

namespace canvas {
namespace qt {

util::key::type getKeyCode( int symbol ) {
	util::key::type key;

	if (symbol >= Qt::Key_A && symbol <= Qt::Key_Z) {
		key = (util::key::type) (util::key::small_a + (symbol-Qt::Key_A));
	}
	else {
		switch (symbol) {
			case Qt::Key_0: key = (util::key::number_0); break;
			case Qt::Key_1: key = (util::key::number_1); break;
			case Qt::Key_2: key = (util::key::number_2); break;
			case Qt::Key_3: key = (util::key::number_3); break;
			case Qt::Key_4: key = (util::key::number_4); break;
			case Qt::Key_5: key = (util::key::number_5); break;
			case Qt::Key_6: key = (util::key::number_6); break;
			case Qt::Key_7: key = (util::key::number_7); break;
			case Qt::Key_8: key = (util::key::number_8); break;
			case Qt::Key_9: key = (util::key::number_9); break;

			case Qt::Key_PageDown: key = (util::key::page_down); break;
			case Qt::Key_PageUp  : key = (util::key::page_up); break;

			case Qt::Key_F1 : key = (util::key::f1); break;
			case Qt::Key_F2 : key = (util::key::f2); break;
			case Qt::Key_F3 : key = (util::key::f3); break;
			case Qt::Key_F4 : key = (util::key::f4); break;
			case Qt::Key_F5 : key = (util::key::f5); break;
			case Qt::Key_F6 : key = (util::key::f6); break;
			case Qt::Key_F7 : key = (util::key::f7); break;
			case Qt::Key_F8 : key = (util::key::f8); break;
			case Qt::Key_F9 : key = (util::key::f9); break;
			case Qt::Key_F10: key = (util::key::f10); break;
			case Qt::Key_F11: key = (util::key::f11); break;
			case Qt::Key_F12: key = (util::key::f12); break;

			case Qt::Key_Plus : key = (util::key::plus_sign); break;
			case Qt::Key_Minus: key = (util::key::minus_sign); break;

			case Qt::Key_Asterisk   : key = (util::key::asterisk); break;

			case Qt::Key_Period: key = (util::key::period); break;

			case Qt::Key_Print: key = (util::key::printscreen); break;

			case Qt::Key_Super_L:
			case Qt::Key_Super_R:
				key = (util::key::super); break;
			
			case Qt::Key_Menu : key = (util::key::menu); break;
			case Qt::Key_Down : key = (util::key::cursor_down); break;
			case Qt::Key_Left : key = (util::key::cursor_left); break;
			case Qt::Key_Right: key = (util::key::cursor_right); break;
			case Qt::Key_Up   : key = (util::key::cursor_up); break;

			case Qt::Key_Return: key = (util::key::enter); break;

			case Qt::Key_Backspace: key = (util::key::backspace); break;
			case Qt::Key_Escape: key = (util::key::escape); break;

			case Qt::Key_PowerOff : key = (util::key::power); break;
			case Qt::Key_Pause : key = (util::key::pause); break;

			case Qt::Key_Greater: key = (util::key::greater_than_sign); break;
			case Qt::Key_Less   : key = (util::key::less_than_sign); break;

			default:
				key = (util::key::null);
				break;
		};
	}

	return key;
}

}
}

