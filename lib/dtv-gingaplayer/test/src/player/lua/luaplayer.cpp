/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../../util.h"
#include "luaplayer.h"
#include "../../../../src/player/luaplayer.h"
#include "../../../../src/device.h"
#include "../../../../src/player/settings.h"
#include <util/registrator.h>
#include <util/cfg/cfg.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

void LuaPlayer::SetUp( void ){
	//	Reset configuration
	{
		util::reg::fin();
		util::reg::init();
	}
	//	Load settings configuration
	player::settings::load();

  // Enable lua persistent module by setting a file
  util::cfg::setValue("gingaPlayer.luaPersistence.file", "test_lua_persistent.xml");

	Player::SetUp();
}

std::string LuaPlayer::playerType() const {
    return "LuaPlayer";
}

std::string LuaPlayer::property() const {
    return "basic";
}

std::string LuaPlayer::getLuaPath( std::string folder, std::string file ) const {
    fs::path path( util::getTestRoot() );
    path /= "lua";
    path /= folder;
    path /= property();
    path /= file;

    return path.string();
}

player::type::type LuaPlayer::type() const {
	return player::type::lua;
}

TEST_F( LuaPlayer, basic ) {
    fs::path luapath( util::getTestRoot() );
    luapath /= "lua";
    luapath /= "canvas";
    luapath /= "new1.lua";

    ASSERT_TRUE( _player->setProperty( "src",  luapath.string() ) );
    ASSERT_TRUE( _player->setProperty( "bounds", canvas::Rect(0,0,720, 576) ) );

    ASSERT_TRUE( play(_player) );
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("basic") ) );

    stop(_player);
    ASSERT_TRUE( compareImages( canvas(), "nothing" ) );
}

TEST_F( LuaPlayer, play_after_stop ) {
    fs::path luapath( util::getTestRoot() );
    luapath /= "lua";
    luapath /= "canvas";
    luapath /= "new1.lua";

    ASSERT_TRUE( setProperty( _player, "src",  luapath.string() ) );
    ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720, 576) ) );

    ASSERT_TRUE( play(_player) );
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("basic") ) );

    stop(_player);
    ASSERT_TRUE( compareImages( canvas(), "nothing" ) );

    ASSERT_TRUE( play(_player) );
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("basic") ) );
}

TEST_F( LuaPlayer, set_src_file ) {
    fs::path script( util::getTestRoot() );
    script /= "lua";
    script /= "canvas";
    script /= "new1.lua";

    ASSERT_TRUE( setProperty( _player, "src", script.string() ) );
}

TEST_F( LuaPlayer, set_src_http ) {
    ASSERT_TRUE( setProperty( _player, "src", "http://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_https ) {
    ASSERT_TRUE( setProperty( _player, "src", "https://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_rtsp ) {
    ASSERT_FALSE( setProperty( _player, "src", "rtsp://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_rtp ) {
    ASSERT_FALSE( setProperty( _player, "src", "rtp://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_mirror ) {
    ASSERT_FALSE( setProperty( _player, "src", "ncl-mirror://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_sbtvd ) {
    ASSERT_FALSE( setProperty( _player, "src", "sbtvd-ts://path/script.lua") );
}

TEST_F( LuaPlayer, set_src_invalid_schema ) {
    ASSERT_FALSE( setProperty( _player, "src", "invalid://path/script.lua" ) );
}
