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

#include "videoplayer.h"
#include "../../util.h"
#include "../../../../src/player/videoplayer.h"

class VideoPlayerZIndex : public VideoPlayer {
protected:
    std::string property() const { return "zindex"; };
};

TEST_F( VideoPlayerZIndex, video_overlayed ) {
	ASSERT_TRUE( _player->setProperty( "src", util::getImageName("video.mp4") ));
	ASSERT_TRUE( _player->setProperty( "bounds", canvas::Rect(0,0,100,100) ));

	player::Player* imgPlayer1 = createPlayer( player::type::image );
	ASSERT_TRUE ( imgPlayer1->setProperty( "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( imgPlayer1->setProperty( "bounds", canvas::Rect(0,0,720,576) ) );
	ASSERT_TRUE ( imgPlayer1->setProperty( "fit","fill" ) );
	ASSERT_TRUE ( imgPlayer1->setProperty( "zIndex", 0 ) );
	ASSERT_TRUE ( play(imgPlayer1) );

	player::Player* imgPlayer2 = createPlayer( player::type::image );
	ASSERT_TRUE ( imgPlayer2->setProperty( "src", util::getImageName("red_100_100.jpg") ) );
	ASSERT_TRUE ( imgPlayer2->setProperty( "bounds", canvas::Rect(0,0,360,288) ) );
	ASSERT_TRUE ( imgPlayer2->setProperty( "fit","fill" ) );
	ASSERT_TRUE ( imgPlayer2->setProperty( "zIndex", 1 ) );
	ASSERT_TRUE ( play(imgPlayer2) );

	player::Player* imgPlayer3 = createPlayer( player::type::image );
	ASSERT_TRUE ( imgPlayer3->setProperty( "src", util::getImageName("green.jpg") ) );
	ASSERT_TRUE ( imgPlayer3->setProperty( "bounds", canvas::Rect(360,288,360,288) ) );
	ASSERT_TRUE ( imgPlayer3->setProperty( "fit","fill" ) );
	ASSERT_TRUE ( imgPlayer3->setProperty( "zIndex", 3 ) );
	ASSERT_TRUE ( play(imgPlayer3) );

	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(240,192,240,192) ) );
	ASSERT_TRUE ( _player->setProperty( "zIndex", 2 ) );
	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( compareImages( canvas(),  getExpectedPath("video_overlayed")) );

	destroyPlayer( imgPlayer1 );
	destroyPlayer( imgPlayer2 );
	destroyPlayer( imgPlayer3 );
}
