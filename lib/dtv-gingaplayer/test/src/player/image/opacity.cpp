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
#include "../../../../src/device.h"

class ImagePlayerOpacity : public ImagePlayer {
protected:
    std::string property() const { return "opacity"; };
};

TEST_F( ImagePlayerOpacity, opaque ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
 	ASSERT_TRUE ( setProperty( _player, "opacity", 1.0f ));
	ASSERT_TRUE ( setProperty( _player, "zIndex",2 ) );

  	player::Player* player2 = createPlayer( player::type::image );
  	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("image.png") ) );
  	ASSERT_TRUE ( setProperty( player2, "zIndex",1 ) );
  	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(0,0,500,375) ) );

	ASSERT_TRUE( play(_player) );
  	ASSERT_TRUE( play(player2) );


	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "opaque" ) ) );

  	destroyPlayer( player2 );
}

TEST_F( ImagePlayerOpacity, default ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "zIndex",2 ) );

	player::Player* player2 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( player2, "zIndex",1 ) );
	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(0,0,500,375) ) );

	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( play(player2) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "opaque" ) ) );

	destroyPlayer( player2 );
}

TEST_F( ImagePlayerOpacity, opacity_full_transparent ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "zIndex",2 ) );
	ASSERT_TRUE ( setProperty( _player, "opacity", 0.0f ));

	player::Player* player2 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( player2, "zIndex",1 ) );
	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(0,0,500,375) ) );

	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( play(player2) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "full_transparent" ) ) );

	destroyPlayer( player2 );
}

TEST_F( ImagePlayerOpacity, opacity_half_transparent ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "zIndex",2 ) );
	ASSERT_TRUE ( setProperty( _player, "opacity", 0.5f ));

	player::Player* player2 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( player2, "zIndex",1 ) );
	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(0,0,500,375) ) );

	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( play(player2) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "half_transparent" ) ) );

	destroyPlayer( player2 );
}

TEST_F( ImagePlayerOpacity, dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE ( setProperty( _player, "zIndex",2 ) );

	player::Player* player2 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( player2, "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( setProperty( player2, "zIndex",1 ) );
	ASSERT_TRUE ( setProperty( player2, "bounds", canvas::Rect(0,0,500,375) ) );

	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( play(player2) );

	ASSERT_TRUE ( setProperty( _player, "opacity", 0.5f ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "half_transparent" ) ) );

	destroyPlayer( player2 );
}
