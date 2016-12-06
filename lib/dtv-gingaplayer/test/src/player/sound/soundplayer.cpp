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

#include "soundplayer.h"
#include "../../util.h"
#include "../../../../src/player/soundplayer.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::string SoundPlayer::playerType() const {
    return "SoundPlayer";
}

std::string SoundPlayer::property() const {
    return "basic";
}

player::type::type SoundPlayer::type() const {
	return player::type::audio;
}

TEST_F( SoundPlayer, basic ) {
	ASSERT_FALSE ( _player->isPlaying() );
	ASSERT_FALSE ( _player->isPaused() );

	ASSERT_TRUE ( setProperty( _player, "src",util::getImageName("audio.mp3")) );

	ASSERT_TRUE ( play(_player) );
	ASSERT_TRUE ( _player->isPlaying() );
	ASSERT_FALSE ( _player->isPaused() );

	pause( _player, true );
	ASSERT_TRUE ( _player->isPlaying() );
	ASSERT_TRUE ( _player->isPaused() );

	pause( _player, false );
	ASSERT_TRUE ( _player->isPlaying() );
	ASSERT_FALSE( _player->isPaused() );

	stop( _player );
	ASSERT_FALSE ( _player->isPlaying() );
	ASSERT_FALSE( _player->isPaused() );

	ASSERT_TRUE ( play(_player) );
	pause( _player, true );
	stop(_player);
	ASSERT_FALSE ( _player->isPlaying() );
	ASSERT_FALSE( _player->isPaused() );

	pause( _player, true );
	ASSERT_FALSE( _player->isPaused() );
}

TEST_F( SoundPlayer, invalid_file ) {
	ASSERT_FALSE( setProperty( _player, "src", "Invalid" ) );
}

TEST_F( SoundPlayer, set_src_file ) {
	ASSERT_TRUE( setProperty( _player, "src", util::getImageName("audio.mp3") ) );
}

TEST_F( SoundPlayer, DISABLED_set_src_http ) {
	ASSERT_TRUE( setProperty( _player, "src", "http://path/audio.mp3") );
}

TEST_F( SoundPlayer, DISABLED_set_src_https ) {
	ASSERT_TRUE( setProperty( _player, "src", "https://path/audio.mp3") );
}

TEST_F( SoundPlayer, DISABLED_set_src_rtsp ) {
	ASSERT_TRUE( setProperty( _player, "src", "rtsp://path/audio.mp3") );
}

TEST_F( SoundPlayer, DISABLED_set_src_rtp ) {
	ASSERT_TRUE( setProperty( _player, "src", "rtp://path/audio.mp3") );
}

TEST_F( SoundPlayer, DISABLED_set_src_mirror ) {
	ASSERT_TRUE( setProperty( _player, "src", "ncl-mirror://path/audio.mp3") );
}

TEST_F( SoundPlayer, DISABLED_set_src_sbtvd ) {
	ASSERT_TRUE( setProperty( _player, "src", "sbtvd-ts://path/audio.mp3") );
}

TEST_F( SoundPlayer, set_src_invalid_schema ) {
	ASSERT_FALSE( setProperty( _player, "src", "invalid://path/audio.mp3" ) );
}
