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

class ImagePlayerBackgroundColor : public ImagePlayer {
protected:
    std::string property() const { return "backgroundColor"; };
};

TEST_F( ImagePlayerBackgroundColor, backgroundColor_white ){
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "white"));
	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("white") ) );

}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_black ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "black"));
	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("black") ) );

}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_silver ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "silver"));
	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("silver") ) );

}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_gray ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "gray"));
	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("gray") ) );

}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_red ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "red"));
	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("red") ) );

}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_maroon ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "maroon"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("maroon") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_fuchsia ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "fuchsia"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("fuchsia") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_purple ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "purple"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("purple") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_lime ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "lime"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("lime") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_green ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "green"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("green") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_yellow ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "yellow"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("yellow") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_olive ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "olive"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("olive") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_blue ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "blue"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("blue") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_navy ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "navy"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("navy") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_aqua ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "aqua"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("aqua") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_teal ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "teal"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("teal") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_transparent ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );
	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "transparent"));

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("transparent") ) );
}

TEST_F( ImagePlayerBackgroundColor, backgroundColor_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "fit","hidden" ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(50,50,500,500) ) );

	ASSERT_TRUE( play(_player) );

	ASSERT_TRUE ( setProperty( _player, "backgroundColor", "white"));
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("white") ) );
}
