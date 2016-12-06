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

#include "mediaplayer.h"

namespace canvas {

class Player;

namespace single {

class MediaPlayer : public canvas::MediaPlayer {
public:
	MediaPlayer( System *sys, Player *player, canvas::MediaPlayer *mp );
	virtual ~MediaPlayer();

	virtual void stop();
	virtual void pause();
	virtual void unpause();
	virtual void loop( bool loop );

	virtual void feed( util::Buffer *buf );

	virtual void onEnqueued();
	virtual void onDequeued();

protected:
	virtual bool startImpl( const std::string &url );
	virtual void stopImpl();
	virtual void pauseImpl( bool needPause );

	void onMediaStopped();

	virtual void muteImpl( bool needsMute );
	virtual void volumeImpl( Volume vol );

	virtual void onScreenChanged();
	virtual void moveResizeImpl( const Rect &rect );

	virtual canvas::StreamsInfo *createStreamsInfo();
	virtual void streamsInfoImpl();
	virtual bool switchVideoStreamImpl( int id );
	virtual bool switchAudioStreamImpl( int id );
	virtual bool switchSubtitleStreamImpl( int id );

private:
	bool _needPause;
	bool _needLoop;
	bool _needMute;
	Player *_player;
	canvas::MediaPlayer *_mp;
};

}
}

