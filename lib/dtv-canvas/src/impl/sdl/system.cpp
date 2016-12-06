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
#include "canvas.h"
#include "timer.h"
#include <util/log.h>
#include <util/mcr.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>

namespace canvas {
namespace sdl {

namespace impl {
	System *sys = NULL;	//	HACK: We cannot use 2 sdl system
}

System::System()
{
	_canvas = NULL;
}

System::~System()
{
	SDL_Quit();
}

//	Getters
bool System::init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		LERROR("sdl::System", "The initialization of SDL failed: %s", SDL_GetError());
		return false;
	}

	impl::sys = this;
		
	_thread = boost::thread( boost::bind( &System::inputThread, this ) );
	return true;
}

void System::fin() {
	{	//	Stop input thread
		SDL_Event quit_event;
		quit_event.type = SDL_QUIT;
		SDL_PushEvent(&quit_event);
		_thread.join();
	}

	if ( TTF_WasInit() == 1 ) {
		TTF_Quit();
	}

	impl::sys = NULL;
}

//	Timers
canvas::Timer *System::createTimer( const timer::Callback &callback ) {
	return new Timer( callback );
}

void System::inputThread( void ) {
	SDL_Event event;
	bool needExit=false;

	LINFO("sdl::System", "Input thread started");
	while (!needExit) {
		if (!SDL_WaitEvent(&event)) {
			needExit=true;
		}
		else {
			LDEBUG("sdl::System", "Get event: type=%d", event.type);
			
			if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				bool isUp = (event.type == SDL_KEYUP);
				dispatchKey( getKeyCode(event.key.keysym.sym), isUp );
			}
			else if (event.type == SDL_QUIT) {
				needExit=true;
			}
		}
	}

	LINFO("sdl::System", "Input thread stopped");
}

