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

#pragma once

#include <pvr/types.h>
#include <canvas/types.h>
#include <boost/signals2.hpp>
#include <vector>

namespace canvas {
	class MediaPlayer;
	class System;
}

namespace pvr {
	class Player;
	class Channel;
}

namespace zapper {

class Zapper;

namespace channel {

class Player {
public:
	explicit Player( Zapper *zapper );
	virtual ~Player();

	//	Initialization/start
	bool initialize();
	void finalize();
	void start();
	void stop();

	//	Change channel
	typedef boost::signals2::signal<void (int, pvr::Channel *)> ChangeChannelSignal;
	ChangeChannelSignal &onChangeChannel();
	int change( int channelID );
	int current() const;
	const std::string &currentNetworkName() const;
	int nextChannel( int first, int factor ) const;
	int nextFavorite( int first, int factor ) const;
	bool showMobile() const;
	void showMobile( bool show ) const;

	//	Status
	bool isBlocked() const;
	void status( int &signal, int &quality );
	typedef boost::signals2::signal<void (bool isprotected)> ChannelProtectedSignal;
	ChannelProtectedSignal &onChannelProtected();

	//	Player methods
	int nextVideo();
	int videoCount() const;
	void videoInfo( pvr::info::Basic &info, int id=-1 ) const;

	int nextAudio();
	int audioCount() const;
	void audioInfo( pvr::info::Audio &info, int id=-1 ) const;

	int nextSubtitle();
	int subtitleCount() const;
	void subtitleInfo( pvr::info::Basic &info, int id=-1 ) const;

	void setFullScreen();
	void resize( int x, int y, int w, int h );
	void mute( bool needMute );
	void volume( canvas::Volume vol );
	canvas::audio::channel::type getAudio() const;
	canvas::aspect::type getAspect() const;
	canvas::mode::type getResolution() const;

	bool playVideoStream( int sId, std::string &url );
	void stopVideoStream( int sID );

protected:
	//	Notifications
	void onMedia( const std::string &url, bool started );

	//	Getters
	Zapper *zapper() const;
	pvr::Player *player() const;
	canvas::MediaPlayer *media() const;

private:
	Zapper *_zapper;
	pvr::Player *_player;
	canvas::MediaPlayer *_media;
};

}
}
