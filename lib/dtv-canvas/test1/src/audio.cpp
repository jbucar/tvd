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
#include "../../src/types.h"
#include "../../src/system.h"
#include "../../src/audio.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/signals2.hpp>

AudioTest::AudioTest()
{
	_mixer = NULL;
	_testData.nMute = 0;
	_testData.nVol = 0;
	_testData.nChannel = 0;
	_testData.nOpen = 0;
	_testData.nClose = 0;
	_testData.vol = 0.0;
	_testData.nMuteSignals = 0;
	_testData.nVolSignals = 0;
	_testData.nChannelSignals = 0;
}

AudioTest::~AudioTest()
{
	DTV_ASSERT(!_mixer);
}

bool AudioTest::init() {
	util::log::flush();
	_mixer = createAudio();
	return _mixer != NULL;
}

void AudioTest::fin() {
	DEL(_mixer);
	util::log::flush();
}

canvas::Audio *AudioTest::createAudio() {
	return canvas::Audio::create();
}

void AudioTest::SetUp() {
	init();
	_mixer->initialize( NULL );
	_onMuteChanged = _mixer->onMuteChanged().connect( boost::bind( &AudioTest::onMuteChanged, this, _1 ) );
	_onVolumeChanged = _mixer->onVolumeChanged().connect( boost::bind( &AudioTest::onVolumeChanged, this, _1 ) );
	_onChannelChanged = _mixer->onChannelChanged().connect( boost::bind( &AudioTest::onChannelChanged, this, _1 ) );
}

void AudioTest::TearDown() {
	_mixer->finalize();
	_onMuteChanged.disconnect();
	_onVolumeChanged.disconnect();
	_onChannelChanged.disconnect();
	fin();
}

TEST_F( AudioFailInitializationTest, initialize ) {
	ASSERT_FALSE( _mixer->initialize( NULL ) );
}

TEST_F( AudioOkInitializationTest, initialize ) {
	ASSERT_TRUE( _mixer->initialize( NULL ) );
	ASSERT_EQ( _testData.nOpen, 1 );
}

TEST_F( AudioOkInitializationTestNoDevices, initialize ) {
	ASSERT_TRUE( _mixer->initialize( NULL ) );
	ASSERT_EQ( _testData.nOpen, 0 );
}

TEST_F( AudioOkInitializationTest, finalize ) {
	ASSERT_TRUE( _mixer->initialize( NULL ) );
	ASSERT_EQ( _testData.nOpen, 1 );
	_mixer->finalize();
	ASSERT_EQ( _testData.nClose, 1 );
}

TEST_F( AudioOkInitializationTestNoDevices, finalize ) {
	ASSERT_TRUE( _mixer->initialize( NULL ) );
	ASSERT_EQ( _testData.nOpen, 0 );
	_mixer->finalize();
	ASSERT_EQ( _testData.nClose, 0 );
}

TEST_F( AudioOkTest, setVolume ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioOkTest, setVolume2 ) {
	ASSERT_TRUE( _mixer->volume( MIN_VOLUME ) );
	ASSERT_EQ( _mixer->volume(), MIN_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioOkTest, setVolume_invalid ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_FALSE( _mixer->volume( MAX_VOLUME * 2 ) );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioOkTest, setVolume_invalid2 ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_FALSE( _mixer->volume( -10 ) );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioOkTest, volUp ) {
	ASSERT_TRUE( _mixer->volume( 1 ) );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_TRUE( _mixer->volumeUp() );
	ASSERT_EQ( _mixer->volume(), 2 );
	ASSERT_EQ( _testData.nVol, 2 );
	ASSERT_EQ( _testData.nVolSignals, 2 );
}

TEST_F( AudioOkTest, volDown ) {
	ASSERT_TRUE( _mixer->volume( 9 ) );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_TRUE( _mixer->volumeDown() );
	ASSERT_EQ( _mixer->volume(), 8 );
	ASSERT_EQ( _testData.nVol, 2 );
	ASSERT_EQ( _testData.nVolSignals, 2 );
}

TEST_F( AudioOkTest, volUp_outOfBounds ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_FALSE( _mixer->volumeUp() );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioOkTest, volDown_outOfBounds ) {
	ASSERT_TRUE( _mixer->volume( MIN_VOLUME ) );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_FALSE( _mixer->volumeDown() );
	ASSERT_EQ( _mixer->volume(), MIN_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioVolChangeFailTest, setVolume ) {
	ASSERT_FALSE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_NE( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 0 );
}

TEST_F( AudioOkTest, mute_twice ) {
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_FALSE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->mute() );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_EQ( _testData.nMuteSignals, 1 );
}

TEST_F( AudioOkTest, mute_unmute ) {
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_TRUE( _mixer->mute( false ) );
	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _testData.nMute, -1 );
	ASSERT_EQ( _testData.nMuteSignals, 2 );
}