util::key::type System::getKeyCode( int symbol ) {
	util::key::type key;

	switch (symbol) {
		case SDLK_0: key = (util::key::number_0); break;
		case SDLK_1: key = (util::key::number_1); break;
		case SDLK_2: key = (util::key::number_2); break;
		case SDLK_3: key = (util::key::number_3); break;
		case SDLK_4: key = (util::key::number_4); break;
		case SDLK_5: key = (util::key::number_5); break;
		case SDLK_6: key = (util::key::number_6); break;
		case SDLK_7: key = (util::key::number_7); break;
		case SDLK_8: key = (util::key::number_8); break;
		case SDLK_9: key = (util::key::number_9); break;

		case SDLK_a: key = (util::key::small_a); break;
		case SDLK_b: key = (util::key::small_b); break;
		case SDLK_c: key = (util::key::small_c); break;
		case SDLK_d: key = (util::key::small_d); break;
		case SDLK_e: key = (util::key::small_e); break;
		case SDLK_f: key = (util::key::small_f); break;
		case SDLK_g: key = (util::key::small_g); break;
		case SDLK_h: key = (util::key::small_h); break;
		case SDLK_i: key = (util::key::small_i); break;
		case SDLK_j: key = (util::key::small_j); break;
		case SDLK_k: key = (util::key::small_k); break;
		case SDLK_l: key = (util::key::small_l); break;
		case SDLK_m: key = (util::key::small_m); break;
		case SDLK_n: key = (util::key::small_n); break;
		case SDLK_o: key = (util::key::small_o); break;
		case SDLK_p: key = (util::key::small_p); break;
		case SDLK_q: key = (util::key::small_q); break;
		case SDLK_r: key = (util::key::small_r); break;
		case SDLK_s: key = (util::key::small_s); break;
		case SDLK_t: key = (util::key::small_t); break;
		case SDLK_u: key = (util::key::small_u); break;
		case SDLK_v: key = (util::key::small_v); break;
		case SDLK_w: key = (util::key::small_w); break;
		case SDLK_x: key = (util::key::small_x); break;
		case SDLK_y: key = (util::key::small_y); break;
		case SDLK_z: key = (util::key::small_z); break;

			/*                case SDLK_A: key = (util::key::capital_a); break;*/
			//case SDLK_B: key = (util::key::capital_b); break;
			//case SDLK_C: key = (util::key::capital_c); break;
			//case SDLK_D: key = (util::key::capital_d); break;
			//case SDLK_E: key = (util::key::capital_e); break;
			//case SDLK_F: key = (util::key::capital_f); break;
			//case SDLK_G: key = (util::key::capital_g); break;
			//case SDLK_H: key = (util::key::capital_h); break;
			//case SDLK_I: key = (util::key::capital_i); break;
			//case SDLK_J: key = (util::key::capital_j); break;
			//case SDLK_K: key = (util::key::capital_k); break;
			//case SDLK_L: key = (util::key::capital_l); break;
			//case SDLK_M: key = (util::key::capital_m); break;
			//case SDLK_N: key = (util::key::capital_n); break;
			//case SDLK_O: key = (util::key::capital_o); break;
			//case SDLK_P: key = (util::key::capital_p); break;
			//case SDLK_Q: key = (util::key::capital_q); break;
			//case SDLK_R: key = (util::key::capital_r); break;
			//case SDLK_S: key = (util::key::capital_s); break;
			//case SDLK_T: key = (util::key::capital_t); break;
			//case SDLK_U: key = (util::key::capital_u); break;
			//case SDLK_V: key = (util::key::capital_v); break;
			//case SDLK_W: key = (util::key::capital_w); break;
			//case SDLK_X: key = (util::key::capital_x); break;
			//case SDLK_Y: key = (util::key::capital_y); break;
			/*case SDLK_Z: key = (util::key::capital_z); break;*/

		case SDLK_PAGEDOWN: key = (util::key::page_down); break;
		case SDLK_PAGEUP  : key = (util::key::page_up); break;

		case SDLK_F1 : key = (util::key::f1); break;
		case SDLK_F2 : key = (util::key::f2); break;
		case SDLK_F3 : key = (util::key::f3); break;
		case SDLK_F4 : key = (util::key::f4); break;
		case SDLK_F5 : key = (util::key::f5); break;
		case SDLK_F6 : key = (util::key::f6); break;
		case SDLK_F7 : key = (util::key::f7); break;
		case SDLK_F8 : key = (util::key::f8); break;
		case SDLK_F9 : key = (util::key::f9); break;
		case SDLK_F10: key = (util::key::f10); break;
		case SDLK_F11: key = (util::key::f11); break;
		case SDLK_F12: key = (util::key::f12); break;

		case SDLK_PLUS : key = (util::key::plus_sign); break;
		case SDLK_MINUS: key = (util::key::minus_sign); break;

		case SDLK_ASTERISK   : key = (util::key::asterisk); break;
		case SDLK_HASH: key = (util::key::number_sign); break;

		case SDLK_PERIOD: key = (util::key::period); break;

		case SDLK_PRINT: key = (util::key::printscreen); break;
		case SDLK_LSUPER: case SDLK_RSUPER: key = (util::key::super); break;
		case SDLK_MENU : key = (util::key::menu); break;
			//This keys does not exist in SDL
			//case SDLK_INFO : key = (util::key::info); break;
			//case SDLK_EPG  : key = (util::key::epg); break;
			//case SDLK_CHANNEL_DOWN: key = (util::key::channel_down); break;
			//case SDLK_CHANNEL_UP  : key = (util::key::channel_up); break;
			//case SDLK_VOLUME_DOWN: key = (util::key::volume_down); break;
			//case SDLK_VOLUME_UP  : key = (util::key::volume_up); break;
			//case SDLK_OK   : key = (util::key::ok); break;
			//case SDLK_RED   : key = (util::key::red); break;
			//case SDLK_GREEN : key = (util::key::green); break;
			//case SDLK_YELLOW: key = (util::key::yellow); break;
			//case SDLK_BLUE  : key = (util::key::blue); break;
			//case SDLK_CUSTOM0: key = (util::key::tap); break;
			//case SDLK_REWIND: key = (util::key::rewind); break;
			//case SDLK_STOP  : key = (util::key::stop); break;
			//case SDLK_EJECT : key = (util::key::eject); break;
			//case SDLK_PLAY  : key = (util::key::play); break;
			//case SDLK_RECORD: key = (util::key::record); break;
			//case SDLK_BACK  : key = (util::key::back); break;

		case SDL_QUIT  : key = (util::key::exit); break;

		case SDLK_DOWN : key = (util::key::cursor_down); break;
		case SDLK_LEFT : key = (util::key::cursor_left); break;
		case SDLK_RIGHT: key = (util::key::cursor_right); break;
		case SDLK_UP   : key = (util::key::cursor_up); break;

		case SDLK_RETURN: key = (util::key::enter); break;

		case SDLK_BACKSPACE: key = (util::key::backspace); break;
		case SDLK_ESCAPE: key = (util::key::escape); break;

		case SDLK_POWER : key = (util::key::power); break;
		case SDLK_PAUSE : key = (util::key::pause); break;

		case SDLK_GREATER: key = (util::key::greater_than_sign); break;
		case SDLK_LESS   : key = (util::key::less_than_sign); break;

		default:
			key = (util::key::null);
			break;
	};

	return key;
}

}
}
