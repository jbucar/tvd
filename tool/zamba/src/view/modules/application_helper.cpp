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

#include <luaz/lua.h>
#include <zapper/zapper.h>
#include <canvas/surface.h>
#include <util/log.h>
#include <util/fs.h>
#include <vector>
#include <string.h>

#define LUA_APPHELPERLIBNAME "application_helper"

namespace application_helper {

namespace impl {

static std::string appsPath() {
	return util::fs::make( util::fs::installDataDir(), "apps" );
}

}

static int l_path( lua_State *L ) {
	lua_pushstring(L, impl::appsPath().c_str());
	return 1;
}

static int l_getModuleLoader(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);

	lua_getfield(L, LUA_ENVIRONINDEX, "package");
	lua_getfield(L, -1, "loaders");

	if (!lua_istable(L, -1)) {
		luaL_error(L, LUA_QL("package.loaders") " must be a table");
	}

	lua_pushstring(L, "");  /* error message accumulator */
	for (int i=1; ; i++) {
		lua_rawgeti(L, -2, i);  /* get a loader */
		if (lua_isnil(L, -1)) {
			luaL_error(L, "module " LUA_QS " not found:%s", name, lua_tostring(L, -2));
		}

		lua_getfield(L, LUA_ENVIRONINDEX, "package");
		lua_setfenv(L, -2);
		lua_pushstring(L, name);
		lua_call(L, 1, 1);  /* call it */

		if (lua_isfunction(L, -1)) { /* did it find module? */
			break;  /* module loaded successfully */
		} else if (lua_isstring(L, -1)) { /* loader returned error message? */
			lua_concat(L, 2);  /* accumulate it */
		} else {
			lua_pop(L, 1);
		}
	}

	// return the module loader function
	return 1;
}

static void modinit(lua_State *L, const char *modname) {
	const char *dot;
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "_M");  /* module._M = module */
	lua_pushstring(L, modname);
	lua_setfield(L, -2, "_NAME");

	dot = strrchr(modname, '.');  /* look for last dot in module name */
	if (dot == NULL) {
		dot = modname;
	} else  {
		dot++;
	}

	/* set _PACKAGE as package name (full module name minus last part) */
	lua_pushlstring(L, modname, dot - modname);
	lua_setfield(L, -2, "_PACKAGE");
}

static int l_module(lua_State *L) {
	const char *modname = luaL_checkstring(L, 1);

	// TODO: check this getglobal (¿¿ Use LUA_ENVIRONINDEX?? )
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaders");
	lua_getfield(L, -1, modname);

	if (!lua_istable(L, -1)) {  /* not found? */
		lua_pop(L, 1);  /* remove previous result */

		/* try global variable (and create one if it does not exist) */
		if (luaL_findtable(L, LUA_GLOBALSINDEX, modname, 1) != NULL) {
			return luaL_error(L, "name conflict for module " LUA_QS, modname);
		}

		lua_pushvalue(L, -1);
		lua_setfield(L, -1, modname);
	}

	/* check whether table already has a _NAME field */
	lua_getfield(L, -1, "_NAME");
	if (!lua_isnil(L, -1)) { /* is table an initialized module? */
		lua_pop(L, 1);
	} else {  /* no; initialize it */
		lua_pop(L, 1);
		modinit(L, modname);
	}

	lua_pushvalue(L, -1);

	return 1;
}

static const struct luaL_Reg application_helper_methods[] = {
	{ "path",                   l_path                   },
	{ "get_module_loader",      l_getModuleLoader        },
	{ "module",                 l_module                 },
	{ NULL,                     NULL                     }
};

//	Module methods
void initialize( zapper::Zapper * /*mgr*/ ) {
}

void finalize( void ) {
	LINFO("application_helper::Module", "Stop");
}

void start( lua_State *L ) {
	LINFO("application_helper::Module", "Start");
	luaL_register(L, LUA_APPHELPERLIBNAME, application_helper_methods);
}

void attach( std::vector<std::string> & /*deps*/ ) {}
void stop() {}

}
