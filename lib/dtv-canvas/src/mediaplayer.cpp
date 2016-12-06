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
#include "player.h"
#include "point.h"
#include "types.h"
#include "canvas.h"
#include "window.h"
#include "screen.h"
#include "system.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/cfg/cfg.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {

void MediaPlayer::destroy( MediaPlayerPtr &mp ) {
	DTV_ASSERT(mp);
	System *sys = mp->system();
	sys->player()->destroy( mp );
}

/**
 * Constructor base.
 */
MediaPlayer::MediaPlayer( System *sys )
{
	DTV_ASSERT(sys);
	_sys = sys;
	_loop = false;
	_state = stopped;
	_muted = false;
	_volume = boost::math::iround(float(MAX_VOLUME+MIN_VOLUME)/2.0f);
	_streamsInfo = NULL;
	_bounds = Rect(Point(0,0),canvasSize());
}

/**
 * Destructor base.
 */
MediaPlayer::~MediaPlayer()
{
	if (_streamsInfo) {
		DEL(_streamsInfo);
	}
}

bool MediaPlayer::play( const std::string &url ) {
	LDEBUG("MediaPlayer", "Play url=%s", url.c_str());
	if (_state > stopped) {
		stop();
	}
	if (!url.empty()) {
		_url = url;
		if (startImpl(url)) {
			_state = playing;
			_onModeChanged = _sys->screen()->onModeChanged().connect( boost::bind( &MediaPlayer::onScreenChanged, this ) );
			_onAspectChanged = _sys->screen()->onAspectChanged().connect( boost::bind( &MediaPlayer::onScreenChanged, this ) );
			_onStart();

			if (parsedOnPlay()) {
				onMediaParsed();
			}
		}
	}
	return _state == playing;
}

bool MediaPlayer::parsedOnPlay() const {
	return false;
}

void MediaPlayer::stop() {
	LDEBUG("MediaPlayer", "Stop");
	if (_state > stopped) {
		_state = stopped;
		_onModeChanged.disconnect();
		_onAspectChanged.disconnect();
		stopImpl();
		sInfo()->reset();
		_onStop();
	}
}

void MediaPlayer::pause() {
	LTRACE("MediaPlayer", "Pause: state=%d", _state);
	if (_state == playing) {
		pauseImpl(true);
		_state = paused;
	}
}

void MediaPlayer::unpause() {
	LTRACE("MediaPlayer", "Unpause: state=%d", _state);
	if (_state == paused) {
		pauseImpl(false);
		_state = playing;
	}
}

void MediaPlayer::loop( bool loop ) {
	LTRACE("MediaPlayer", "Loop old=%d new=%d", _loop, loop);
	_loop = loop;
}

bool MediaPlayer::isPlaying() const {
	return _state == playing;
}

bool MediaPlayer::isPaused() const {
	return _state == paused;
}

bool MediaPlayer::isLooping() const {
	return (_state == playing) && _loop;
}

const std::string &MediaPlayer::url() const {
	return _url;
}

void MediaPlayer::mute( bool needsMute ) {
	if (_muted != needsMute) {
		muteImpl( needsMute );
		_muted = needsMute;
	}
}

bool MediaPlayer::mute() const {
	return _muted;
}

void MediaPlayer::volume( Volume vol ) {
	if (vol != _volume) {
		if(vol >= MIN_VOLUME && vol <= MAX_VOLUME) {
			volumeImpl( vol );
			_volume = vol;
			mute( false );
		} else {
			LWARN("MediaPlayer", "Fail to set volume to: %d. Invalid value", vol);
		}
	}
}

Volume MediaPlayer::volume() const {
	return _volume;
}

void MediaPlayer::move( const Point &point ) {
	LTRACE("MediaPlayer", "Move. point=(%d,%d)", point.x, point.y);
	moveResize( Rect(point.x, point.y, _bounds.w, _bounds.h) );
}

void MediaPlayer::resize( const Size &size ) {
	LTRACE("MediaPlayer", "Resize. size=(%d,%d)", size.w, size.h);
	moveResize( Rect(_bounds.x, _bounds.y, size.w, size.h) );
}

