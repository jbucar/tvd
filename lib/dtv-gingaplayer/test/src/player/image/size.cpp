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

class ImagePlayerSize : public ImagePlayer {
protected:
    std::string property() const { return "size"; };
};

TEST_F( ImagePlayerSize, 100px ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "100px" ) ) );
}

TEST_F( ImagePlayerSize, 100px_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,300,300) ) );	
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "100px" ) ) );
}

TEST_F( ImagePlayerSize, full ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "full" ) ) );
}

TEST_F( ImagePlayerSize, half ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,360,288) ) );
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "half" ) ) );
}

TEST_F( ImagePlayerSize, zero ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_FALSE ( setProperty( _player, "bounds", canvas::Rect(0,0,0,0) ) );
}

TEST_F( ImagePlayerSize, full_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );	
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "full" ) ) );
}

TEST_F( ImagePlayerSize, half_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );		
	ASSERT_TRUE( play(_player) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,360,288) ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "half" ) ) );
}

TEST_F( ImagePlayerSize, zero_dynamic ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ) );	
	play(_player);
	ASSERT_FALSE ( setProperty( _player, "bounds", canvas::Rect(0,0,0,0) ) );
}

TEST_F( ImagePlayerSize, change_bounds ) {
	ASSERT_TRUE ( setProperty( _player, "src", util::getImageName("red_720_576.jpg") ) );
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,476,100,100) ) );
	play(_player);
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(100,100,200,200) ) );
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath( "change_bounds" ) ) );
}