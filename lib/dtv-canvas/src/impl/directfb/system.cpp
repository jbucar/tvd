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
#include "canvas.h"
#include "dfb.h"
#include "../../surface.h"
#include "../../inputmanager.h"
#include <util/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <util/io/dispatcher.h>
#include <util/cfg/configregistrator.h>
#include <boost/bind.hpp>

namespace canvas {
namespace directfb {

REGISTER_INIT_CONFIG( gui_directfb ) {
	root().addNode( "directfb" )
		.addValue( "pixelColorFormat", "Pixel color format", "ARGB" )
		.addValue( "drawBuffer", "Function to draw pixels", (unsigned long)0 )
		.addNode( "colorKey" )
			.addValue( "red", "Red color", (util::BYTE)0 )
			.addValue( "green", "Green color", (util::BYTE)0 )
			.addValue( "blue", "Blue color", (util::BYTE)0 )
			.addValue( "alpha", "Alpha color", (util::BYTE)0 );
}

Color getColorKey() {
	util::cfg::PropertyNode &root = util::cfg::get("gui.directfb.colorKey");
	Color color(
		root.get<util::BYTE>( "red" ),
		root.get<util::BYTE>( "green" ),
		root.get<util::BYTE>( "blue" ),
		root.get<util::BYTE>( "alpha" )
	);
	return color;
}

DFBSurfacePixelFormat getFormat() {
	DFBSurfacePixelFormat fmt = DSPF_ARGB;
	const std::string &color = util::cfg::getValue<std::string>("gui.directfb.pixelColorFormat");
	if (color == "RGB16") {
		fmt=DSPF_RGB16;
	}
	return fmt;
}

System::System()
{
	_dfb = NULL;
	_evtBuffer = NULL;
}

System::~System()
{
}

//	Initialization
bool System::init() {
	//	Initialize DirectFB
	DFBResult err=DirectFBInit(NULL, 0);
	DFB_CHECK( "[canvas::directfb::System] DirectFBInit" );

	//	Create dfb object
	err=DirectFBCreate(&_dfb);
	DFB_CHECK( "[canvas::directfb::System] DirectFBCreate" );

	err = _dfb->CreateInputEventBuffer( _dfb, DICAPS_KEYS, DFB_TRUE, &_evtBuffer );
	DFB_CHECK( "[canvas::directfb::System] CreateInputEventBuffer" );

	int fd;
	err = _evtBuffer->CreateFileDescriptor( _evtBuffer, &fd );
	DFB_CHECK( "[canvas::directfb::System] CreateFileDescriptor" );

	int flags = ::fcntl(fd, F_GETFL, 0);
	::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	//	Start listen of keyboard buffer event
	_evtBufferID = io()->addIO( fd, boost::bind(&System::onKeyEvent,this,fd) );
	if (!util::id::isValid(_evtBufferID)) {
		LERROR("directfb::System", "cannot start fd io event");
		return false;
	}

	return true;
}

void System::fin() {
	//	Stop listen of keyboard buffer event
	io()->stopIO( _evtBufferID );

	DFB_DEL(_evtBuffer);
	DFB_DEL(_dfb);
}

//	Instance creation
canvas::Canvas *System::createCanvas() const {
	return new Canvas( _dfb );
}

canvas::Window *System::createWindow() const {
	return new Window( _dfb );
}

//	Aux keyboard
void System::onKeyEvent( int fd ) {
	DFBEvent buf[23];
	int len=1;

	while (len > 0) {
		len = read( fd, buf, sizeof(buf) );
		if (len > 0) {
			len /= sizeof (DFBEvent);
			for (int i=0; i<len; i++) {
				DFBInputEvent *evt = &buf[i].input;
				if (evt->type == DIET_KEYPRESS || evt->type == DIET_KEYRELEASE) {
					mapKey( evt->key_symbol, evt->type == DIET_KEYRELEASE );
				}
			}
		}
	}
	_evtBuffer->Reset( _evtBuffer );
}

void System::mapKey( int keyID, bool isUp ) {
	util::key::type key;

	switch (keyID) {
		case DIKS_NULL: key = (util::key::null); break;
		case DIKS_0: key = (util::key::number_0); break;
		case DIKS_1: key = (util::key::number_1); break;
		case DIKS_2: key = (util::key::number_2); break;
		case DIKS_3: key = (util::key::number_3); break;
		case DIKS_4: key = (util::key::number_4); break;
		case DIKS_5: key = (util::key::number_5); break;
		case DIKS_6: key = (util::key::number_6); break;
		case DIKS_7: key = (util::key::number_7); break;
		case DIKS_8: key = (util::key::number_8); break;
		case DIKS_9: key = (util::key::number_9); break;

		case DIKS_SMALL_A: key = (util::key::small_a); break;
		case DIKS_SMALL_B: key = (util::key::small_b); break;
		case DIKS_SMALL_C: key = (util::key::small_c); break;
		case DIKS_SMALL_D: key = (util::key::small_d); break;
		case DIKS_SMALL_E: key = (util::key::small_e); break;
		case DIKS_SMALL_F: key = (util::key::small_f); break;
		case DIKS_SMALL_G: key = (util::key::small_g); break;
		case DIKS_SMALL_H: key = (util::key::small_h); break;
		case DIKS_SMALL_I: key = (util::key::small_i); break;
		case DIKS_SMALL_J: key = (util::key::small_j); break;
		case DIKS_SMALL_K: key = (util::key::small_k); break;
		case DIKS_SMALL_L: key = (util::key::small_l); break;
		case DIKS_SMALL_M: key = (util::key::small_m); break;
		case DIKS_SMALL_N: key = (util::key::small_n); break;
		case DIKS_SMALL_O: key = (util::key::small_o); break;
		case DIKS_SMALL_P: key = (util::key::small_p); break;
		case DIKS_SMALL_Q: key = (util::key::small_q); break;
		case DIKS_SMALL_R: key = (util::key::small_r); break;
		case DIKS_SMALL_S: key = (util::key::small_s); break;
		case DIKS_SMALL_T: key = (util::key::small_t); break;
		case DIKS_SMALL_U: key = (util::key::small_u); break;
		case DIKS_SMALL_V: key = (util::key::small_v); break;
		case DIKS_SMALL_W: key = (util::key::small_w); break;
		case DIKS_SMALL_X: key = (util::key::small_x); break;
		case DIKS_SMALL_Y: key = (util::key::small_y); break;
		case DIKS_SMALL_Z: key = (util::key::small_z); break;

		case DIKS_CAPITAL_A: key = (util::key::capital_a); break;
		case DIKS_CAPITAL_B: key = (util::key::capital_b); break;
		case DIKS_CAPITAL_C: key = (util::key::capital_c); break;
		case DIKS_CAPITAL_D: key = (util::key::capital_d); break;
		case DIKS_CAPITAL_E: key = (util::key::capital_e); break;
		case DIKS_CAPITAL_F: key = (util::key::capital_f); break;
		case DIKS_CAPITAL_G: key = (util::key::capital_g); break;
		case DIKS_CAPITAL_H: key = (util::key::capital_h); break;
		case DIKS_CAPITAL_I: key = (util::key::capital_i); break;
		case DIKS_CAPITAL_J: key = (util::key::capital_j); break;
		case DIKS_CAPITAL_K: key = (util::key::capital_k); break;
		case DIKS_CAPITAL_L: key = (util::key::capital_l); break;
		case DIKS_CAPITAL_M: key = (util::key::capital_m); break;
		case DIKS_CAPITAL_N: key = (util::key::capital_n); break;
		case DIKS_CAPITAL_O: key = (util::key::capital_o); break;
		case DIKS_CAPITAL_P: key = (util::key::capital_p); break;
		case DIKS_CAPITAL_Q: key = (util::key::capital_q); break;
		case DIKS_CAPITAL_R: key = (util::key::capital_r); break;
		case DIKS_CAPITAL_S: key = (util::key::capital_s); break;
		case DIKS_CAPITAL_T: key = (util::key::capital_t); break;
		case DIKS_CAPITAL_U: key = (util::key::capital_u); break;
		case DIKS_CAPITAL_V: key = (util::key::capital_v); break;
		case DIKS_CAPITAL_W: key = (util::key::capital_w); break;
		case DIKS_CAPITAL_X: key = (util::key::capital_x); break;
		case DIKS_CAPITAL_Y: key = (util::key::capital_y); break;
		case DIKS_CAPITAL_Z: key = (util::key::capital_z); break;

		case DIKS_PAGE_DOWN: key = (util::key::page_down); break;
		case DIKS_PAGE_UP  : key = (util::key::page_up); break;

		case DIKS_F1 : key = (util::key::f1); break;
		case DIKS_F2 : key = (util::key::f2); break;
		case DIKS_F3 : key = (util::key::f3); break;
		case DIKS_F4 : key = (util::key::f4); break;
		case DIKS_F5 : key = (util::key::f5); break;
		case DIKS_F6 : key = (util::key::f6); break;
		case DIKS_F7 : key = (util::key::f7); break;
		case DIKS_F8 : key = (util::key::f8); break;
		case DIKS_F9 : key = (util::key::f9); break;
		case DIKS_F10: key = (util::key::f10); break;
		case DIKS_F11: key = (util::key::f11); break;
		case DIKS_F12: key = (util::key::f12); break;

		case DIKS_PLUS_SIGN : key = (util::key::plus_sign); break;
		case DIKS_MINUS_SIGN: key = (util::key::minus_sign); break;

		case DIKS_ASTERISK   : key = (util::key::asterisk); break;
		case DIKS_NUMBER_SIGN: key = (util::key::number_sign); break;

		case DIKS_PERIOD: key = (util::key::period); break;

		case DIKS_PRINT: key = (util::key::printscreen); break;
		case DIKS_SUPER: key = (util::key::super); break;
		case DIKS_MENU : key = (util::key::menu); break;
		case DIKS_INFO : key = (util::key::info); break;
		case DIKS_EPG  : key = (util::key::epg); break;

		case DIKS_CURSOR_DOWN : key = (util::key::cursor_down); break;
		case DIKS_CURSOR_LEFT : key = (util::key::cursor_left); break;
		case DIKS_CURSOR_RIGHT: key = (util::key::cursor_right); break;
		case DIKS_CURSOR_UP   : key = (util::key::cursor_up); break;

		case DIKS_CHANNEL_DOWN: key = (util::key::channel_down); break;
		case DIKS_CHANNEL_UP  : key = (util::key::channel_up); break;

		case DIKS_VOLUME_DOWN: key = (util::key::volume_down); break;
		case DIKS_VOLUME_UP  : key = (util::key::volume_up); break;

		case DIKS_ENTER: key = (util::key::enter); break;
		case DIKS_OK   : key = (util::key::ok); break;

		case DIKS_RED   : key = (util::key::red); break;
		case DIKS_GREEN : key = (util::key::green); break;
		case DIKS_YELLOW: key = (util::key::yellow); break;
		case DIKS_BLUE  : key = (util::key::blue); break;

		case DIKS_BACKSPACE: key = (util::key::backspace); break;
		case DIKS_BACK  : key = (util::key::back); break;
		case DIKS_ESCAPE: key = (util::key::escape); break;
		case DIKS_EXIT  : key = (util::key::exit); break;

		case DIKS_POWER : key = (util::key::power); break;
		case DIKS_REWIND: key = (util::key::rewind); break;
		case DIKS_STOP  : key = (util::key::stop); break;
		case DIKS_EJECT : key = (util::key::eject); break;
		case DIKS_PLAY  : key = (util::key::play); break;
		case DIKS_RECORD: key = (util::key::record); break;
		case DIKS_PAUSE : key = (util::key::pause); break;

		case DIKS_GREATER_THAN_SIGN: key = (util::key::greater_than_sign); break;
		case DIKS_LESS_THAN_SIGN   : key = (util::key::less_than_sign); break;

		case DIKS_CUSTOM0: key = (util::key::tap); break;

		case DIKS_SUBTITLE: key = (util::key::subtitle); break;
		case DIKS_AUDIO: key = (util::key::audio); break;
		case DIKS_MUTE: key = (util::key::mute); break;

		case DIKS_PREVIOUS: key = (util::key::previous); break;
			//case DIKS_TEXT: key = (util::key::text); break;
		case DIKS_INTERNET: key = (util::key::favorites); break;

		default: {
			LWARN("directfb::System", "Key symbol not found: %d", keyID );
			key = (util::key::null);
			break;
		}
	};

	if (key != util::key::null) {
		input()->dispatchKey( key, isUp );
	}
}

}
}