void MediaPlayer::moveResize( const Rect &rect ) {
	LTRACE("MediaPlayer", "Move and resize. rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h);
	Size s = canvasSize();
	if (rect.x >= 0 && rect.y >= 0 && rect.w > 0 && rect.h > 0 && rect.w <= s.w && rect.h <= s.h) {
		_bounds = rect;
		if (_state>stopped) {
			moveResizeImpl( rect );
		}
	} else {
		LWARN("MediaPlayer", "Invalid bounds x: %d y: %d w: %d h: %d", rect.x, rect.y, rect.w, rect.h);
	}
}

void MediaPlayer::translateToWindow( Rect &tmp ) {
	if (!isFullScreen()) {
		_sys->window()->translateToWindow(tmp.x, tmp.y, true);
		_sys->window()->scaleToWindow(tmp.w, tmp.h, true);
	}
	else {
		tmp = Rect( Point(0,0), _sys->window()->size() );
	}
}

void MediaPlayer::setFullScreen() {
	LTRACE("MediaPlayer", "Set fullscreen!");
	moveResize( Rect(Point(0,0), canvasSize()) );
}

bool MediaPlayer::isFullScreen() const {
	Rect canvasBounds(Point(0,0),canvasSize());
	return _bounds == canvasBounds;
}

const Size &MediaPlayer::canvasSize() const {
	return system()->canvas()->size();
}

void MediaPlayer::feed( util::Buffer * /*buf*/ ) {
}

/**
 * @return Señal que es lanzada cuando comienza la reporducción de un archivo multimedia.
 */
MediaPlayer::OnMediaEvent &MediaPlayer::onStart() {
	return _onStart;
}

/**
 * @return Señal que es lanzada cuando finaliza la reporducción de un archivo multimedia.
 */
MediaPlayer::OnMediaEvent &MediaPlayer::onStop() {
	return _onStop;
}

/**
 * @return Señal que es lanzada cuando ocurre un error en la reporducción de un archivo multimedia.
 */
MediaPlayer::OnMediaError &MediaPlayer::onError() {
	return _onError;
}

/**
 * @return Señal que es lanzada cuando cambia la informacion de los streams de audio, video o subtitulos.
 */
MediaPlayer::OnMediaEvent &MediaPlayer::onMediaChanged() {
	return _onMediaChanged;
}

void MediaPlayer::onMediaCompleted() {
	LTRACE("MediaPlayer", "On media completed! Loop=%d", _loop);
	DTV_ASSERT( checkThread() );
	stop();
	if (_loop) {
		if (!play(_url)) {
			onMediaError("Fail to loop on media");
		}
	}
}

void MediaPlayer::onMediaParsed() {
	DTV_ASSERT( checkThread() );
	if (_state>stopped) {
		sInfo()->parse();
		_onMediaChanged();
	}
}

void MediaPlayer::onMediaError( const std::string &msg ) {
	LWARN("MediaPlayer", "Error playing media. msg: %s", msg.c_str());
	DTV_ASSERT( checkThread() );
	if (_state > stopped) {
		stop();
	}
	_onError(msg);
}

void MediaPlayer::onScreenChanged() {
	moveResizeImpl( _bounds );
}

Rect MediaPlayer::bounds() const {
	return _bounds;
}

const StreamsInfo &MediaPlayer::streamsInfo() {
	LTRACE("MediaPlayer", "Get streams info!");
	if (_state > stopped) {
		streamsInfoImpl();
	}
	return *sInfo();
}

void MediaPlayer::streamsInfoImpl() {
	sInfo()->refresh();
}

bool MediaPlayer::switchVideoStream( int id ) {
	LTRACE("MediaPlayer", "Switch video stream! id=%d", id);
	bool check = false;
	if (_state > stopped) {
		check = id >= 0;
		check &= id < (int) sInfo()->videoCount();
		if (check && switchVideoStreamImpl(id)) {
			sInfo()->currentVideo(id);
		} else {
			LERROR("MediaPlayer", "Fail to switch video stream! id=%d", id);
			check = false;
		}
	} else {
		LWARN("MediaPlayer", "Fail to switch video stream! invalid state=%d stream_id=%d", _state, id);
	}
	return check;
}

bool MediaPlayer::switchAudioStream( int id ) {
	LTRACE("MediaPlayer", "Switch audio stream! id=%d", id);
	bool check = false;
	if (_state > stopped) {
		check = id >= 0;
		check &= id < (int) sInfo()->audioCount();
		if (check && switchAudioStreamImpl(id)) {
			sInfo()->currentAudio(id);
		} else {
			LERROR("MediaPlayer", "Fail to switch audio stream! id=%d", id);
			check = false;
		}
	} else {
		LWARN("MediaPlayer", "Fail to switch audio stream! invalid state=%d stream_id=%d", _state, id);
	}
	return check;
}

bool MediaPlayer::switchSubtitleStream( int id ) {
	LTRACE("MediaPlayer", "Switch subtitle stream! id=%d", id);
	bool check = false;
	if (_state > stopped) {
		check = id >= 0;
		check &= id < (int) sInfo()->subtitleCount();
		if (check && switchSubtitleStreamImpl(id)) {
			sInfo()->currentSubtitle(id);
		} else {
			LERROR("MediaPlayer", "Fail to switch subtitle stream! id=%d", id);
			check = false;
		}
	} else {
		LWARN("MediaPlayer", "Fail to switch subtitle stream! invalid state=%d stream_id=%d", _state, id);
	}
	return check;
}

StreamsInfo *MediaPlayer::sInfo() {
	if (!_streamsInfo) {
		_streamsInfo = createStreamsInfo();
	}
	return _streamsInfo;
}

void MediaPlayer::onEnqueued() {
}

void MediaPlayer::onDequeued() {
}

bool MediaPlayer::checkThread() const {
	return mainLoopThreadId() == boost::thread::id() || mainLoopThreadId() == boost::this_thread::get_id();
}

void MediaPlayer::mainLoopThreadId( const boost::thread::id &id ) {
	_threadId = id;
}

const boost::thread::id &MediaPlayer::mainLoopThreadId() const {
	return _threadId;
}

System *MediaPlayer::system() const {
	return _sys;
}

}
