/******************************************************************************

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

#include "display.h"
#include "../lua.h"
#include <canvas/screen.h>
#include <canvas/system.h>
#include <zapper/zapper.h>

namespace luaz {
namespace display {

namespace impl {

static canvas::Screen *screen = NULL;
typedef std::pair<canvas::connector::type,const char *> ConnItem;
typedef std::pair<canvas::mode::type,const char *> VMItem;
typedef std::pair<canvas::aspect::type,const char *> VAItem;
typedef std::vector<ConnItem> ConnCol;
typedef std::vector<VMItem> VMCol;
typedef std::vector<VAItem> VACol;

static ConnCol connectors;
static VMCol vm[canvas::connector::LAST_CONNECTOR];
static VACol va;
}

#define BUILD_NAMES(Type,vValues,getValuesMethod,getNameMethod)	  \
	{ \
		vValues.clear(); \
		std::vector<Type> values = impl::screen->getValuesMethod; \
		for (size_t i=0; i<values.size(); i++) { \
			Type val = values[i]; \
			vValues.push_back( std::make_pair(val,getNameMethod) ); \
		}\
	}

#define BUILD_VM_NAMES(conn) \
	BUILD_NAMES( canvas::mode::type, impl::vm[conn], supportedModes( conn ), canvas::mode::get(val).name );

static int l_getConnector( lua_State *L ) {
	return lua::get( L, impl::screen->connector(), impl::connectors );
}

static int l_getConnectors( lua_State *L ) {
	return lua::getModes( L, impl::screen->connector(), impl::connectors );
}

//	Video modes
static int l_getVideoMode( lua_State *L ) {
	return lua::get( L, impl::screen->mode(), impl::vm[impl::screen->connector()] );
}

static int l_getVideoModes( lua_State *L ) {
	canvas::connector::type conn=impl::screen->connector();
	return lua::getModes( L, impl::screen->defaultMode(conn), impl::vm[conn] );
}

static int l_listVideoModes( lua_State *L ) {
	canvas::connector::type conn=lua::getIndex(L,impl::connectors);
	canvas::mode::type sel = (conn == impl::screen->connector()) ? impl::screen->mode() : impl::screen->defaultMode(conn);
	return lua::getModes( L, sel, impl::vm[conn] );
}

static int l_setVideoMode( lua_State *L ) {
	canvas::connector::type conn=lua::getIndex(L,impl::connectors);
	canvas::mode::type mode=lua::getIndex(L,impl::vm[conn],2);
	bool ret = impl::screen->mode( conn, mode );
	lua_pushnumber( L, ret ? 1 : 0 );
	return 1;
}

static int l_getAspectMode( lua_State *L ) {
	return lua::get( L, impl::screen->aspect(), impl::va );
}

static int l_getAspectModes( lua_State *L ) {
	return lua::getModes( L, impl::screen->aspect(), impl::va );
}

static int l_setAspectMode( lua_State *L ) {
	impl::screen->aspect( lua::getIndex(L,impl::va) );
	return 0;
}

static const struct luaL_Reg display_methods[] = {
	{ "getConnectors",       l_getConnectors       },
	{ "getConnector",        l_getConnector        },
	{ "getVideoMode",        l_getVideoMode        },
	{ "getVideoModes",       l_getVideoModes       },
	{ "listVideoModes",      l_listVideoModes      },
	{ "setVideoMode",        l_setVideoMode        },
	{ "getAspectModes",      l_getAspectModes      },
	{ "setAspectMode",       l_setAspectMode       },
	{ "getAspectMode",       l_getAspectMode       },
	{ NULL,                  NULL                  }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::screen = zapper->sys()->screen();
}

void finalize( void ) {
	impl::screen = NULL;
}

void start( lua_State *st ) {
	{	//	Build display resolutions
		std::vector<canvas::connector::type> supported=impl::screen->supportedConnectors();
		canvas::connector::type conn;
		for (size_t i=0; i<supported.size(); i++) {
			conn=supported[i];
			BUILD_VM_NAMES( conn );
		}
	}

	BUILD_NAMES( canvas::connector::type, impl::connectors, supportedConnectors(), canvas::connector::name(val) );
	BUILD_NAMES( canvas::aspect::type, impl::va, supportedAspects(), canvas::aspect::get(val).name );
	luaL_register( st, "display", display_methods );
}

void stop( void ) {
}

}
}

