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

#include "singlemediaplayer.h"
#include "player.h"
#include <util/assert.h>
#include <util/log.h>

namespace canvas {
namespace single {

class StreamsInfo : public canvas::StreamsInfo {
public:
	explicit StreamsInfo( const canvas::StreamsInfo &sInfo ) : _sInfo(sInfo) {}
	virtual ~StreamsInfo() {}

	virtual int currentAudio() const { return _sInfo.currentAudio(); }
	virtual const std::vector<AudioStreamInfo*> &audioInfo() const { return _sInfo.audioInfo(); }

	virtual int currentVideo() const { return _sInfo.currentVideo(); }
	virtual const std::vector<VideoStreamInfo*> &videoInfo() const { return _sInfo.videoInfo(); }

	virtual int currentSubtitle() const { return _sInfo.currentSubtitle(); }
	virtual const std::vector<SubtitleStreamInfo*> &subtitleInfo() const { return _sInfo.subtitleInfo(); }

protected:
	virtual void refreshImpl() { /*Do nothing*/ };
	virtual void parseImpl() { /*Do nothing*/ };

private:
	const canvas::StreamsInfo &_sInfo;
};

MediaPlayer::MediaPlayer( canvas::System *sys, canvas::Player *player, canvas::MediaPlayer *mp )
	: canvas::MediaPlayer(sys), _player(player), _mp(mp)
{
	DTV_ASSERT(_player);
	DTV_ASSERT(_mp);
	_needPause = false;
	_needLoop = false;
	_needMute = false;

	_mp->onMediaChanged().connect( boost::bind(&MediaPlayer::onMediaParsed, this) );
	_mp->onError().connect( boost::bind(&MediaPlayer::onMediaError, this, _1) );
	_mp->onStop().connect( boost::bind(&MediaPlayer::onMediaStopped, this) );
}

MediaPlayer::~MediaPlayer()
{
	_player = NULL;
	_mp = NULL;
}

bool MediaPlayer::startImpl( const std::string &url ) {
	_player->enqueue(this);
	if (!_mp->play(url)) {
		_player->dequeue(this);
		return false;
	}
	_mp->loop(_needLoop);
	_mp->mute(_needMute);
	_mp->volume(volume());
	_mp->moveResize(bounds());
	return true;
}

void MediaPlayer::stop() {
	canvas::MediaPlayer::stop();
	_player->dequeue(this);
}

void MediaPlayer::stopImpl() {
	_mp->stop();
}

void MediaPlayer::pause() {
	canvas::MediaPlayer::pause();
	_needPause = true;
}

void MediaPlayer::unpause() {
	canvas::MediaPlayer::unpause();
	_needPause = false;
}

void MediaPlayer::pauseImpl( bool needPause ) {
	if (needPause) {
		_mp->pause();
	} else {
		_mp->unpause();
	}
}

void MediaPlayer::loop( bool loop ) {
	canvas::MediaPlayer::loop( loop );
	if (_player->isCurrent(this)) {
		_mp->loop(loop);
	}
	_needLoop = loop;
}

void MediaPlayer::onScreenChanged() {} //Avoid the moveResize so it doesn't do it twice

void MediaPlayer::feed( util::Buffer *buf ) {
	_mp->feed(buf);
}

void MediaPlayer::muteImpl( bool needsMute ) {
	if (_player->isCurrent(this)) {
		_mp->mute(needsMute);
	}
	_needMute = needsMute;
}
void MediaPlayer::volumeImpl( Volume vol ) {
	if (_player->isCurrent(this)) {
		_mp->volume(vol);
	}
}

void MediaPlayer::moveResizeImpl( const Rect &rect ) {
	if (_player->isCurrent(this)) {
		_mp->moveResize(rect);
	}
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo(_mp->streamsInfo());
}

void MediaPlayer::streamsInfoImpl() {
	_mp->streamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	return _mp->switchVideoStream( id );
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	return _mp->switchAudioStream( id );
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	return _mp->switchSubtitleStream( id );
}

void MediaPlayer::onMediaStopped() {
	if (isPlaying()) {
		onMediaCompleted();
	}
}

void MediaPlayer::onEnqueued() {
	LDEBUG("single::MediaPlayer", "Enqueued mp=%p", this);
	_needPause = isPaused();
}

void MediaPlayer::onDequeued() {
	LDEBUG("single::MediaPlayer", "Dequeued mp=%p", this);
	if (_needPause) {
		pause();
	}
	loop(_needLoop);
	mute(_needMute);
	_mp->volume(volume());
	_mp->moveResize(bounds());
}

}
}
