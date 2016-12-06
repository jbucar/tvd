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

class SoundPlayerBassLevel : public SoundPlayer {
protected:
    std::string property() const { return "BassLevel"; };
};

TEST_F( SoundPlayerBassLevel, DISABLED_minimum_level) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_TRUE ( _player->setProperty( "bassLevel", 0.0f) );
}

TEST_F( SoundPlayerBassLevel, DISABLED_almost_minimum_level) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_TRUE ( _player->setProperty( "bassLevel", 0.1f) );
}


TEST_F( SoundPlayerBassLevel, DISABLED_over_minimum_level ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_FALSE ( _player->setProperty( "bassLevel", -0.5f) );
}

TEST_F( SoundPlayerBassLevel, DISABLED_medium_level ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_TRUE ( _player->setProperty( "bassLevel", 0.5f) );
}

TEST_F( SoundPlayerBassLevel, DISABLED_max_level ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_TRUE ( _player->setProperty( "bassLevel", 1.0f) );
}

TEST_F( SoundPlayerBassLevel, DISABLED_over_max_level ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_FALSE ( _player->setProperty( "bassLevel", 1.1f) );
}

TEST_F( SoundPlayerBassLevel, DISABLED_almost_max_level ) {
	ASSERT_TRUE ( _player->setProperty( "src", util::getImageName("sound.ogg") ) );

	ASSERT_TRUE ( _player->setProperty( "bassLevel", 0.9f) );
}
