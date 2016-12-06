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

#include "mediaplayer.h"
#include <util/log.h>

namespace canvas {
namespace dummy {

class StreamsInfo : public canvas::StreamsInfo {
protected:
	virtual void refreshImpl() {
		LDEBUG("dummy::MediaPlayer", "StreamsInfo refresh");
	}

	virtual void parseImpl() {
		LDEBUG("dummy::MediaPlayer", "StreamsInfo parse");
	}
};

MediaPlayer::MediaPlayer( canvas::System *sys ) : canvas::MediaPlayer( sys )
{
}

MediaPlayer::~MediaPlayer()
{
}

//	Stream operations
bool MediaPlayer::startImpl( const std::string &url ) {
	LDEBUG("dummy::MediaPlayer", "Start: url=%s", url.c_str());
	return true;
}

void MediaPlayer::stopImpl() {
 	LDEBUG("dummy::MediaPlayer", "Stop");
}

void MediaPlayer::pauseImpl( bool needPause ) {
 	LDEBUG("dummy::MediaPlayer", "Pause: value=%d", needPause);
}

void MediaPlayer::muteImpl( bool needsMute ) {
	LDEBUG("dummy::MediaPlayer", "Needs mute = %d", needsMute);
}

void MediaPlayer::volumeImpl( Volume vol ) {
	LDEBUG("dummy::MediaPlayer", "Change volume to: %d", vol);
}

void MediaPlayer::moveResizeImpl(const Rect& rect) {
	LDEBUG("dummy::MediaPlayer", "MoveResize to x: %d y: %d w: %d h: %d", rect.x, rect.y, rect.w, rect.h);
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	LDEBUG("dummy::MediaPlayer", "Switch video stream info id=%d", id);
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	LDEBUG("dummy::MediaPlayer", "Switch audio stream info id=%d", id);
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	LDEBUG("dummy::MediaPlayer", "Switch subtitle stream info id=%d", id);
	return true;
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	LDEBUG("dummy::MediaPlayer", "Create streams info");
	return new StreamsInfo();
}

}
}
