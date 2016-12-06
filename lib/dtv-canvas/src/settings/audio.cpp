/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "audio.h"
#include "../audio.h"
#include <util/settings/settings.h>
#include <util/assert.h>

namespace canvas {
namespace settings {

Audio::Audio( util::Settings *settings, canvas::Audio *audio )
	: util::SettingsDelegate(settings), _audio(audio)
{
	DTV_ASSERT(audio);
	_onMuteChanged = _audio->onMuteChanged().connect( boost::bind( &Audio::onMuteChanged, this, _1 ) );
	_onVolumeChanged = _audio->onVolumeChanged().connect( boost::bind( &Audio::onVolumeChanged, this, _1 ) );
	_onChannelChanged = _audio->onChannelChanged().connect( boost::bind( &Audio::onChannelChanged, this, _1 ) );
}

Audio::~Audio()
{
}

void Audio::load() {
	{	//	Load volume
		Volume vol = DEFAULT_VOLUME;
		settings()->get( "audio.volume", vol );
		_audio->volume( vol );
	}

	{	//	Load mute
		bool muted = false;
		settings()->get( "audio.muted", muted );
		_audio->mute( muted );
	}

	{	//	Load channel layout
		int chs = _audio->defaultChannel();
		settings()->get( "audio.audioChannel", chs );
		_audio->channel( (audio::channel::type)chs );
	}
}

void Audio::reset() {
	_audio->volume( DEFAULT_VOLUME );
	if (_audio->mute()) {
		_audio->toggleMute();
	}
	_audio->channel( _audio->defaultChannel() );
}

void Audio::onMuteChanged( bool muted ) {
	settings()->put( "audio.muted", muted );
	settings()->commit();
}

void Audio::onVolumeChanged( canvas::Volume vol ) {
	settings()->put( "audio.volume", vol );
	settings()->commit();
}

void Audio::onChannelChanged( canvas::audio::channel::type chs ) {
	settings()->put( "audio.audioChannel", chs );
	settings()->commit();
}

}
}
