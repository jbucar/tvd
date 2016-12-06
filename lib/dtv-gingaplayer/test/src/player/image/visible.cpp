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

#include "imageplayer.h"
#include "../../util.h"
#include "../../../../src/player/imageplayer.h"

class ImagePlayerVisible : public ImagePlayer {
protected:
    std::string property() const { return "visible"; };
};

TEST_F( ImagePlayerVisible, player_visible ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "visible",true ) );
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,500,375) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "player_visible" ) ) );
}

TEST_F( ImagePlayerVisible, player_invisible ) { // Same as visible_fill_default, but visible = false
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( _player, "visible",false ) );
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,500,375) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "player_invisible" ) ) );
}

TEST_F( ImagePlayerVisible, player_visible_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,500,375) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "visible",true ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "player_visible" ) ) );
}

TEST_F( ImagePlayerVisible, player_invisible_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(100,100,500,375) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "visible",false ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "player_invisible" ) ) );
}