TEST_F( AudioOkTest, mute_unmute2 ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_TRUE( _mixer->mute( false ) );
	ASSERT_EQ( _testData.nMute, -1 );
	ASSERT_EQ( _mixer->volume(), MAX_VOLUME );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nMuteSignals, 2 );
}

TEST_F( AudioOkTest, mute_volUp ) {
	ASSERT_TRUE( _mixer->volume( 1 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_TRUE( _mixer->volumeUp() );
	ASSERT_EQ( _testData.nMute, -1 );
	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _testData.nMuteSignals, 2 );
}

TEST_F( AudioVolNormalizeTest, setVolume_max ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_FLOAT_EQ( _testData.vol, 5.0 );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioVolNormalizeTest, setVolume_min ) {
	ASSERT_TRUE( _mixer->volume( MIN_VOLUME ) );
	ASSERT_FLOAT_EQ( _testData.vol, 1.0 );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioVolNormalizeTest, setVolume_mid ) {
	ASSERT_TRUE( _mixer->volume( 9 ) );
	ASSERT_FLOAT_EQ( _testData.vol, 2.8 );
	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nVolSignals, 1 );
}

TEST_F( AudioVolNormalizeTest, setVolume_negative ) {
	ASSERT_FALSE( _mixer->volume( -5 ) );
	ASSERT_EQ( _testData.nVol, 0 );
	ASSERT_EQ( _testData.nVolSignals, 0 );
}

TEST_F( AudioVolNormalizeTest, setVolume_overflow ) {
	ASSERT_FALSE( _mixer->volume( MAX_VOLUME * 2 ) );
	ASSERT_EQ( _testData.nVol, 0 );
	ASSERT_EQ( _testData.nVolSignals, 0 );
}

TEST_F( AudioVolFloatMaxMinTest, setVolume_max ) {
	ASSERT_TRUE( _mixer->volume( MAX_VOLUME ) );
	ASSERT_FLOAT_EQ( 0.5, _testData.vol );
	ASSERT_EQ( 1, _testData.nVol );
	ASSERT_EQ( 1, _testData.nVolSignals );
}

TEST_F( AudioVolFloatMaxMinTest, setVolume_min ) {
	ASSERT_TRUE( _mixer->volume( MIN_VOLUME ) );
	ASSERT_FLOAT_EQ( 0.0, _testData.vol );
	ASSERT_EQ( 1, _testData.nVol );
	ASSERT_EQ( 1, _testData.nVolSignals );
}

TEST_F( AudioVolFloatMaxMinTest, setVolume_mid ) {
	ASSERT_TRUE( _mixer->volume( 9 ) );
	ASSERT_FLOAT_EQ( 0.225, _testData.vol );
	ASSERT_EQ( 1, _testData.nVol );
	ASSERT_EQ( 1, _testData.nVolSignals );
}

TEST_F( AudioVolFloatMaxMinTest, setVolume_negative ) {
	ASSERT_FALSE( _mixer->volume( -5 ) );
	ASSERT_FLOAT_EQ( 0.0, _testData.nVol );
	ASSERT_EQ( _testData.nVolSignals, 0 );
}

TEST_F( AudioVolFloatMaxMinTest, setVolume_overflow ) {
	ASSERT_FALSE( _mixer->volume( MAX_VOLUME * 2 ) );
	ASSERT_FLOAT_EQ( 0.0, _testData.nVol );
	ASSERT_EQ( _testData.nVolSignals, 0 );
}

TEST_F( AudioOkTest, change_audioChannel ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioOkTest, change_audioChannel_twice ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::surround ) );
	ASSERT_EQ( _testData.nChannel, 3 );
	ASSERT_EQ( _testData.nChannelSignals, 2 );
}

TEST_F( AudioOkTest, change_audioChannel_invalid ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_FALSE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioOkTest, change_audioChannel_invalid2 ) {
	ASSERT_FALSE( _mixer->channel( canvas::audio::channel::mono ) );
	ASSERT_EQ( _testData.nChannel, 1 );
	ASSERT_EQ( _testData.nChannelSignals, 0 );
}

