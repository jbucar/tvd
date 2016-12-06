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
#include "soundproperties.h"
#include "../device.h"
#include <canvas/mediaplayer.h>
#include <util/mcr.h>

namespace player {

SoundPlayer::SoundPlayer( Device *dev )
	: Player( dev )
{
	_media = dev->createMediaPlayer();
	_sound = new SoundProperties( _media );

	_media->onStop().connect( boost::bind(&SoundPlayer::stop,this) );
}

SoundPlayer::~SoundPlayer()
{
	delete _sound;
	device()->destroy(_media);
}

bool SoundPlayer::startPlay() {
	return _media->play((schema() == schema::file) ? body() : url());
}

void SoundPlayer::stopPlay() {
	_media->stop();
}

void SoundPlayer::pausePlay( bool needPause ) {
	if (needPause) {
		_media->pause();
	} else {
		_media->unpause();
	}
}

bool SoundPlayer::supportSchemma( schema::type sch ) const {
	return sch == schema::file || sch == schema::rtp || sch == schema::rtsp;
}

void SoundPlayer::registerProperties() {
	Player::registerProperties();
	_sound->registerProperties(this);
}

}
