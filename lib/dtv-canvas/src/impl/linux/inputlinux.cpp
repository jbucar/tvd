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

#include "inputlinux.h"
#include "../../system.h"
#include "../../inputmanager.h"
#include <util/io/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <util/log.h>
#include <util/keydefs.h>
#include <util/assert.h>
#include <linux/input.h>
#include <fcntl.h>

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

REGISTER_INIT_CONFIG( gui_input_linux ) {
	root().addNode("linux")
		.addValue( "enabled", "LINUX input event enabled", false );
}

namespace canvas {
namespace input {
namespace event {

InputLinux::InputLinux( System *sys )
	: Input( sys )
{
	_isShiftDown=false;
	_isCapsOn=false;
}

InputLinux::~InputLinux() {
}

//	Getter
std::string InputLinux::name() const {
	return "input_event";
}

bool InputLinux::initialize() {
	DTV_ASSERT(_events.empty());
	char filename[32];

	for (int i=0; i<32; i++) {
		snprintf(filename,sizeof(filename), "/dev/input/event%d", i);
		int fd = open( filename, O_RDONLY);
		if (fd < 0) {
			LTRACE("ievent", "Couldn't open input device=%s", filename );
		}
		else {
			if (!hasKeyEvent(fd)) {
				LTRACE("ievent", "Ignoring linux input event device=%s", filename );
				::close(fd);
			}
			else {
				{	//	Show device name
					char buf[32];
					int rc = ioctl(fd,EVIOCGNAME(sizeof(buf)),buf);
					if (rc >= 0) {
						LINFO( "ievent", "Using linux input event: name=%s, device=%s", filename, buf );
					}
				}

				util::id::Ident id = sys()->io()->addIO( fd, boost::bind(&InputLinux::onEvent, this, fd) );
				if (!util::id::isValid(id)) {
					::close(fd);
					LWARN("ievent", "Cannot register connection io");
					return false;
				}

				_events[fd] = id;
			}
		}
	}

	if (_events.empty()) {
		LERROR( "ievent", "No linux input event device available" );
		return false;
	}

	return true;
}

void InputLinux::finalize() {
	LINFO("ievent", "finalize!");
	Events::iterator it = _events.begin();
	while (it != _events.end()) {
		close(it);
		it++;
	}
	_events.clear();
}

void InputLinux::close( Events::iterator &it ) {
	sys()->io()->stopIO( it->second );
	::close(it->first);
}

#define test_bit(bit,array) (array[bit / 8] & (1 << (bit % 8)))
bool InputLinux::hasKeyEvent( int fd ) {
	uint32_t bits[32];
	int rc = ioctl(fd, EVIOCGBIT(0,sizeof(bits)), bits);
	if (rc >= 0) {
		for (int i=0; i< EV_MAX; i++) {
			if (i == EV_KEY && test_bit (i, bits)) {
				return true;
			}
		}
	}
	return false;
}

void InputLinux::onEvent( int fd ) {
	struct input_event ev[64];
	int rb = read(fd, ev, sizeof(ev));
	if (rb < 0) {
		LDEBUG("ievent", "Remove linux input event: fd=%d", fd);
		Events::iterator it = _events.find(fd);
		if (it != _events.end()) {
			close(it);
			_events.erase(it);
		}
	}
	else if (rb < (int) sizeof(struct input_event)) {
		LERROR("ievent", "short read");
	}
	else {
		for (int i=0; i<(int) (rb/sizeof(struct input_event)); i++) {
			if (EV_KEY == ev[i].type) {
				if ((ev[i].value == KEY_PRESS) || (ev[i].value == KEY_RELEASE) || (ev[i].value == KEY_KEEPING_PRESSED)) {
					mapKey( fd, ev[i].code, (ev[i].value == KEY_RELEASE) ? true : false );
				}
			}
		}
	}
}

void InputLinux::mapKey( int fd, int code, bool isUp ) {
	util::key::type key = util::key::null;

	util::BYTE led_map[LED_MAX/8 + 1];
	memset(led_map, 0, sizeof( led_map ));
	ioctl(fd, EVIOCGLED(sizeof( led_map )), led_map);
	_isCapsOn = (( 1<<LED_CAPSL ) & led_map[LED_CAPSL/8] ) ? true : false;
	bool mayus = (( _isShiftDown && !_isCapsOn ) || ( !_isShiftDown && _isCapsOn ));

	if (code >= KEY_1 && code <= KEY_0) {
		if (code == KEY_0) {
			key = util::key::number_0;
		}
		else {
			key = (util::key::type)((code-KEY_1) + util::key::number_1);
		}
	}
	else if (code >= KEY_F1 && code <= KEY_F10) {
		key = (util::key::type)((code-KEY_F1) + util::key::f1);
	}
	else if (code >= KEY_F11 && code <= KEY_F12) {
		key = (util::key::type)((code-KEY_F11) + util::key::f11);
	}
	else {
		switch (code) {
			case KEY_A:
				key = mayus ? util::key::capital_a : util::key::small_a;
				break;
			case KEY_B:
				key = mayus ? util::key::capital_b : util::key::small_b;
				break;
			case KEY_C:
				key = mayus ? util::key::capital_c : util::key::small_c;
				break;
			case KEY_D:
				key = mayus ? util::key::capital_d : util::key::small_d;
				break;
			case KEY_E:
				key = mayus ? util::key::capital_e : util::key::small_e;
				break;
			case KEY_F:
				key = mayus ? util::key::capital_f : util::key::small_f;
				break;
			case KEY_G:
				key = mayus ? util::key::capital_g : util::key::small_g;
				break;
			case KEY_H:
				key = mayus ? util::key::capital_h : util::key::small_h;
				break;
			case KEY_I:
				key = mayus ? util::key::capital_i : util::key::small_i;
				break;
			case KEY_J:
				key = mayus ? util::key::capital_j : util::key::small_j;
				break;
			case KEY_K:
				key = mayus ? util::key::capital_k : util::key::small_k;
				break;
			case KEY_L:
				key = mayus ? util::key::capital_l : util::key::small_l;
				break;
			case KEY_M:
				key = mayus ? util::key::capital_m : util::key::small_m;
				break;
			case KEY_N:
				key = mayus ? util::key::capital_n : util::key::small_n;
				break;
			case KEY_O:
				key = mayus ? util::key::capital_o : util::key::small_o;
				break;
			case KEY_P:
				key = mayus ? util::key::capital_p : util::key::small_p;
				break;
			case KEY_Q:
				key = mayus ? util::key::capital_q : util::key::small_q;
				break;
			case KEY_R:
				key = mayus ? util::key::capital_r : util::key::small_r;
				break;
			case KEY_S:
				key = mayus ? util::key::capital_s : util::key::small_s;
				break;
			case KEY_T:
				key = mayus ? util::key::capital_t : util::key::small_t;
				break;
			case KEY_U:
				key = mayus ? util::key::capital_u : util::key::small_u;
				break;
			case KEY_V:
				key = mayus ? util::key::capital_v : util::key::small_v;
				break;
			case KEY_W:
				key = mayus ? util::key::capital_w : util::key::small_w;
				break;
			case KEY_X:
				key = mayus ? util::key::capital_x : util::key::small_x;
				break;
			case KEY_Y:
				key = mayus ? util::key::capital_y : util::key::small_y;
				break;
			case KEY_Z:
				key = mayus ? util::key::capital_z : util::key::small_z;
				break;
			case KEY_LEFTSHIFT:
			case KEY_RIGHTSHIFT:
				_isShiftDown=!isUp;
				break;
			case KEY_CAPSLOCK:
				break;
			case KEY_PAGEUP:
				key = util::key::page_up;
				break;
			case KEY_PAGEDOWN:
				key = util::key::page_down;
				break;
			case KEY_CHANNELUP:
				key = util::key::channel_up;
				break;
			case KEY_CHANNELDOWN:
				key = util::key::channel_down;
				break;
			case KEY_VOLUMEUP:
				key = util::key::volume_up;
				break;
			case KEY_VOLUMEDOWN:
				key = util::key::volume_down;
				break;
			case KEY_INFO:
				key = util::key::info;
				break;
			case KEY_MENU:
				key = util::key::menu;
				break;
			case KEY_EPG:
				key = util::key::epg;
				break;
			case KEY_KPPLUS:
				key = util::key::plus_sign;
				break;
			case KEY_KPMINUS:
				key = util::key::minus_sign;
				break;
			case KEY_KPASTERISK:
				key = util::key::asterisk;
				break;
			case KEY_SYSRQ:
				key = util::key::printscreen;
				break;
			case KEY_LEFTMETA:
			case KEY_RIGHTMETA:
				key = util::key::super;
				break;
			case KEY_UP:
				key = util::key::cursor_up;
				break;
			case KEY_DOWN:
				key = util::key::cursor_down;
				break;
			case KEY_LEFT:
				key = util::key::cursor_left;
				break;
			case KEY_RIGHT:
				key = util::key::cursor_right;
				break;
			case KEY_ENTER:
			case KEY_KPENTER:
				key = util::key::enter;
				break;
			case KEY_ESC:
				key = util::key::escape;
				break;
			case KEY_BACKSPACE:
				key = util::key::backspace;
				break;
			case KEY_MUTE:
				key = util::key::mute;
				break;
			case KEY_PLAY:
				key = util::key::play;
				break;
			case KEY_STOP:
				key = util::key::stop;
				break;
			case KEY_BACK:
				key = util::key::rewind;
				break;
			case KEY_FORWARD:
				key = util::key::forward;
				break;
			case KEY_RECORD:
				key = util::key::record;
				break;
			case KEY_MODE:
				key = util::key::mode;
				break;
			case KEY_PVR:
				key = util::key::pvr;
				break;
			// case KEY_PROG4://loop
			// 	key = util::key::previous;
			// 	break;
			case KEY_SUBTITLE:
				key = util::key::subtitle;
				break;
			case KEY_AUDIO:
				key = util::key::audio;
				break;
			case KEY_TEXT:
				key = util::key::text;
				break;
			// case KEY_TIME:
			// 	key = util::key::text;
				// 	break;
			case KEY_HOME:
				key = util::key::nav;
				break;
			case KEY_FAVORITES:
				key = util::key::favorites;
				break;
			// case KEY_TV:
			// 	key = util::key::favorites;
				// 	break;
			case KEY_RED:
				key = util::key::red;
					break;
			case KEY_GREEN:
				key = util::key::green;
					break;
			case KEY_YELLOW:
				key = util::key::yellow;
					break;
			case KEY_BLUE:
				key = util::key::blue;
				break;
		}
	}

	if (key != util::key::null) {
		LDEBUG("ievent", "KeyName=%s isUp=%s", util::key::getKeyName(key), isUp ? "true" : "false");
		sys()->input()->dispatchKey( key, isUp );
	} else {
		LINFO("ievent", "Key not mapped (code=%d)", code);
	}
}

}
}
}

