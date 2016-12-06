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

#include "canvas.h"
#include "surface.h"
#include "../../lua.h"
#include <zapper/zapper.h>
#include <canvas/surface.h>
#include <canvas/canvas.h>
#include <canvas/system.h>
#include <canvas/layer/layer.h>
#include <util/log.h>
#include <util/assert.h>
#include <boost/foreach.hpp>

#define LUA_CANVASLIBNAME "canvas"

namespace luaz{
namespace canvas {

namespace impl {
	static ::canvas::Layer *layer = NULL;
	static ::canvas::Surface *surface = NULL;
	static std::list< ::canvas::Surface * > images;

	static ::canvas::Canvas *canvas() {
		return surface->canvas();
	}
}

static int l_canvasCreateSurface( lua_State *L ) {
	::canvas::Rect bounds;

	if (lua_gettop(L) == 2) {
		bounds.w = luaL_checkint(L, 1);
		bounds.h = luaL_checkint(L, 2);
	} else {
		bounds.x = luaL_checkint(L, 1);
		bounds.y = luaL_checkint(L, 2);
		bounds.w = luaL_checkint(L, 3);
		bounds.h = luaL_checkint(L, 4);
	}

	::canvas::Surface *surface = impl::canvas()->createSurface( bounds );
	DTV_ASSERT(surface);
	surface->setColor( ::canvas::Color(0, 0, 0, 255) );
	impl::images.push_back( surface );

	return pushSurface( L, surface);
}

static int l_canvasCreateSurfaceFromPath( lua_State *L ) {
	std::string img = luaL_checkstring(L, 1);
	::canvas::Surface *surface = impl::canvas()->createSurfaceFromPath(img);
	DTV_ASSERT(surface);

	surface->setColor(::canvas::Color(0, 0, 0, 255));
	impl::images.push_back(surface);

	return pushSurface(L, surface);
}

static int l_canvasFlush( lua_State * /*L*/ ) {
	impl::canvas()->flush();
	return 0;
}

static int l_canvasSize( lua_State * L ) {
	const ::canvas::Size size = impl::canvas()->size();
	lua_pushnumber(L, size.w );
	lua_pushnumber(L, size.h );
	return 2;
}

static int l_canvasDestroySurface( lua_State *L ) {
	::canvas::Surface *surface = checkSurface( L );
	std::list< ::canvas::Surface * >::iterator it;
	it = std::find( impl::images.begin(), impl::images.end(), surface );
	if ( it != impl::images.end() ) {
		impl::canvas()->destroy( *it );
		impl::images.erase( it );
	} else {
		LERROR("canvas", "Surface to destroy not found");
		DTV_ASSERT(false);
	}
	return 0;
}

static int l_zapperZIndex( lua_State *L ) {
	lua_pushnumber( L, impl::surface->getZIndex() );
	return 1;
}

static int l_setTransparency( lua_State *L ) {
	impl::layer->opacity( luaL_checkint(L, 1) );
	return 0;
}

static int l_getTransparency( lua_State *L ) {
	int value=impl::layer->opacity();
	lua_pushnumber( L, value );
	return 1;
}

static const struct luaL_Reg canvas_functions[] = {
	{ "createSurface",         l_canvasCreateSurface           },
	{ "createSurfaceFromPath", l_canvasCreateSurfaceFromPath   },
	{ "flush",                 l_canvasFlush                   },
	{ "size",                  l_canvasSize                    },
	{ "destroy",               l_canvasDestroySurface          },
	{ "zapperZIndex",          l_zapperZIndex                  },
	{ "getTransparency",       l_getTransparency               },
	{ "setTransparency",       l_setTransparency               },
	{ NULL,                    NULL                            }
};

// 	Module methods
bool init( zapper::Zapper *zapper ) {
	impl::layer = zapper->getLayer( zapper::layer::zapper );
	DTV_ASSERT(impl::layer);

	impl::surface = impl::layer->getSurface();
	DTV_ASSERT(impl::surface);

	return true;
}

void fin( void ) {
	LINFO("canvas::Module", "Stop");
	typedef std::list< ::canvas::Surface * >::value_type vt;
	BOOST_FOREACH(vt pair, impl::images) {
		impl::canvas()->destroy(pair);
	}
	impl::images.clear();
}

static int luaopen_canvas(lua_State *L) {
	luaL_openlib(L, LUA_CANVASLIBNAME, canvas_functions, 0);
	return luaopen_surface(L);
}

void start( lua_State *L ) {
	luaopen_canvas(L);
	LINFO("canvas::Module", "Start");
}


}//	End canvas namespace
}//	End luaz namespace
