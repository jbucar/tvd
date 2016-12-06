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
#include "impl/dummy/audio.h"
#include "generated/config.h"
#if CANVAS_AUDIO_USE_PULSE
#	include "impl/pulse/audio.h"
#endif
#if CANVAS_AUDIO_USE_ALSA
#	include "impl/alsa/audio.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>

namespace canvas {

#if CANVAS_AUDIO_USE_PULSE
#	define DEFAULT_AUDIO "pulse"
#elif CANVAS_AUDIO_USE_ALSA
#	define DEFAULT_AUDIO "alsa"
#else
#	define DEFAULT_AUDIO "dummy"
#endif

REGISTER_INIT_CONFIG( gui_audio ) {
	root().addNode( "audio" )
		.addValue( "use", "Audio to instance", std::string(DEFAULT_AUDIO) )
		.addValue( "device", "Device to instance", "" );
}

//	Instance creation
Audio *Audio::create( const std::string &param ) {
	const std::string &use = param.empty() ? util::cfg::getValue<std::string>("gui.audio.use") : param;
	LINFO("audio", "Using audio: %s", use.c_str());

#if CANVAS_AUDIO_USE_ALSA
	if (use == "alsa") {
		return new alsa::Audio();
	}
#endif

#if CANVAS_AUDIO_USE_PULSE
	if (use == "pulse") {
		return new pulse::Audio();
	}
#endif

	return new dummy::Audio();
}

Audio::Audio()
{
	_sys = NULL;
	_initialized = false;
	_muted = false;
	_volume = -50;
	_currentChannel = audio::channel::LAST_CHANNEL;
}

Audio::~Audio()
{
}

bool Audio::initialize( System *sys ) {
	DTV_ASSERT( !_initialized );

	_sys = sys;
	if (!init()) {
		return false;
	}

	if (supportMultipleDevices()) {
		if (_devices.empty() || defaultDevice() >= _devices.size()) {
			LERROR("audio", "Cannot initialize audio. Implementation has no output devices" );
			fin();
			return false;
		}

		{	//	Open output device
			std::string dev = util::cfg::getValue<std::string>("gui.audio.device");
			if (dev.empty()) {
				dev = _devices[defaultDevice()].name;
			}
			LINFO( "audio", "Initialize: device=%s", dev.c_str() );
			if (!open( dev )) {
				dumpDevices();
				_devices.clear();
				fin();
				return false;
			}
		}
	}

	_initialized=true;

	muteImpl( _muted );
	channelImpl( _currentChannel );

	return _initialized;
}

void Audio::finalize() {
	LINFO( "audio", "Finalize" );

	if (_initialized) {
		if (supportMultipleDevices()) {
			close();
			_devices.clear();
		}
		fin();
		_initialized=false;
	}
}

bool Audio::init() {
	return true;
}

void Audio::fin() {
}

System *Audio::system() const {
	return _sys;
}

//	Volume
bool Audio::mute( bool needMute ) {
	LDEBUG( "audio", "Set audio mute: needMute=%d", needMute );
	bool result = false;
	if (_muted != needMute) {
		result = muteImpl( needMute );
		if (result) {
			_muted = needMute;
			_onMuteChanged( _muted );
		}
	}
	return result;
}

bool Audio::mute() const {
	return _muted;
}

bool Audio::toggleMute() {
	return mute( !_muted );
}

bool Audio::volume( Volume vol ) {
	LDEBUG( "audio", "Set audio volume: vol=%d", vol );
	bool result = false;
	if (vol != _volume) {
		if(vol >= MIN_VOLUME && vol <= MAX_VOLUME) {
			result = volumeImpl( vol );
			if (result) {
				_volume = vol;
				mute( false );
				_onVolumeChanged( _volume );
			}
		} else {
			LWARN("Audio", "Fail to set volume to: %d. Invalid value", vol);
		}
	}
	return result;
}

Volume Audio::volume() const {
	return _volume;
}

bool Audio::volumeDown() {
	Volume old = _volume;
	if (_volume > MIN_VOLUME) {
		old = (old - STEP_VOLUME < MIN_VOLUME) ? MIN_VOLUME : (old - STEP_VOLUME);
	}
	return volume( old );
}

bool Audio::volumeUp() {
	Volume old = _volume;
	if (_volume < MAX_VOLUME) {
		old = (old + STEP_VOLUME > MAX_VOLUME) ? MAX_VOLUME : (old + STEP_VOLUME);
	}
	return volume( old );
}

void Audio::onDeviceUpdated( Volume vol, bool muted ) {
	if (_volume != vol) {
		_volume = vol;
		_onVolumeChanged( _volume );
	}
	if (_muted != muted) {
		_muted = muted;
		_onMuteChanged( _muted );
	}
}

bool Audio::refresh() {
	bool status = volumeImpl( _volume );
	status &= muteImpl( _muted );
	status &= channelImpl( _currentChannel );
	return status;
}

//	Standby/wakeup
void Audio::standby() {
	muteImpl(true);
}

void Audio::wakeup() {
	refresh();
}

const audio::channel::type& Audio::channel() {
	return _currentChannel;
}

std::vector<audio::channel::type> Audio::supportedChannels() const {
	std::vector<audio::channel::type> tmp;
	tmp.push_back( audio::channel::mono );
	tmp.push_back( audio::channel::stereo );
	return tmp;
}

audio::channel::type Audio::defaultChannel() const {
	return audio::channel::stereo;
}

bool Audio::channel( audio::channel::type channel ) {
	LDEBUG( "audio", "Set audio channel: channel=%d", channel );

	bool result = false;
	if (_currentChannel != channel) {
		if (isChannelSupported( channel )) {
			result = channelImpl( channel );
			if (result) {
				_currentChannel = channel;
				_onChannelChanged( _currentChannel );
			} else {
				LWARN( "audio", "Cannot set audio channel: %d", channel );
			}
		}
		else {
			LWARN( "audio", "Audio channel not supported: %d", channel );
		}
	}

	return result;
}

bool Audio::isChannelSupported( audio::channel::type channel ) {
	std::vector<audio::channel::type> channels = supportedChannels();
	std::vector<audio::channel::type>::const_iterator it=std::find( channels.begin(), channels.end(), channel );
	return it != channels.end();
}

bool Audio::open(const std::string& /*dev*/) {
	return true;
}

void Audio::close() {
}

//	Devices
bool Audio::supportMultipleDevices() {
	return false;
}

const audio::Devices &Audio::devices() {
	return _devices;
}

void Audio::addDevice( const audio::DeviceInfo &dev ) {
	LDEBUG( "audio", "Add audio output device: name=%s", dev.name.c_str() );
	_devices.push_back( dev );
}

void Audio::dumpDevices() {
	if (util::log::canLog( LOG_LEVEL_TRACE, "canvas", "audio" )) {
		LTRACE( "audio", "Devices: default=%s", _devices[defaultDevice()].name.c_str() );
		for (size_t i=0; i<_devices.size(); ++i) {
			LTRACE( "audio", "\tName=%s", _devices[i].name.c_str() );
			LTRACE( "audio", "\t\tDisplay=%s", _devices[i].displayName.c_str() );
			LTRACE( "audio", "\t\tDescription=%s", _devices[i].displayDescription.c_str() );
		}
	}
}

size_t Audio::defaultDevice() const {
	return 0;
}

float Audio::translateVol( Volume vol, float minImpl, float maxImpl ) {
	float relation = (maxImpl - minImpl) / (float)( MAX_VOLUME - MIN_VOLUME );
	return (float)vol * relation + minImpl;
}

ChangedMute &Audio::onMuteChanged() {
	return _onMuteChanged;
}

ChangedVolume &Audio::onVolumeChanged() {
	return _onVolumeChanged;
}

ChangedChannel &Audio::onChannelChanged() {
	return _onChannelChanged;
}

}
