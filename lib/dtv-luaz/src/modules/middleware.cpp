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

#include "middleware.h"
#include "../lua.h"
#include <zapper/middleware/provider.h>
#include <zapper/middleware/application.h>
#include <zapper/middleware/middleware.h>
#include <zapper/zapper.h>
#include <canvas/layer/layer.h>
#include <boost/foreach.hpp>
#include <boost/signals2.hpp>

#define GINGA_MIDDLEWARE "middleware::Ginga.ar"

namespace luaz {
namespace middleware {

namespace impl {
	static zapper::middleware::Middleware *middleware = NULL;
	static lua_State *st=NULL;
	static boost::signals2::connection _cLayer, _cDownload;
}

static void onLayerEnabled( bool isLayerEnabled ) {
	lua::call( impl::st, "zapperApplicationLayerEnabled", isLayerEnabled ? 1 : 0 );
}

static void onDownloadProgress( int /*id*/, int step, int total ) {
	lua::call( impl::st, "zapperApplicationDownloadProgress", step, total );
}

static void pushApplicationInfo( lua_State *L, zapper::middleware::Application *app ) {
	lua_newtable(L);
	lua::setField( L, "id", app->id() );
	lua::setField( L, "name", app->name() );
	lua::setField( L, "icon", app->icon() );
}

static int l_isEnabled( lua_State *L ) {
	bool isEnabled = impl::middleware->isEnabled( GINGA_MIDDLEWARE );
	lua_pushnumber(L, isEnabled ? 1 : 0 );
	return 1;
}

static int l_enable( lua_State *L ) {
	int enable = luaL_checkint(L, 1);
	impl::middleware->enable( GINGA_MIDDLEWARE, enable );
	return 0;
}

static int l_getApplications( lua_State *L ) {
	lua_newtable(L);
	const std::vector<zapper::middleware::Application *> &apps = impl::middleware->getApplications();
	for (size_t i=0; i<apps.size(); i++) {
		//	Key
		lua_pushnumber(L, i+1 );
		//	Value
		pushApplicationInfo( L, apps[i] );
		//	Set to table
		lua_settable(L, -3);
	}
	return 1;
}

static int l_haveApplications( lua_State *L ) {
	int res=0;
	//	Only count attached (from channel) applications
	const std::vector<zapper::middleware::Application *> &apps = impl::middleware->getApplications();
	BOOST_FOREACH( zapper::middleware::Application *info, apps ) {
		if (info->isAttachedToChannel()) {
			res++;
		}
	}
	lua_pushnumber(L, res);
	return 1;
}

static int l_runApplication( lua_State* L ) {
	int id = luaL_checkint(L, 1);
	bool result = impl::middleware->startApplication( id );
	lua_pushboolean(L, result ? 1 : 0);
	return 1;
}

static int l_stopApplication( lua_State* L ) {
	int id = luaL_checkint(L, 1);
	impl::middleware->stopApplication( id );
	return 0;
}

static int l_current( lua_State* L ) {
	bool found=false;
	const std::vector<zapper::middleware::Application*> &apps = impl::middleware->getApplications();
	BOOST_FOREACH(zapper::middleware::Application *app, apps) {
		if (app->isRunning()) {
			pushApplicationInfo( L, app );
			found=true;
			break;
		}
	}
	if (!found) {
		lua_pushnil( L );
	}
	return 1;
}

static int l_getVersion( lua_State* L ) {
	const std::string ver = impl::middleware->version( GINGA_MIDDLEWARE );
	lua_pushstring( L, ver.c_str() );
	return 1;
}

static int l_lock( lua_State* L ) {
	int needLock = (int) luaL_checknumber(L, 1);
	impl::middleware->lockApplications( (needLock == 1) );
	return 0;
}

static const struct luaL_Reg middleware_methods[] = {
	{ "isEnabled",        l_isEnabled        },
	{ "enable",           l_enable           },
	{ "getVersion",       l_getVersion       },
	{ "getApplications",  l_getApplications  },
	{ "haveApplications", l_haveApplications },
	{ "runApplication",   l_runApplication   },
	{ "stopApplication",  l_stopApplication  },
	{ "current",          l_current          },
	{ "lock",             l_lock             },
	{ NULL,               NULL               }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::middleware = zapper->middleware();

	//	Register layer notification
	impl::_cLayer = zapper->getLayer( zapper::layer::middleware )->onEnableChanged().connect( boost::bind(&onLayerEnabled,_1) );

	impl::st = NULL;
}

void finalize() {
}

void start( lua_State *st ) {
	impl::st = st;

	//	Register download notification
	impl::_cDownload = impl::middleware->onDownloadProgress().connect( boost::bind(&onDownloadProgress,_1,_2,_3) );

	luaL_register( st, "middleware", middleware_methods );
}

void stop() {
	impl::st = NULL;
	impl::_cLayer.disconnect();
	impl::_cDownload.disconnect();
}

}
}