TEST_F( AudioOkTest, change_currentAudioChannel ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_EQ( _mixer->channel(), canvas::audio::channel::stereo );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioOkTest, change_currentAudioChannel2 ) {
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );
	ASSERT_FALSE( _mixer->channel( canvas::audio::channel::mono ) );
	ASSERT_EQ( _testData.nChannel, 2 );
	ASSERT_EQ( _mixer->channel(), canvas::audio::channel::stereo );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioOkTest, supported_audioChannel ) {
	std::vector<canvas::audio::channel::type> channels = _mixer->supportedChannels();
	std::vector<canvas::audio::channel::type>::const_iterator it=std::find( channels.begin(), channels.end(), canvas::audio::channel::stereo );
	ASSERT_TRUE( it != channels.end() );
}

TEST_F( AudioOkTest, supported_audioChannel_invalid ) {
	std::vector<canvas::audio::channel::type> channels = _mixer->supportedChannels();
	std::vector<canvas::audio::channel::type>::const_iterator it=std::find( channels.begin(), channels.end(), canvas::audio::channel::mono );
	ASSERT_TRUE( it == channels.end() );
}

TEST_F( AudioOkTest, refresh ) {
	ASSERT_TRUE( _mixer->volume( 2 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );

	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_EQ( _testData.nChannel, 2 );

	ASSERT_TRUE( _mixer->refresh() );

	ASSERT_EQ( _testData.nVol, 2 );
	ASSERT_EQ( _testData.nMute, 1 );
	ASSERT_EQ( _testData.nChannel, 3 );
	
	ASSERT_EQ( _testData.nVolSignals, 1 );
	ASSERT_EQ( _testData.nMuteSignals, 1 );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioVolChangeFailTest, refresh ) {
	ASSERT_FALSE( _mixer->volume( 2 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->channel( canvas::audio::channel::stereo ) );

	ASSERT_EQ( _testData.nVol, 1 );
	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_EQ( _testData.nChannel, 2 );

	ASSERT_FALSE( _mixer->refresh() );

	ASSERT_EQ( _testData.nVol, 2 );
	ASSERT_EQ( _testData.nMute, 1 );
	ASSERT_EQ( _testData.nChannel, 3 );

	ASSERT_EQ( _testData.nVolSignals, 0 );
	ASSERT_EQ( _testData.nMuteSignals, 1 );
	ASSERT_EQ( _testData.nChannelSignals, 1 );
}

TEST_F( AudioOkTest, implVolChanged ) {
	ASSERT_TRUE( _mixer->volume( 10 ) );
	((AudioOk*)_mixer)->deviceChanged( 5, true );

	ASSERT_EQ( _mixer->volume(), 5 );
	ASSERT_TRUE( _mixer->mute() );

	ASSERT_EQ( _testData.nMuteSignals, 1 );
	ASSERT_EQ( _testData.nVolSignals, 2 );
}

TEST_F( AudioWithMuteByVolumeTest, setMute ) {
	ASSERT_TRUE( _mixer->volume( 10 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->mute() );

	ASSERT_EQ( _testData.nMute, 0 );
	ASSERT_EQ( _testData.nVol, 3 );
	ASSERT_EQ( _testData.vol, 0 );
}

TEST_F( AudioWithMuteByVolumeTest, setMute_false ) {
	ASSERT_TRUE( _mixer->volume( 10 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->mute( false ) );
	ASSERT_FALSE( _mixer->mute() );

	ASSERT_EQ( _testData.nMute, -1 );
	ASSERT_EQ( _testData.nVol, 4 );
	ASSERT_EQ( _testData.vol, 10 );
}

TEST_F( AudioWithMuteByVolumeTest, standby_muted ) {
	ASSERT_TRUE( _mixer->volume( 10 ) );
	ASSERT_TRUE( _mixer->mute( true ) );
	ASSERT_TRUE( _mixer->mute() );

	_mixer->standby();

	ASSERT_TRUE( _mixer->mute() );
	ASSERT_EQ( _testData.vol, 0 );

	_mixer->wakeup();

	ASSERT_TRUE( _mixer->mute() );
	ASSERT_EQ( _testData.vol, 0 );
}

TEST_F( AudioWithMuteByVolumeTest, standby_unmuted ) {
	ASSERT_TRUE( _mixer->volume( 10 ) );
	ASSERT_FALSE( _mixer->mute() );

	_mixer->standby();

	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _testData.vol, 0 );

	_mixer->wakeup();

	ASSERT_FALSE( _mixer->mute() );
	ASSERT_EQ( _testData.vol, 10 );
}