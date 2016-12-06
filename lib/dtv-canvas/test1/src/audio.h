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

#pragma once

#include "../../src/audio.h"
#include "../../src/types.h"
#include <gtest/gtest.h>

struct TestData {
	int nMute;
	int nVol;
	int nChannel;
	int nOpen;
	int nClose;
	int nMuteSignals;
	int nVolSignals;
	int nChannelSignals;
	float vol;
};

class AudioOk : public canvas::Audio {
public:
	explicit AudioOk( TestData &testData ) : _testData( testData ) {}

	void deviceChanged( canvas::Volume vol, bool muted ) {
		onDeviceUpdated( vol, muted );
	}

	TestData &_testData;

protected:
	virtual bool init() {
		canvas::audio::DeviceInfo dev;
		dev.name = "output1";
		dev.displayName = "desc";
		dev.displayDescription = "TEST";
		addDevice( dev );
		return true;
	}
	virtual void fin() {}

	virtual bool open( const std::string &/*dev*/ ) {
		_testData.nOpen++;
		return true;
	}
	virtual void close() {
		_testData.nClose++;
	}
	virtual bool muteImpl( bool needsMute ) {
		_testData.nMute += needsMute ? 1 : -1;
		return true;
	}
	virtual bool volumeImpl( canvas::Volume /*vol*/ ) {
		_testData.nVol++;
		return true;
	}
	virtual std::vector<canvas::audio::channel::type> supportedChannels() const {
		std::vector<canvas::audio::channel::type> auds;
		auds.push_back( canvas::audio::channel::stereo );
		auds.push_back( canvas::audio::channel::surround );
		return auds;
	}
	virtual bool channelImpl( canvas::audio::channel::type /*channel*/ ) {
		_testData.nChannel++;
		return true;
	}
	virtual bool supportMultipleDevices() {
		return true;
	}
};

class AudioWithMuteByVolume : public AudioOk {
public:
	explicit AudioWithMuteByVolume( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool muteImpl( bool needsMute ) {
		_testData.nMute += needsMute ? 1 : -1;
		volumeImpl( needsMute ? 0 : 10 );
		return true;
	}
	virtual bool volumeImpl( canvas::Volume vol ) {
		_testData.vol = vol;
		_testData.nVol++;
		return true;
	}
};

class AudioVolChangeFail : public AudioOk {
public:
	explicit AudioVolChangeFail( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool volumeImpl( canvas::Volume vol ) {
		_testData.vol = vol;
		_testData.nVol++;
		return false;
	}
};

class AudioWithVolNormalize : public AudioOk {
public:
	explicit AudioWithVolNormalize( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool volumeImpl( canvas::Volume vol ) {
		_testData.vol = translateVol( vol, 1, 5 );
		_testData.nVol++;
		return true;
	}
};

class AudioVolFloatMaxMin : public AudioOk {
public:
	explicit AudioVolFloatMaxMin( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool volumeImpl( canvas::Volume vol ) {
		_testData.vol = translateVol( vol, 0.0, 0.5 );
		_testData.nVol++;
		return true;
	}
};

class AudioWithOutDevices : public AudioOk {
public:
	explicit AudioWithOutDevices( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool supportMultipleDevices() {
		return false;
	}
};

class AudioFailInit : public AudioOk {
public:
	explicit AudioFailInit( TestData &testData ) : AudioOk( testData ) {}
protected:
	virtual bool init() { return false; }
};

class AudioTest : public testing::Test {
public:
	AudioTest();
	virtual ~AudioTest();

protected:
	bool init();
	void fin();

	virtual void SetUp();
	virtual void TearDown();

	virtual canvas::Audio *createAudio();

	void onMuteChanged( bool /*muted*/ ) {
		_testData.nMuteSignals++;
	}
	void onVolumeChanged( canvas::Volume /*vol*/ ) {
		_testData.nVolSignals++;
	}
	void onChannelChanged( canvas::audio::channel::type /*channel*/ ) {
		_testData.nChannelSignals++;
	}

	canvas::Audio *_mixer;
	boost::signals2::connection _onMuteChanged;
	boost::signals2::connection _onVolumeChanged;
	boost::signals2::connection _onChannelChanged;
	TestData _testData;
};

class AudioFailInitializationTest : public AudioTest {
protected:
	virtual void SetUp() { init(); }
	virtual void TearDown() { fin(); };

	virtual canvas::Audio *createAudio() { return new AudioFailInit( _testData ); }
};

class AudioOkInitializationTest : public AudioFailInitializationTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioOk( _testData ); }
};

class AudioOkInitializationTestNoDevices : public AudioFailInitializationTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioWithOutDevices( _testData ); }
};

class AudioVolNormalizeTest : public AudioTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioWithVolNormalize( _testData ); }
};

class AudioWithMuteByVolumeTest : public AudioTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioWithMuteByVolume( _testData ); }
};

class AudioVolFloatMaxMinTest : public AudioTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioVolFloatMaxMin( _testData ); }
};

class AudioVolChangeFailTest : public AudioTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioVolChangeFail( _testData ); }
};

class AudioOkTest : public AudioTest {
protected:
	virtual canvas::Audio *createAudio() { return new AudioOk( _testData ); }
};
