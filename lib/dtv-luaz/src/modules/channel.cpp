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

#include "channel.h"
#include "../lua.h"
#include <zapper/player.h>
#include <zapper/zapper.h>
#include <canvas/types.h>

namespace luaz {
namespace channel {

namespace impl {
	static zapper::channel::Player *player = NULL;
	static boost::signals2::connection _cChannelBlocked;
}

static void onChannelBlocked( struct lua_State *L, bool blocked ) {
	lua_getfield( L, LUA_GLOBALSINDEX, "zapperCurrentShowBlocked" );
	lua_pushboolean( L, blocked );
	lua_call( L, 1, 0 );
}

static int l_current( lua_State *L ) {
	int ch=impl::player->current();
	lua_pushnumber( L, ch );
	return 1;
}

static int l_currentNetworkName( lua_State *L ) {
	std::string net = impl::player->currentNetworkName();
	lua_pushstring( L, net.c_str() );
	return 1;
}

static int l_status( lua_State *L ) {
	int signal=0, snr=0;
	impl::player->status( signal, snr );
	lua_pushnumber( L, signal );
	lua_pushnumber( L, snr );
	return 2;
}

static int l_change( lua_State *L ) {
	impl::player->change( luaL_checkint(L, 1) );
	return 0;
}

static int l_setFullScreen( lua_State * /*L*/ ) {
	impl::player->setFullScreen();
	return 0;
}

static int l_resize( lua_State *L ) {
	impl::player->resize(
		luaL_checkint(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3),
		luaL_checkint(L, 4) );
	return 0;
}

static int l_mute( lua_State *L ) {
	int toggle = luaL_checkint(L, 1);
	impl::player->mute( toggle ? true : false );
	return 0;
}

static int l_volume( lua_State *L ) {
	int vol = luaL_checkint(L, 1);
	impl::player->volume( vol );
	return 0;
}

static int l_nextVideo( lua_State *L ) {
	int cur=impl::player->nextVideo();
	lua_pushnumber( L, cur );
	return 1;
}

static int l_nextAudio( lua_State *L ) {
	int cur=impl::player->nextAudio();
	lua_pushnumber( L, cur );
	return 1;
}

static int l_nextSubtitle( lua_State *L ) {
	int cur=impl::player->nextSubtitle();
	lua_pushnumber( L, cur );
	return 1;
}

static int l_haveSubtitle( lua_State *L ) {
	int cur=impl::player->subtitleCount();
	if (cur > 0) {
		lua_pushstring( L, "CC" );
	}
	else {
		lua_pushstring( L, "" );
	}
	return 1;
}

static int l_getResolution( lua_State *L ) {
	canvas::mode::type id=impl::player->getResolution();
	canvas::Mode &mode = canvas::mode::get( id );
	lua_pushstring( L, mode.name );
	return 1;
}

static int l_getResolutionMode( lua_State *L ) {
	canvas::mode::type id=impl::player->getResolution();
	canvas::Mode &mode = canvas::mode::get( id );
	lua_pushstring( L, mode.mode );
	return 1;
}

static int l_getAspect( lua_State *L ) {
	canvas::aspect::type id=impl::player->getAspect();
	canvas::Aspect &aspect = canvas::aspect::get( id );
	lua_pushstring( L, aspect.name );
	return 1;
}

static int l_getVideoCount( lua_State *L ) {
	int count = impl::player->videoCount();
	lua_pushnumber( L, count );
	return 1;
}

static int l_getAudio( lua_State *L ) {
	canvas::audio::channel::type id=impl::player->getAudio();
	canvas::audio::Channel &ch = canvas::audio::getChannelInfo( id );
	lua_pushstring( L, ch.name );
	return 1;
}

static int l_getAudioInfo( lua_State *L ) {
	int id = luaL_checkint(L, 1);
	pvr::info::Audio info;
	impl::player->audioInfo( info, id );
	lua_pushstring( L, info.lang.c_str() );
	return 1;
}

static int l_getAudioCount( lua_State *L ) {
	int count = impl::player->audioCount();
	lua_pushnumber( L, count );
	return 1;
}

static int l_next( lua_State *L ) {
	int first  = luaL_checkint(L, 1);
	int factor = luaL_checkint(L, 2);
	int ch=impl::player->nextChannel( first, factor );
	lua_pushnumber( L, ch );
	return 1;
}

static int l_nextFavorite( lua_State *L ) {
	int first  = luaL_checkint(L, 1);
	int factor = luaL_checkint(L, 2);
	int ch=impl::player->nextFavorite( first, factor );
	lua_pushnumber( L, ch );
	return 1;
}

static int l_toggleMobile( lua_State * /*L*/ ) {
	bool show = !impl::player->showMobile();
	impl::player->showMobile(show);
	return 0;
}

static int l_showMobile( lua_State *L ) {
	lua_pushboolean( L, impl::player->showMobile()? 1 : 0 );
	return 1;
}

static const struct luaL_Reg channel_methods[] = {
	{ "current",            l_current            },
	{ "currentNetworkName", l_currentNetworkName },
	{ "status",             l_status             },
	{ "change",             l_change             },
	{ "next",               l_next               },
	{ "nextFavorite",       l_nextFavorite       },
	{ "setFullScreen",      l_setFullScreen      },
	{ "resize",             l_resize             },
	{ "mute",               l_mute               },
	{ "volume",             l_volume             },
	{ "nextVideo",          l_nextVideo          },
	{ "nextAudio",          l_nextAudio          },
	{ "nextSubtitle",       l_nextSubtitle       },
	{ "haveSubtitle",       l_haveSubtitle       },
	{ "getResolution",      l_getResolution      },
	{ "getResolutionMode",  l_getResolutionMode  },
	{ "getAspect",          l_getAspect          },
	{ "getVideoCount",      l_getVideoCount      },
	{ "getAudio",           l_getAudio           },
	{ "getAudioInfo",       l_getAudioInfo       },
	{ "getAudioCount",      l_getAudioCount      },
	{ "toggleMobile",       l_toggleMobile       },
	{ "showMobile",         l_showMobile         },
	{ NULL,                 NULL                 }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::player = zapper->player();
}

void finalize( void ) {
	impl::player = NULL;
}

void start( lua_State *st ) {
	boost::function<void (bool)> fnc = boost::bind(&onChannelBlocked,st,_1);
	impl::_cChannelBlocked = impl::player->onChannelProtected().connect( fnc );
	luaL_register( st, "channel", channel_methods );
}

void stop( void ) {
	impl::_cChannelBlocked.disconnect();
}

}
}

