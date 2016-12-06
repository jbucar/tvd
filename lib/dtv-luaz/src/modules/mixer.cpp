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

#include "mixer.h"
#include "../lua.h"
#include <zapper/zapper.h>
#include <canvas/types.h>
#include <canvas/audio.h>
#include <canvas/system.h>

namespace luaz {
namespace mixer {

namespace impl {
	typedef std::pair<canvas::audio::channel::type,const char *> ChannelItem;
	typedef std::vector<ChannelItem> ChannelCol;

	static ChannelCol aChannels;
	static canvas::Audio *mixer = NULL;
}

static void buildChannels( void ) {
	impl::aChannels.clear();
	std::vector<canvas::audio::channel::type> channels = impl::mixer->supportedChannels();
	for (size_t i=0; i<channels.size(); i++) {
		canvas::audio::channel::type id = channels[i];
		canvas::audio::Channel &ch = canvas::audio::getChannelInfo( id );
		impl::aChannels.push_back( std::make_pair(id,ch.name) );
	}
}

static int l_toggleMute( lua_State *L ) {
	impl::mixer->toggleMute();
	bool muted = impl::mixer->mute();
	lua_pushnumber( L, muted ? 1 : 0 );
	return 1;
}

static int l_mute( lua_State *L ) {
	bool muted = luaz::lua::checkBool(L, 1);
	impl::mixer->mute( muted );
	return 0;
}

static int l_isMuted( lua_State *L ) {
	bool muted=impl::mixer->mute();
	lua_pushnumber( L, muted ? 1 : 0 );
	return 1;
}

static int l_setVolume( lua_State *L ) {
	int vol = luaL_checkint(L, 1);
	impl::mixer->volume( vol );
	int res = impl::mixer->volume();
	lua_pushnumber( L, res );
	return 1;
}

static int l_volumeUp( lua_State *L ) {
	impl::mixer->volumeUp();
	int vol = impl::mixer->volume();
	lua_pushnumber( L, vol );
	return 1;
}

static int l_volumeDown( lua_State *L ) {
	impl::mixer->volumeDown();
	int vol = impl::mixer->volume();
	lua_pushnumber( L, vol );
	return 1;
}

static int l_maxVolume( lua_State *L ) {
	lua_pushnumber( L, MAX_VOLUME );
	return 1;
}

static int l_getVolume( lua_State *L ) {
	canvas::Volume vol=impl::mixer->volume();
	lua_pushnumber( L, vol );
	return 1;
}

static int l_getAudioChannels( lua_State *L ) {
	return lua::getModes( L, impl::mixer->channel(), impl::aChannels );
}

static int l_getAudioChannel( lua_State *L ) {
	return lua::get( L, impl::mixer->channel(), impl::aChannels );
}

static int l_setAudioChannel( lua_State *L ) {
	impl::mixer->channel( lua::getIndex(L,impl::aChannels) );
	return 0;
}

static const struct luaL_Reg mixer_methods[] = {
	{ "toggleMute",       l_toggleMute       },
	{ "isMuted",          l_isMuted          },
	{ "mute",             l_mute             },
	{ "setVolume",        l_setVolume        },
	{ "getVolume",        l_getVolume        },
	{ "volumeUp",         l_volumeUp         },
	{ "volumeDown",       l_volumeDown       },
	{ "maxVolume",        l_maxVolume        },
	{ "getAudioChannel",  l_getAudioChannel  },
	{ "setAudioChannel",  l_setAudioChannel  },
	{ "getAudioChannels", l_getAudioChannels },
	{ NULL,               NULL               },
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::mixer = zapper->sys()->audio();
	DTV_ASSERT(impl::mixer);
}

void finalize( void ) {
	impl::mixer = NULL;
}

void start( lua_State *st ) {
	buildChannels();
	luaL_register( st, "mixer", mixer_methods );
}

void stop( void ) {
}

}
}
