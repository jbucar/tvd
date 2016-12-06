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

#include "control.h"
#include "../lua.h"
#include <boost/config/no_tr1/complex.hpp>
#include <zapper/zapper.h>
#include <pvr/parental/session.h>
#include <pvr/pvr.h>
#include <util/assert.h>

namespace luaz {
namespace control {

namespace impl {
	static pvr::parental::Session *session = NULL;
	static boost::signals2::connection _cActived;
}

static void onActiveChanged( lua_State *L, bool isActived ) {
	lua_getfield( L, LUA_GLOBALSINDEX, "zapperOnSessionChanged" );
	lua_pushboolean( L, isActived? 1 : 0 );
	lua_call( L, 1, 0 );
}

static int l_setPIN( lua_State *L ) {
	impl::session->pass(luaL_checkstring(L, 1));
	return 0;
}

static int l_resetPIN( lua_State * /*L*/ ) {
	impl::session->pass("");
	return 0;
}

static int l_checkPIN( lua_State *L ) {
	lua_pushboolean(L, impl::session->check(luaL_checkstring(L, 1)));
	return 1;
}

static int l_isSet( lua_State *L ) {
	lua_pushboolean(L, impl::session->isEnabled()? 1 : 0);
	return 1;
}

static int l_isSessionOpened( lua_State *L ) {
	lua_pushboolean(L, impl::session->isBlocked() ? 0 : 1);
	return 1;
}

static int l_sessionExpireTime( lua_State *L ) {
	lua_pushinteger(L, impl::session->timeExpiration());
	return 1;
}

static int l_setExpireTime( lua_State *L ) {
	impl::session->timeExpiration(luaL_checkint(L, 1));
	return 0;
}

static int l_expireSession( lua_State * /*L*/ ) {
	impl::session->expire();
	return 0;
}

static int l_getParentalAge( lua_State *L ) {
	lua_pushnumber(L, impl::session->ageRestricted());
	return 1;
}

static int l_setParentalAge( lua_State *L ) {
	impl::session->restrictAge(luaL_checkint(L, 1));
	return 0;
}

static int l_setDrugsRestriction( lua_State *L ) {
	const bool enabled = lua::checkBool(L, 1);
	impl::session->restrictDrugs(enabled);
	return 0;
}

static int l_setSexRestriction( lua_State *L ) {
	const bool enabled = lua::checkBool(L, 1);
	impl::session->restrictSex(enabled);
	return 0;
}

static int l_setViolenceRestriction( lua_State *L ) {
	const bool enabled = lua::checkBool(L, 1);
	impl::session->restrictViolence(enabled);
	return 0;
}

static int l_isViolenceRestricted( lua_State *L ) {
	lua_pushboolean(L, impl::session->isViolenceRestricted()? 1 : 0);
	return 1;
}

static int l_isDrugsRestricted( lua_State *L ) {
	lua_pushboolean(L, impl::session->isDrugsRestricted()? 1 : 0);
	return 1;
}

static int l_isSexRestricted( lua_State *L ) {
	lua_pushboolean(L, impl::session->isSexRestricted()? 1 : 0);
	return 1;
}

static const struct luaL_Reg control_methods[] = {
	{ "setPIN",                   l_setPIN                   },
	{ "resetPIN",                 l_resetPIN                 },
	{ "checkPIN",                 l_checkPIN                 },
	{ "isSet",                    l_isSet                    },
	{ "isSessionOpened",          l_isSessionOpened          },
	{ "sessionExpireTime",        l_sessionExpireTime        },
	{ "setExpireTime",            l_setExpireTime            },
	{ "expireSession",            l_expireSession            },
	{ "setParentalAge",           l_setParentalAge           },
	{ "getParentalAge",           l_getParentalAge           },
	{ "setDrugsRestriction",      l_setDrugsRestriction      },
	{ "setSexRestriction",        l_setSexRestriction        },
	{ "setViolenceRestriction",   l_setViolenceRestriction   },
	{ "isViolenceRestricted",     l_isViolenceRestricted     },
	{ "isDrugsRestricted",        l_isDrugsRestricted        },
	{ "isSexRestricted",          l_isSexRestricted          },
	{ NULL,                       NULL                       }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::session = zapper->pvr()->session();
}

void finalize( void ) {
	impl::session = NULL;
}

void start( lua_State *st ) {
	boost::function<void (bool)> fnc = boost::bind(&onActiveChanged, st, _1);
	impl::_cActived = impl::session->onActiveChanged().connect( fnc );
	luaL_register( st, "control", control_methods );
}

void stop( void ) {
	impl::_cActived.disconnect();
}

}
}
