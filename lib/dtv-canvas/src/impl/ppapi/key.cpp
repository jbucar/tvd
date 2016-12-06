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

#include "key.h"
#include <util/log.h>

#ifndef _WIN32
//ui/events/keycodes/keyboard_codes_posix.h:
#define VKEY_F1  0x70
#define VKEY_F2  0x71
#define VKEY_F3  0x72
#define VKEY_F4  0x73
#define VKEY_F5  0x74
#define VKEY_F6  0x75
#define VKEY_F7  0x76
#define VKEY_F8  0x77
#define VKEY_F9  0x78
#define VKEY_F10  0x79
#define VKEY_F11  0x7A
#define VKEY_F12  0x7B
#define VKEY_0  0x30
#define VKEY_1  0x31
#define VKEY_2  0x32
#define VKEY_3  0x33
#define VKEY_4  0x34
#define VKEY_5  0x35
#define VKEY_6  0x36
#define VKEY_7  0x37
#define VKEY_8  0x38
#define VKEY_9  0x39
#define VKEY_A  0x41
#define VKEY_B  0x42
#define VKEY_C  0x43
#define VKEY_D  0x44
#define VKEY_E  0x45
#define VKEY_F  0x46
#define VKEY_G  0x47
#define VKEY_H  0x48
#define VKEY_I  0x49
#define VKEY_J  0x4A
#define VKEY_K  0x4B
#define VKEY_L  0x4C
#define VKEY_M  0x4D
#define VKEY_N  0x4E
#define VKEY_O  0x4F
#define VKEY_P  0x50
#define VKEY_Q  0x51
#define VKEY_R  0x52
#define VKEY_S  0x53
#define VKEY_T  0x54
#define VKEY_U  0x55
#define VKEY_V  0x56
#define VKEY_W  0x57
#define VKEY_X  0x58
#define VKEY_Y  0x59
#define VKEY_Z  0x5A
#define VKEY_LEFT  0x25
#define VKEY_UP    0x26
#define VKEY_RIGHT 0x27
#define VKEY_DOWN  0x28
#define VKEY_ADD   0x6B
#define VKEY_SUBTRACT 0x6D
#define VKEY_DECIMAL 0x6E
#define VKEY_DIVIDE 0x6F
#define VKEY_VOLUME_MUTE 0xAD
#define VKEY_VOLUME_DOWN 0xAE
#define VKEY_VOLUME_UP 0xAF
#define VKEY_MENU 0x12
#define VKEY_PAUSE  0x13
#define VKEY_RETURN 0x0D
#define VKEY_ESCAPE 0x1B
#define VKEY_BACK  0x08
#define VKEY_TAB  0x09
#define VKEY_BACKTAB  0x0A
#define VKEY_POWER 0x98
#define VKEY_OEM_103  0xE3  // GTV KEYCODE_MEDIA_REWIND
#define VKEY_MEDIA_NEXT_TRACK  0xB0
#define VKEY_MEDIA_PREV_TRACK  0xB1
#define VKEY_MEDIA_STOP  0xB2
#define VKEY_BROWSER_BACK  0xA6
#define VKEY_BROWSER_FORWARD  0xA7
#define VKEY_BROWSER_REFRESH  0xA8
#define VKEY_BROWSER_STOP  0xA9
#define VKEY_BROWSER_SEARCH  0xAA
#define VKEY_BROWSER_FAVORITES  0xAB
#define VKEY_BROWSER_HOME  0xAC
#define VKEY_MODECHANGE  0x1F
#define VKEY_PRIOR 0x21
#define VKEY_NEXT 0x22
#define VKEY_PLAY 0xFA

// TODO: This codes are hardcoded for microtrol remote control
#define VKEY_NEXT_FAV 0x174
#define VKEY_CHANNEL_LIST 0x18B
#define VKEY_RED 0x193
#define VKEY_GREEN 0x194
#define VKEY_YELLOW 0x195
#define VKEY_BLUE 0x196
#define VKEY_PREVIOUS 0x19D
#define VKEY_CHANNEL_UP 0x1AB
#define VKEY_CHANNEL_DOWN 0x1AC
#define VKEY_EPG 0x1CA
#define VKEY_INFO 0x1C9
#define VKEY_SUBTITLE 0x1CC

#else
#endif

