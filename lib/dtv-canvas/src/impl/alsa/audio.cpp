/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "audio.h"
#include "generated/config.h"
#include <util/assert.h>
#include <util/log.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {
namespace alsa {
	
Audio::Audio() {
	_handle = NULL;
	_element = NULL;
	_oldVolume = 0;
	_oldMute = false;
	_min = 0;
	_max = 0;
}

Audio::~Audio() {
	DTV_ASSERT( !_handle );
	DTV_ASSERT( !_element );
}

bool Audio::init() {
	LDEBUG("alsa", "Init");
	snd_mixer_selem_id_t *sid;

	if (snd_mixer_open( &_handle, 0 ) != 0) {
		LERROR( "alsa", "Failed to open mixer" );
		return false;
	}

	if (snd_mixer_attach( _handle, "default" ) != 0) {
		LERROR( "alsa", "Failed to attach to mixer" );
		closeHandle();
		return false;
	}

	if (snd_mixer_selem_register( _handle, NULL, NULL ) != 0) {
		LERROR( "alsa", "Failed to register mixer" );
		closeHandle();
		return false;
	}

	if (snd_mixer_load( _handle ) != 0) {
		LERROR( "alsa", "Failed to load mixer" );
		closeHandle();
		return false;
	}

	snd_mixer_selem_id_alloca( &sid );
	snd_mixer_selem_id_set_index( sid, 0 );
	snd_mixer_selem_id_set_name( sid, "Master" );
	_element = snd_mixer_find_selem( _handle, sid );

	if (!_element) {
		LERROR( "alsa", "Failed to find mixer element" );
		closeHandle();
		return false;
	}

	snd_mixer_selem_get_playback_volume_range( _element, &_min, &_max );

	return true;
}

void Audio::fin() {
	LDEBUG("alsa", "fin");
	closeHandle();
	_element = NULL;
}

void Audio::closeHandle() {
	snd_mixer_close( _handle );
	_handle = NULL;
}

bool Audio::muteImpl( bool needsMute ) {
	LDEBUG( "alsa", "Set audio mute: needMute=%d", needsMute );
	DTV_ASSERT( _element );
	bool status = false;
	if(!CANVAS_ALSA_MUTE_USE_VOLUME) {
		LDEBUG( "alsa", "Using standard mute" );
		if (snd_mixer_selem_has_playback_switch( _element )) {
			snd_mixer_selem_set_playback_switch_all( _element, needsMute ? 0 : 1 );
			status = true;
		}
	} else {
		LDEBUG( "alsa", "Using mute by volume" );
		Volume vol;
		if (needsMute) {
			vol = MIN_VOLUME;
			_oldVolume = canvas::Audio::volume();
		} else {
			vol = _oldVolume;
		}
		status = volumeImpl( vol );
	}
	return status;
}

bool Audio::volumeImpl( Volume vol ) {
	LDEBUG( "alsa", "Set audio volume: vol=%d", vol );
	DTV_ASSERT( _element );

	long newVol = boost::math::iround( translateVol( vol, _min, _max ) );
	snd_mixer_selem_set_playback_volume_all( _element, newVol );
	return true;
}

bool Audio::channelImpl( audio::channel::type /*channel*/ ) {
	return true;
}

}
}
