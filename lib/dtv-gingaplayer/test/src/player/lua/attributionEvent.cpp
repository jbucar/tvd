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

#include "luaplayer.h"
#include "../../util.h"
#include "../../../../src/player/luaplayer.h"
#include <boost/filesystem.hpp>
#include <string>

namespace fs = boost::filesystem;

class LuaPlayerAttributionEvent : public LuaPlayer {
protected:
    std::string property() const { return "attribution"; };
};

TEST_F( LuaPlayerAttributionEvent, basic ){

    ASSERT_TRUE( setProperty( _player, "src",  getLuaPath("event", "basic.lua") ) );
    ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720, 576) ) );

    ASSERT_TRUE( play(_player) );

    ASSERT_TRUE( setProperty( _player, "attributionEvent", std::make_pair<std::string, std::string>("basic","1") ) );

    ASSERT_TRUE( compareImages( canvas(), "green" ) );

    stop(_player);
}

TEST_F( LuaPlayerAttributionEvent, before_play ){

    ASSERT_TRUE( setProperty( _player, "src",  getLuaPath("event", "basic.lua") ) );
    ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720, 576) ) );
    ASSERT_FALSE( _player->setProperty( "attributionEvent", std::make_pair<std::string, std::string>("basic","1") ) );
}

TEST_F( LuaPlayerAttributionEvent, after_stop ){

    ASSERT_TRUE( setProperty( _player, "src",  getLuaPath("event", "basic.lua") ) );
    ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,720, 576) ) );

    ASSERT_TRUE( play(_player) );
    stop(_player);

    ASSERT_FALSE( _player->setProperty( "attributionEvent", std::make_pair<std::string, std::string>("basic","1") ) );
}
