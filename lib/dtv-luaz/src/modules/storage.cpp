/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "storage.h"
#include "../lua.h"
#include <zapper/zapper.h>
#include <util/settings/settings.h>

namespace luaz {
namespace storage {

namespace impl {
	static zapper::Zapper *zapper = NULL;
}

static int l_saveStr( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	std::string value = luaL_checkstring(L, 2);
	impl::zapper->settings()->put( key, value );
	return 0;
}

static int l_loadStr( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	std::string value = luaL_checkstring(L, 2);
	impl::zapper->settings()->get( key, value );
	lua_pushstring( L, value.c_str() );
	return 1;
}

static int l_saveInt( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	int value = luaL_checkint(L, 2);
	impl::zapper->settings()->put( key, value );
	return 0;
}

static int l_loadInt( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	int value = luaL_checkint(L, 2);
	impl::zapper->settings()->get( key, value );
	lua_pushnumber( L, value );
	return 1;
}

static int l_saveBool( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	bool value = luaz::lua::checkBool(L, 2);
	impl::zapper->settings()->put( key, value );
	return 0;
}

static int l_loadBool( lua_State *L ) {
	std::string key = luaL_checkstring(L, 1);
	bool value = luaz::lua::checkBool(L, 2);
	impl::zapper->settings()->get( key, value );
	lua_pushboolean( L, value );
	return 1;
}

static int l_commit( lua_State *L ) {
	bool result = impl::zapper->settings()->commit();
	lua_pushboolean( L, result);
	return 1;
}

static const struct luaL_Reg methods[] = {
	{ "saveStr",     l_saveStr      },
	{ "loadStr",     l_loadStr      },
	{ "saveInt",     l_saveInt      },
	{ "loadInt",     l_loadInt      },
	{ "saveBool",    l_saveBool     },
	{ "loadBool",    l_loadBool     },
	{ "commit",      l_commit       },
	{ NULL,          NULL           }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::zapper = zapper;
}

void finalize() {
	impl::zapper = NULL;
}

void start( lua_State *st ) {
	luaL_register( st, "storage", methods );
}

void stop() {}

}
}
