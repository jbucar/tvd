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

#include "middleware.h"
#include "../lua.h"
#include <zapper/update/info.h>
#include <zapper/update/update.h>
#include <zapper/update/provider.h>
#include <zapper/zapper.h>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

namespace luaz {
namespace update {

namespace impl {

static zapper::update::Update *update = NULL;
static lua_State *st=NULL;
static boost::signals2::connection _cUpdateReceived;
static boost::signals2::connection _cDownloadProgress;

static void pushUpdateInfo( lua_State *L, zapper::update::Info *info ) {
	lua_newtable(L);
	lua::setField( L, "id", info->id());
	lua::setField( L, "description", info->description());
}

static std::string makeProviderName( const char *ptr ) {
	std::string tmp = "update::";
	tmp += ptr;
	return tmp;
}

}

static void processUpdate( zapper::update::Info *info ) {
	DTV_ASSERT(impl::st);

	lua_getfield(impl::st, LUA_GLOBALSINDEX, "zapperUpdateReceived");
	impl::pushUpdateInfo(impl::st, info);
	lua_call(impl::st, 1, 0);
}

static void downloadProgress( int step, int total ) {
	DTV_ASSERT(impl::st);
	lua::call( impl::st, "zapperUpdateDownloadProgress", step, total );
}

static int l_enable( lua_State *L ) {
	const char *prov = luaL_checkstring(L,1);
	if (!prov) {
		return luaL_error(L, "Bad argument. The provider argument must be a string.");
	}
	impl::update->enable( impl::makeProviderName(prov), lua::checkBool(L, 2));
	return 0;
}

static int l_isEnabled( lua_State *L ) {
	bool enabled = false;

	if (lua_gettop(L) == 0) {
		enabled = impl::update->isEnabled();
	}
	else {
		const char *prov = luaL_checkstring(L,1);
		if (!prov) {
			return luaL_error(L, "Bad argument. The provider argument must be a string.");
		}
		enabled = impl::update->isEnabled( impl::makeProviderName(prov) );
	}

	lua_pushboolean(L, enabled ? 1 : 0 );
	return 1;
}

static int l_apply( lua_State *L ) {
	impl::update->apply(luaL_checkint(L, 1), lua::checkBool(L, 2));
	return 0;
}

static const struct luaL_Reg update_methods[] = {
	{ "isEnabled", l_isEnabled },
	{ "enable",    l_enable    },
	{ "apply",     l_apply     },
	{ NULL,        NULL        }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::update = zapper->update();
	DTV_ASSERT(impl::update);

}

void finalize() {
	impl::update = NULL;
}

void start( lua_State *st ) {
	impl::st = st;
	//	Connect notifications
	impl::_cUpdateReceived = impl::update->onUpdateReceived().connect( boost::bind( &processUpdate, _1 ) );
	impl::_cDownloadProgress = impl::update->onDownloadProgress().connect( boost::bind( &downloadProgress, _1, _2 ) );
	luaL_register( st, "update", update_methods );
}

void stop() {
	impl::_cUpdateReceived.disconnect();
	impl::_cDownloadProgress.disconnect();
	impl::st = NULL;
}

}
}
