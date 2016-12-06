/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "settings.h"
#include "../../../src/audio.h"
#include "../../../src/settings/audio.h"
#include <util/settings/settings.h>
#include <util/assert.h>

class AudioSettingsTest : public SettingsTest {
public:
	AudioSettingsTest() {
		_mixer = NULL;
		_audioSettings = NULL;
	}
protected:
	virtual bool init() {
		DTV_ASSERT( SettingsTest::init() );
	
		_mixer = canvas::Audio::create("dummy");
		_mixer->initialize( NULL );
		_audioSettings = new canvas::settings::Audio( _settings, _mixer );
		return _audioSettings != NULL;
	}
	virtual void fin() {
		SettingsTest::fin();
		DEL(_audioSettings);
		DEL(_mixer);
	}

	canvas::Audio *_mixer;
	canvas::settings::Audio *_audioSettings;
};

TEST_F( AudioSettingsTest, volChange ) {
	ASSERT_TRUE( _mixer->volume( 15 ) );
	canvas::Volume vol;
	_settings->get( "audio.volume", vol );
	ASSERT_EQ( vol, 15);
}

TEST_F( AudioSettingsTest, volChange_invalid ) {
	ASSERT_TRUE( _mixer->volume( 15 ) );
	ASSERT_FALSE( _mixer->volume( 50 ) );
	canvas::Volume vol;
	_settings->get( "audio.volume", vol );
	ASSERT_EQ( vol, 15);
}

TEST_F( AudioSettingsTest, muteChange ) {
	ASSERT_TRUE( _mixer->mute( true ) );
	bool muted;
	_settings->get( "audio.muted", muted );
	ASSERT_TRUE( muted );
}

TEST_F( AudioSettingsTest, channelChange ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::mono ) );
	int channel;
	_settings->get( "audio.audioChannel", channel );
	ASSERT_EQ( channel, canvas::audio::channel::mono );
}

TEST_F( AudioSettingsTest, channelChange_invalid ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::mono ) );
	ASSERT_FALSE( _mixer->channel( canvas::audio::channel::surround ) );
	int channel;
	_settings->get( "audio.audioChannel", channel );
	ASSERT_EQ( channel, canvas::audio::channel::mono );
}

TEST_F( AudioSettingsTest, onLoad ) {
	_settings->put( "audio.volume", 15l );
	_settings->put( "audio.muted", false );
	_settings->put( "audio.audioChannel", canvas::audio::channel::mono );
	_settings->commit();

	_audioSettings->load();

	ASSERT_EQ( _mixer->volume(), 15 );
	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _mixer->channel(), canvas::audio::channel::mono );
}

TEST_F( AudioSettingsTest, onReset ) {
	_settings->put( "audio.volume", 15l );
	_settings->put( "audio.muted", false );
	_settings->put( "audio.audioChannel", canvas::audio::channel::mono );
	_settings->commit();

	_audioSettings->load();

	ASSERT_EQ( _mixer->volume(), 15 );
	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _mixer->channel(), canvas::audio::channel::mono );
	
	_audioSettings->reset();
	
	ASSERT_EQ( _mixer->volume(), DEFAULT_VOLUME );
	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _mixer->channel(), canvas::audio::channel::stereo );
}