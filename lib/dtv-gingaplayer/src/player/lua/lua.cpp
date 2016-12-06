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

#include "lua.h"
#include "../luaplayer.h"

namespace player {
namespace lua {

void storeObject( lua_State *st, void *obj, const char *index ) {
	lua_pushlightuserdata( st, obj );
	lua_setfield( st, LUA_REGISTRYINDEX, index );
}

const char *getField( lua_State *st, int pos, const char *szIndex ) {
	lua_getfield( st, pos, szIndex );
	if (lua_isnil(st, -1)) {
		return NULL;
	}
	return luaL_checkstring( st, -1 );
}

void pushValue( lua_State *st, const std::string &index, int value ) {
	lua_pushnumber( st, value );
	lua_setfield( st, -2, index.c_str() );
}

void pushValue( lua_State *st, const std::string &index, bool value ) {
	lua_pushboolean( st, value ? 1 : 0 );
	lua_setfield( st, -2, index.c_str() );
}

void pushValue( lua_State *st, const std::string &index, const std::string &value ) {
	lua_pushstring( st, value.c_str() );
	lua_setfield( st, -2, index.c_str() );
}

}
}
