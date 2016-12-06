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

#include "types.h"

namespace canvas {

typedef boost::signals2::signal<void (Volume)> ChangedVolume;
typedef boost::signals2::signal<void (bool)> ChangedMute;
typedef boost::signals2::signal<void (audio::channel::type)> ChangedChannel;

class Audio {
public:
	Audio();
	virtual ~Audio();

	//	Initialization
	bool initialize( System *sys );
	void finalize();

	//	Mute control
	bool mute() const;
	bool mute( bool needMute );
	bool toggleMute();

	//	Volume control
	bool volume( Volume vol );
	bool volumeUp();
	bool volumeDown();
	Volume volume() const;

	//	Audio channel control
	bool channel( audio::channel::type channel );
	const audio::channel::type &channel();
	virtual std::vector<audio::channel::type> supportedChannels() const;
	virtual audio::channel::type defaultChannel() const;

	//	Refresh state to device
	bool refresh();

	//	Standby/wakeup
	virtual void standby();
	virtual void wakeup();

	//	Audio outputs
	const audio::Devices &devices();
	virtual size_t defaultDevice() const;

	//	Instance creation
	static Audio *create( const std::string &use="" );

	//	Signal handle
	ChangedMute &onMuteChanged();
	ChangedVolume &onVolumeChanged();
	ChangedChannel &onChannelChanged();

protected:
	virtual bool init();
	virtual void fin();

	virtual bool open( const std::string &dev );
	virtual void close();

	virtual bool muteImpl( bool needsMute )=0;
	virtual bool volumeImpl( Volume vol )=0;
	virtual bool channelImpl( audio::channel::type channel )=0;

	bool isChannelSupported( audio::channel::type channel );
	virtual bool supportMultipleDevices();
	void addDevice( const audio::DeviceInfo &dev );
	void dumpDevices();

	//	Translates volumen into the implementation ranges
	float translateVol( Volume vol, float min, float max );

	void onDeviceUpdated( Volume vol, bool muted );
	System *system() const;

private:
	System *_sys;
	bool _initialized;
	audio::Devices _devices;
	Volume _volume;
	bool _muted;
	audio::channel::type _currentChannel;
	ChangedMute _onMuteChanged;
	ChangedVolume _onVolumeChanged;
	ChangedChannel _onChannelChanged;
};

}

