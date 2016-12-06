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
#include "st.h"
#include <util/log.h>
#include <util/assert.h>


extern "C" {
#include "stapp_main.h"
}

namespace st {
	
Audio::Audio()
{
	DTV_ASSERT( AUD_Handle[AUDIO_ID] );
	_maxAttenuation = -1;	
}

Audio::~Audio()
{
}

//	Initialization
bool Audio::init() {
	ST_ErrorCode_t err;
	STAUD_Capability_t caps;

	_auds.clear();

	LDEBUG( "st", "Initialize audio subsystem" );
		
	//	Ask for capabilities
	err=STAUD_GetCapability( AUD_DeviceName[AUDIO_ID], &caps );
	if (err != ST_NO_ERROR) {
		LWARN( "st", "Cannot get audio capabilities" );
		return false;
	}
	
	//	Add supported modes
	if ((caps.DRCapability.SupportedStereoModes & STAUD_STEREO_MODE_STEREO) == STAUD_STEREO_MODE_STEREO) {
		_auds.push_back( canvas::audio::channel::stereo );
	}

	if ((caps.DRCapability.SupportedStereoModes & STAUD_STEREO_MODE_MONO) == STAUD_STEREO_MODE_MONO) {
		_auds.push_back( canvas::audio::channel::mono );
	}

	if ((caps.DRCapability.SupportedStereoModes & STAUD_STEREO_MODE_PROLOGIC) == STAUD_STEREO_MODE_PROLOGIC) {
		_auds.push_back( canvas::audio::channel::surround );
	}

	//	Save max attenuation
	_maxAttenuation = caps.PostProcCapability.MaxAttenuation;

	if (_auds.empty()) {
		_auds.push_back( canvas::audio::channel::stereo );
		LWARN("st", "unable to get the capabilities of the audio driver!");
	}

	return true;
}

void Audio::fin() {
}

//	Audio control
bool Audio::muteImpl( bool needMute ) {
	ST_ErrorCode_t err;

	//	Mute/Unmute all devices
	LINFO("st", "mute: needMute=%d", needMute);

	if (needMute) {
#ifdef MT_742PV
		//	PIO_SET B 2
		err=PIO_BIT_Set( (U32)0xB,(U32)0x2,TRUE);
		if (err != ST_NO_ERROR) {
			LWARN("st", "unable to hack audio mute!");
			return false;
		}
#endif
		err=STAUD_Mute( AUD_Handle[AUDIO_ID], TRUE, TRUE );
	}
	else {
#ifdef MT_742PV
		//	PIO_CLEAR B 2
		err=PIO_BIT_Set((U32)0xB,(U32)0x2,FALSE);
		if (err != ST_NO_ERROR) {
			LWARN("st", "unable to hack audio mute!");
			return false;
		}
#endif
		err=STAUD_Mute( AUD_Handle[AUDIO_ID], FALSE, FALSE );
	}

	st::setMuteState( needMute );

	return (err == ST_NO_ERROR);
}

bool Audio::volumeImpl( canvas::Volume vol ) {
	U16 AttenuationdB;

	int db = _maxAttenuation - 20;
	AttenuationdB = (!vol) ? _maxAttenuation : db-((db*vol)/MAX_VOLUME);
	
	LINFO("st", "setVolume: vol=%ld, max=%d, cur=%d", vol, _maxAttenuation, AttenuationdB);
	
	//	Set the volume now
	STAUD_Attenuation_t AUD_Attenuation;
	memset(&AUD_Attenuation,0,sizeof(STAUD_Attenuation_t));
	AUD_Attenuation.Left          = AttenuationdB;
	AUD_Attenuation.Right         = AttenuationdB;
	AUD_Attenuation.LeftSurround  = AttenuationdB;
	AUD_Attenuation.RightSurround = AttenuationdB;
	AUD_Attenuation.Center        = AttenuationdB;
	AUD_Attenuation.Subwoofer     = AttenuationdB;
	ST_ErrorCode_t ErrCode=STAUD_SetAttenuation( AUD_Handle[AUDIO_ID], AUD_Attenuation );
	if (ErrCode!=ST_NO_ERROR) {
		LWARN("st", "unable to set the volume to the audio decoder");
		return false;
	}	
	return true;
}

bool Audio::channelImpl( canvas::audio::channel::type channel ) {
	STAUD_Stereo_t mode;

	LINFO("st", "channelImpl: channels=%d", channel);
	
	switch (channel) {
		case canvas::audio::channel::stereo:
			mode = STAUD_STEREO_STEREO;
			break;
		case canvas::audio::channel::mono:
			mode = STAUD_STEREO_MONO;
			break;
		case canvas::audio::channel::surround:
			mode = STAUD_STEREO_PROLOGIC;
			break;			
		default:
			LWARN("st", "invalid channel mode: mode=%d", channel);
			return false;
	}
	
	//	Set the stereo output mode
	ST_ErrorCode_t ErrCode=STAUD_SetStereoOutput(AUD_Handle[AUDIO_ID],mode);
	if (ErrCode!=ST_NO_ERROR) {
		LWARN("st", "unable to set the stereo output mode to the audio decoder");
		return false;
	}
	
	return true;
}

std::vector<canvas::audio::channel::type> Audio::supportedChannels() const {
	return _auds;	
}

canvas::audio::channel::type Audio::defaultChannel() const {
	return _auds[0];
}

}
	
