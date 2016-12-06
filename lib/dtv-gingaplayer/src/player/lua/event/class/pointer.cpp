/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "pointer.h"
#include "../../lua.h"
#include <canvas/system.h>
#include <boost/lexical_cast.hpp>

namespace player {
namespace event {
namespace pointer {

//	Event button types
enum type {
	unknown = 0,
	press,
	release,
	move
};

static type getPointerEventType( const char *name ) {
	if (!strcmp( name, "press" )){
		return press;
	}else if (!strcmp( name, "release" )){
		return release;
	}else if (!strcmp( name, "move" )){
		return move;		
	}else{
		return unknown;
	}
}

int postEvent( lua_State *st, bool isOut, int eventPos ) {
	//	Get event module from stack
	Module *module = Module::get( st );
	if (!module) {
		return luaL_error( st, "[player::event::pointer] Invalid event module" );
	}

	//	Get event type
	const char* value = lua::getField( st, eventPos, "type" );
	if (!value) {
		return luaL_error( st, "[event::pointer] Invalid type field in event" );
	}
	type type = getPointerEventType( value );

	if (type != unknown) {
		//	Get x field
		const char *x = lua::getField( st, eventPos, "x" );
		if (!x) {
			return luaL_error( st, "[event::pointer] Invalid x field in event" );
		}

		//	Get y field
		const char *y = lua::getField( st, eventPos, "y" );
		if (!y) {
			return luaL_error( st, "[event::pointer] Invalid y field in event" );
		}

		if (isOut) {
			//	TODO
			module->dispatchBtn( false, 0, 0 );
		}
		else {
			//	TODO
			dispatchBtn( module, false, 0, 0 );
		}
	} else {
		return luaL_error( st, "[player::event::pointer] Invalid type: type=%s", value );
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchBtn( Module *module, bool isPress, int x, int y ) {
	EventImpl event;
	event["class"] = "pointer";
	event["type"] = isPress ? "press" : "release";	//	TODO move
	event["x"] = boost::lexical_cast<std::string>(x);	// TODO int
	event["y"] = boost::lexical_cast<std::string>(y);	// TODO int
	module->dispatchIn( &event );
}

}
}
}
