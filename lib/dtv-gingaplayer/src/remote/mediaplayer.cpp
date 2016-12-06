/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "mediaplayer.h"
#include <util/log.h>
#include <connector/connector.h>
#include <connector/handler/playvideohandler.h>
#include <connector/handler/stopvideohandler.h>
#include <connector/handler/videoresizehandler.h>
#include <canvas/rect.h>
#include <canvas/surface.h>
#include <canvas/streaminfo.h>

namespace player {
namespace remote {

class StreamsInfo : public canvas::StreamsInfo {
protected:
	virtual void refreshImpl() {}
	virtual void parseImpl() {}
};

MediaPlayer::MediaPlayer( size_t id, connector::Connector *conn, canvas::System *sys )
	: canvas::MediaPlayer( sys ), _id(id), _conn( conn )
{
}

MediaPlayer::~MediaPlayer()
{
}

bool MediaPlayer::startImpl( const std::string &url ) {
	connector::PlayVideoHandler playVideo;
	playVideo.send( _conn, _id, url );
	send();
	return true;
}

void MediaPlayer::stopImpl() {
	LDEBUG("remote::MediaPlayer", "Stop impl: id=%ld", _id );
	connector::StopVideoHandler stopVideo;
	stopVideo.send( _conn, _id );
}

void MediaPlayer::pauseImpl( bool /*needPause*/ ) {
}

void MediaPlayer::muteImpl( bool /*needsMute*/ ) {
}

void MediaPlayer::volumeImpl( canvas::Volume /*vol*/ ) {
}

void MediaPlayer::moveResizeImpl( const canvas::Rect &/*rect*/ ) {
	send();
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int /*id*/ ) {
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int /*id*/ ) {
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int /*id*/ ) {
	return true;
}

void MediaPlayer::send() {
	{	//	Send resize
		connector::VideoResizeHandler<int> videoResize;
		canvas::Rect rect = bounds();
		connector::AVDescriptor<int> desc( rect.x, rect.y, rect.w, rect.h);
		videoResize.send( _conn, _id, &desc );

		LDEBUG("remote::MediaPlayer", "Draw: _rect=(%d,%d,%d,%d)",
			rect.x, rect.y, rect.w, rect.h);
	}
}

}
}
