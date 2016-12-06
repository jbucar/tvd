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

#include "player.h"
#include "../lua.h"
#include <zapper/mount/mount.h>
#include <zapper/zapper.h>
#include <canvas/types.h>
#include <canvas/mediaplayer.h>
#include <canvas/player.h>
#include <canvas/system.h>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

namespace luaz {
namespace player {

namespace fs = boost::filesystem;

namespace impl {
	static zapper::Zapper *zapper = NULL;

	static canvas::MediaPlayer *mediaPlayer = NULL;

	static std::vector<std::string> audioFiles;
	static std::vector<std::string> imageFiles;
	static std::vector<std::string> videoFiles;

	static boost::signals2::connection _cStop, _cMount;

	static zapper::mount::Mount *mount() {
		return zapper->mount();
	}
}

static void onStop( lua_State *L ) {
	lua::call( L, "mediaFileEnd" );
}

static void onMount( lua_State *L, const std::string &mountPoint, bool isMounted ) {
	lua_getfield( L, LUA_GLOBALSINDEX, "zapperMountPointChanged" );
	lua_pushstring( L, mountPoint.c_str() );
	lua_pushboolean( L, isMounted? 1 : 0 );
	lua_call( L, 2, 0 );
}

static void pushFileInfo( lua_State *L, int id, const std::string &file ) {
	fs::path p(file);
	lua_newtable(L);
	lua::setField( L, "id", id );
	lua::setField( L, "name", p.filename().string() );
	lua::setField( L, "status", ">" );
}

static int buildFiles( lua_State *L, const std::vector<std::string> &files ) {
	lua_newtable(L);
	for (size_t i=0; i<files.size(); i++) {
		//	Key
		lua_pushnumber(L, i+1 );
		//	Value
		pushFileInfo( L, i, files[i] );
		//	Set to table
		lua_settable(L, -3);
	}
	return 1;
}

static void pushImageFileInfo( lua_State *L, int id, const std::string &file ) {
	fs::path p(file);
	lua_newtable(L);
	lua::setField( L, "id", id );
	lua::setField( L, "name", p.filename().string() );
	lua::setField( L, "path", file );
}

static int buildImageFiles( lua_State *L, const std::vector<std::string> &files ) {
	lua_newtable(L);
	for (size_t i=0; i<files.size(); i++) {
		//	Key
		lua_pushnumber(L, i+1 );
		//	Value
		pushImageFileInfo( L, i, files[i] );
		//	Set to table
		lua_settable(L, -3);
	}
	return 1;
}

static int l_mute( lua_State *L ) {
	bool muted = luaz::lua::checkBool(L, 1);
	impl::mediaPlayer->mute( muted );
	return 0;
}

static int l_setVolume( lua_State *L ) {
	int vol = luaL_checkint(L, 1);
	impl::mediaPlayer->volume( vol );
	return 0;
}

static int l_volumeUp( lua_State *L ) {
	int vol = impl::mediaPlayer->volume();
	if (vol < MAX_VOLUME) {
		vol++;
	}
	impl::mediaPlayer->volume( vol );
	lua_pushnumber( L, vol );
	return 1;
}

static int l_volumeDown( lua_State *L ) {
	int vol = impl::mediaPlayer->volume();
	if (vol > MIN_VOLUME) {
		vol--;
	}
	impl::mediaPlayer->volume( vol );
	lua_pushnumber( L, vol );
	return 1;
}

static int l_maxVolume( lua_State *L ) {
	lua_pushnumber( L, MAX_VOLUME );
	return 1;
}

static int l_getVolume( lua_State *L ) {
	canvas::Volume vol = impl::mediaPlayer->volume();
	lua_pushnumber( L, vol );
	return 1;
}

static int l_getImageFiles( lua_State *L ) {
	impl::imageFiles.clear();
	impl::mount()->getFiles( ".jpg", impl::imageFiles );
	impl::mount()->getFiles( ".png", impl::imageFiles );
	return buildImageFiles( L, impl::imageFiles );
}

static int l_pauseFile( lua_State *L ) {
	int pause = luaL_checkint(L, 1);
	if (pause == 1) {
		impl::mediaPlayer->pause();
	} else {
		impl::mediaPlayer->unpause();
	}
	return 0;
}

static int l_stopFile( lua_State * /*L*/ ) {
	impl::mediaPlayer->stop();
	return 0;
}

static int l_getAudioFiles( lua_State *L ) {
	impl::audioFiles.clear();
	impl::mount()->getFiles( ".mp3", impl::audioFiles );
	return buildFiles( L, impl::audioFiles );
}

static int l_playAudioFile( lua_State *L ) {
	size_t id = luaL_checkint(L, 1);
	if (id < impl::audioFiles.size()) {
		impl::mediaPlayer->play( impl::audioFiles[id] );
	}
	return 0;
}

static int l_getVideoFiles( lua_State *L ) {
	impl::videoFiles.clear();
	impl::mount()->getFiles( ".mp4", impl::videoFiles );
	impl::mount()->getFiles( ".avi", impl::videoFiles );
	impl::mount()->getFiles( ".mkv", impl::videoFiles );
	return buildFiles( L, impl::videoFiles );
}

static int l_playVideoFile( lua_State *L ) {
	size_t id = luaL_checkint(L, 1);
	if (id < impl::videoFiles.size()) {
		impl::mediaPlayer->play( impl::videoFiles[id] );
	}
	return 0;
}

static int l_playFile( lua_State *L ) {
	const char *file = luaL_checkstring(L, 1);
	impl::mediaPlayer->play( file );
	return 0;
}

static const struct luaL_Reg player_methods[] = {
	{ "mute",             l_mute              },
	{ "setVolume",        l_setVolume         },
	{ "getVolume",        l_getVolume         },
	{ "volumeUp",         l_volumeUp          },
	{ "volumeDown",       l_volumeDown        },
	{ "maxVolume",        l_maxVolume         },
	{ "getImageFiles",    l_getImageFiles     },
	{ "getAudioFiles",    l_getAudioFiles     },
	{ "playAudioFile",    l_playAudioFile     },
	{ "getVideoFiles",    l_getVideoFiles     },
	{ "playVideoFile",    l_playVideoFile     },
	{ "playFile",         l_playFile          },
	{ "pauseFile",        l_pauseFile         },
	{ "stopFile",         l_stopFile          },
	{ NULL,               NULL                },
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::zapper = zapper;

	//	Register extensions
	std::set<std::string> exts;
	exts.insert( ".mp3" );
	exts.insert( ".mp4" );
	exts.insert( ".avi" );
	exts.insert( ".mkv" );
	exts.insert( ".jpg" );
	exts.insert( ".png" );
	impl::mount()->registerExtensions( exts );
}

void finalize( void ) {
	impl::_cStop.disconnect();
	impl::_cMount.disconnect();
	impl::zapper->sys()->player()->destroy(impl::mediaPlayer);
}

void start( lua_State *st ) {
	luaL_register( st, "player", player_methods );
	impl::mediaPlayer = impl::zapper->sys()->player()->create( impl::zapper->sys() );
	DTV_ASSERT(impl::mediaPlayer);
	impl::mediaPlayer->setFullScreen();

	{	//	Connect to stop signal
		boost::function<void (void)> fnc = boost::bind(&onStop,st);
		impl::_cStop = impl::mediaPlayer->onStop().connect( fnc );
	}

	{	//	Connect to mount signal
		boost::function<void (const std::string &, bool)> fnc = boost::bind(&onMount, st, _1, _2);
		impl::_cMount = impl::mount()->onMount().connect( fnc );
	}
}

void stop( void ) {
}

}
}