namespace canvas {
namespace ppapi {

util::key::type mapKey( const pp::KeyboardInputEvent &event ) {
	util::key::type key = util::key::null;
	uint32_t code = event.GetKeyCode();

	bool isCapsOn = event.GetModifiers() & PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY;
	bool isShiftOn = event.GetModifiers() & PP_INPUTEVENT_MODIFIER_SHIFTKEY;
	bool mayus = (isShiftOn ^ isCapsOn) ? true : false;

	if (code >= VKEY_F1 && code <= VKEY_F12) {
		key = (util::key::type)((code-VKEY_F1) + util::key::f1);
	}
	else if (code >= VKEY_0 && code <= VKEY_9) {
		key = (util::key::type)((code-VKEY_0) + util::key::number_0);
	}
	else if (code >= VKEY_A && code <= VKEY_Z) {
		key = (util::key::type)((code-VKEY_A) + mayus ? util::key::capital_a : util::key::small_a);
	}
	else if (code == VKEY_PRIOR) {
		key = util::key::page_up;
	}
	else if (code == VKEY_NEXT) {
		key = util::key::page_down;
	}
	else if (code == VKEY_LEFT) {
		key = util::key::cursor_left;
	}
	else if (code == VKEY_UP) {
		key = util::key::cursor_up;
	}
	else if (code == VKEY_RIGHT) {
		key = util::key::cursor_right;
	}
	else if (code == VKEY_DOWN) {
		key = util::key::cursor_down;
	}
	else if (code == VKEY_ADD) {
		key = util::key::plus_sign;
	}
	else if (code == VKEY_SUBTRACT) {
		key = util::key::minus_sign;
	}
	else if (code == VKEY_DECIMAL) {
		key = util::key::period;
	}
	else if (code == VKEY_VOLUME_UP) {
		key = util::key::volume_up;
	}
	else if (code == VKEY_VOLUME_DOWN) {
		key = util::key::volume_down;
	}
	else if (code == VKEY_MENU) {
		key = util::key::menu;
	}
	else if (code == VKEY_RETURN) {
		key = util::key::enter;
	}
	else if (code == VKEY_BACK) {
		key = util::key::back;
	}
	else if (code == VKEY_BACKTAB) {
		key = util::key::backspace;
	}
	else if (code == VKEY_ESCAPE) {
		key = util::key::escape;
	}
	else if (code == VKEY_POWER) {
		key = util::key::power;
	}
	else if (code == VKEY_OEM_103) {
		key = util::key::rewind;
	}
	else if (code == VKEY_BROWSER_FORWARD) {
		key = util::key::forward;
	}
	else if (code == VKEY_MEDIA_STOP) {
		key = util::key::stop;
	}
	else if (code == VKEY_PLAY) {
		key = util::key::play;
	}
	else if (code == VKEY_PAUSE) {
		key = util::key::pause;
	}
	else if (code == VKEY_VOLUME_MUTE) {
		key = util::key::mute;
	}
	else if (code == VKEY_BROWSER_FAVORITES) {
		key = util::key::favorites;
	}
	else if (code == VKEY_BROWSER_BACK) {
		key = util::key::backspace;
	}
	else if (code == VKEY_MODECHANGE) {
		key = util::key::mode;
	}
	else if (code == VKEY_RED) {
		key = util::key::red;
	}
	else if (code == VKEY_GREEN) {
		key = util::key::green;
	}
	else if (code == VKEY_YELLOW) {
		key = util::key::yellow;
	}
	else if (code == VKEY_BLUE) {
		key = util::key::blue;
	}
	else if (code == VKEY_MEDIA_NEXT_TRACK) {
		key = util::key::audio;
	}
	else if (code == VKEY_SUBTITLE) {
		key = util::key::subtitle;
	}
	else if (code == VKEY_INFO) {
		key = util::key::info;
	}
	else if (code == VKEY_CHANNEL_UP) {
		key = util::key::channel_up;
	}
	else if (code == VKEY_CHANNEL_DOWN) {
		key = util::key::channel_down;
	}
	else if (code == VKEY_CHANNEL_LIST) {
		key = util::key::chlist;
	}
	else if (code == VKEY_EPG) {
		key = util::key::epg;
	}
	else if (code == VKEY_PREVIOUS) {
		key = util::key::previous;
	}
	else if (code == VKEY_NEXT_FAV) {
		key = util::key::favorites;
	}
	else {
		LDEBUG( "ppapi", "Cannot map key event: code=%d", event.GetKeyCode() );
	}

	return key;
}

}
}

