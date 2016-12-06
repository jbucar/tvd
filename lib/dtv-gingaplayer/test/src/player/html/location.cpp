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

#include "htmlplayer.h"
#include "../../util.h"
#include "../../../../src/player/htmlplayer.h"

class HTMLPlayerLocation : public HTMLPlayer {
protected:
    std::string property() const { return "location"; };
};

TEST_F( HTMLPlayerLocation, DISABLED_move_topleft_to_bottomright ) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(620,476,100,100) ));

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_moving") ) );
}

TEST_F( HTMLPlayerLocation, DISABLED_move_topleft_to_bottomright_out_of_bounds ) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_FALSE ( setProperty( _player, "bounds", canvas::Rect(700,500,100,100) ));
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_moving") ) );
}

TEST_F( HTMLPlayerLocation, DISABLED_resize) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(0,0,200,200) ));
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_resizeing") ) );
}

TEST_F( HTMLPlayerLocation, DISABLED_resize_out_of_bounds) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_FALSE ( setProperty( _player, "bounds", canvas::Rect(0,0,800,600) ));
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_moving") ) );
}

TEST_F( HTMLPlayerLocation, DISABLED_move_and_resize) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_TRUE ( setProperty( _player, "bounds", canvas::Rect(520,376,200,200) ));
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("after_moving_and_resizeing") ) );
}

TEST_F( HTMLPlayerLocation, DISABLED_move_and_resize_out_of_bounds) {
	ASSERT_TRUE( setProperty( _player, "src", std::string("file://") + util::getImageName("hello.html") ));
	ASSERT_TRUE( setProperty( _player, "bounds", canvas::Rect(0,0,100,100) ));

	ASSERT_TRUE( play(_player) );
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("before_moving")) );
	
	ASSERT_FALSE ( setProperty( _player, "bounds", canvas::Rect(520,376,300,300) ));
	ASSERT_TRUE ( compareImages( canvas(), getExpectedPath("before_moving") ) );
}

