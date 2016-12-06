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
#include <canvas/rect.h>
#include <canvas/point.h>

std::string ImagePlayer::playerType() const {
    return "ImagePlayer";
}

std::string ImagePlayer::property() const {
    return "basic";
}

player::type::type ImagePlayer::type() const {
	return player::type::image;
}

TEST_F( ImagePlayer, basic ) {
    ASSERT_FALSE ( _player->isPlaying() );
    ASSERT_FALSE ( _player->isPaused() );

    ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,300) ) );
    ASSERT_TRUE ( _player->setProperty("src",util::getImageName("image.png")) );
    ASSERT_TRUE ( play(_player) );
    ASSERT_TRUE ( _player->isPlaying() );
    ASSERT_FALSE ( _player->isPaused() );

    _player->pause( true );
    ASSERT_TRUE ( _player->isPlaying() );
    ASSERT_TRUE ( _player->isPaused() );

    _player->pause( false );
    ASSERT_TRUE ( _player->isPlaying() );
    ASSERT_FALSE( _player->isPaused() );

    stop(_player);
    ASSERT_FALSE ( _player->isPlaying() );
    ASSERT_FALSE( _player->isPaused() );

    ASSERT_TRUE ( play(_player) );
    _player->pause( true );
    stop(_player);
    ASSERT_FALSE ( _player->isPlaying() );
    ASSERT_FALSE( _player->isPaused() );

    _player->pause( true );
    ASSERT_FALSE( _player->isPaused() );
}

TEST_F( ImagePlayer, play ) {
    ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,375) ) );
	play(_player);
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("play") ) );
}

TEST_F( ImagePlayer, image_name_with_accent_character ) {
    ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("imáge.png") ) );
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,375) ) );
	play(_player);
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("play") ) );
}

TEST_F( ImagePlayer, stop ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("image.png") ) );
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,375) ) );
	play(_player);
	stop(_player);
	ASSERT_TRUE( compareImages( canvas(), getExpectedPath("stopped") ) );
}

TEST_F( ImagePlayer, invalid_file ) {
	ASSERT_FALSE ( _player->setProperty( "src",util::getImageName("invalid.png" )) );
	ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,375) ) );
	ASSERT_FALSE( play(_player) );
}

TEST_F( ImagePlayer, play_after_stop ) {
    ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("image.png") ) );
    ASSERT_TRUE ( _player->setProperty( "bounds", canvas::Rect(0,0,500,375) ) );
    ASSERT_TRUE( play(_player) );
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("play") ) );

    stop(_player);
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("stopped") ) );

    ASSERT_TRUE( _player->setProperty( "bounds", canvas::Rect(0,0,500, 375) ) );
    ASSERT_TRUE( play(_player));
    ASSERT_TRUE( compareImages( canvas(), getExpectedPath("play") ) );
}

TEST_F( ImagePlayer, set_src_file ) {
    ASSERT_TRUE( _player->setProperty( "src", util::getImageName("image.png") ) );
}

TEST_F( ImagePlayer, set_src_http ) {
    ASSERT_TRUE( _player->setProperty( "src", "http://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_https ) {
    ASSERT_TRUE( _player->setProperty( "src", "https://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_rtsp ) {
    ASSERT_FALSE( _player->setProperty( "src", "rtsp://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_rtp ) {
    ASSERT_FALSE( _player->setProperty( "src", "rtp://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_mirror ) {
    ASSERT_FALSE( _player->setProperty( "src", "ncl-mirror://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_sbtvd ) {
    ASSERT_FALSE( _player->setProperty( "src", "sbtvd-ts://path/image.png" ) );
}

TEST_F( ImagePlayer, set_src_invalid_schema ) {
    ASSERT_FALSE( _player->setProperty( "src", "invalid://path/image.png" ) );
}
