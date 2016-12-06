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

#include "key.h"
#include "../../lua.h"
#include "../../../keys.h"
#include <canvas/system.h>

namespace player {
namespace event {
namespace key {

//	Event key types
enum type {
	unknown = 0,
	press,
	release
};

static type getKeyEventType( const char *name ) {
	if (!strcmp( name, "press" )){
		return press;
	}else if (!strcmp( name, "release" )){
		return release;
	}else{
		return unknown;
	}
}

int postEvent( lua_State *st, bool isOut, int eventPos ) {
	//	Get event type
	const char* value = lua::getField( st, eventPos, "type" );
	if (!value) {
		return luaL_error( st, "[event::key] Invalid type field in event" );
	}
	type type = getKeyEventType( value );

	if (type != unknown) {
		//	Get key field
		value = lua::getField( st, eventPos, "key" );
		if (!value) {
			return luaL_error( st, "[event::key] Invalid key field in event" );
		}
		util::key::type key = util::key::getKey( value );

		if (key != util::key::null){
			//	Get event module from stack
			Module *module = Module::get( st );
			if (!module) {
				return luaL_error( st, "[player::event::key] Invalid event module" );
			}

			if (isOut) {
				module->dispatchKey( key, type == release );
			}
			else {
				dispatchKey( module, key, type == release );
			}

		} else {
			return luaL_error( st, "[player::event::key], Invalid key: key=%s", value );
		}

	} else {
		return luaL_error( st, "[player::event::key] Invalid type: type=%s", value );
	}

	lua_pushboolean( st, 1 );
	return 1;
}

void dispatchKey( Module *module, util::key::type key, bool isUp ) {
	EventImpl event;
	event["class"] = "key";
	event["type"] = isUp ? "release" : "press";
	event["key"] = util::key::getKeyName( keys::translateKey( key ) );
	module->dispatchIn( &event );
}

}
}
}
