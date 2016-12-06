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
#include "../../../../src/system.h"
#include <canvas/canvas.h>

class ImagePlayerLocation : public ImagePlayer {
protected:
    std::string property() const { return "location"; };
};

TEST_F( ImagePlayerLocation, top_left ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "top_left" ) ) );
}

TEST_F( ImagePlayerLocation, top_right ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(620,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "top_right" ) ) );
}

TEST_F( ImagePlayerLocation, bottom_left ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(0,476,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "bottom_left" ) ) );
}

TEST_F( ImagePlayerLocation, bottom_right ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(620,476,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "bottom_right" ) ) );
}

TEST_F( ImagePlayerLocation, middle ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(310,238,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "middle" ) ) );
}

TEST_F( ImagePlayerLocation, top_left_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(310,238,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "top_left" ) ) );
}

TEST_F( ImagePlayerLocation, top_right_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,0,100,100) ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "top_right" ) ) );
}

TEST_F( ImagePlayerLocation, bottom_left_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,476,100,100) ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "bottom_left" ) ) );
}

TEST_F( ImagePlayerLocation, bottom_right_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(620,476,100,100) ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "bottom_right" ) ) );
}

TEST_F( ImagePlayerLocation, middle_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(310,238,100,100) ));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "middle" ) ) );
}

TEST_F( ImagePlayerLocation, three_players ) {
	canvas()->beginDraw();

	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("blue.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds",canvas::Rect(0,0,360,288) ));
	ASSERT_TRUE( play( _player ) );

	player::Player* _player2 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( _player2, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player2, "bounds",canvas::Rect(0,288,360,288) ));
	ASSERT_TRUE( play( _player2 ) );

	player::Player* _player3 = createPlayer( player::type::image );
	ASSERT_TRUE ( setProperty( _player3, "src", util::getImageName("yellow.jpg") ) );
	ASSERT_TRUE ( setProperty( _player3, "bounds",canvas::Rect(360,0,360,288) ));
	ASSERT_TRUE( play( _player3 )  );

	endDraw();

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "blue_red_yellow" ) ) );

	destroyPlayer( _player2 );
	destroyPlayer( _player3 );
}
