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
#include "rect.h"
#include "streaminfo.h"
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <string>

namespace util {
	class Buffer;
}

namespace canvas {

/**
 * Permite la reproducción de un video en particular.
 */
class MediaPlayer {
public:
	explicit MediaPlayer( System *sys );
	virtual ~MediaPlayer();

	//	Media control
	bool play( const std::string &url );
	virtual void stop();
	virtual void pause();
	virtual void unpause();
	virtual void loop( bool loop );

	//	Getters
	bool isPlaying() const;
	bool isPaused() const;
	bool isLooping() const;
	const std::string &url() const;

	//	Streams info
	const StreamsInfo &streamsInfo();
	bool switchVideoStream( int id );
	bool switchAudioStream( int id );
	bool switchSubtitleStream( int id );

	//	Volume control
	void mute( bool needsMute );
	bool mute() const;
	void volume( Volume vol );
	Volume volume() const;

	//	Render control
	void move( const Point &point );
	void resize( const Size &size );
	void moveResize( const Rect &rect );
	void setFullScreen();
	bool isFullScreen() const;

	//	Notifications
	typedef boost::signals2::signal<void (void)> OnMediaEvent;
	typedef boost::signals2::signal<void (const std::string &msg)> OnMediaError;
	OnMediaEvent &onStart();
	OnMediaEvent &onStop();
	OnMediaError &onError();
	OnMediaEvent &onMediaChanged();

	//	Feed data
	virtual void feed( util::Buffer *buf );

	//	SinglePlayer implementation
	virtual void onEnqueued();
	virtual void onDequeued();

	//	Implementation
	typedef MediaPlayer *MediaPlayerPtr;
	static void destroy( MediaPlayerPtr &mp );
	void mainLoopThreadId( const boost::thread::id &id );
	const boost::thread::id &mainLoopThreadId() const;
	bool checkThread() const;

protected:
	//	Start/stop methods
	virtual bool startImpl( const std::string &url )=0;
	virtual void stopImpl()=0;
	virtual void pauseImpl( bool needPause )=0;
	virtual bool parsedOnPlay() const;

	//	Audio methods
	virtual void muteImpl( bool needsMute )=0;
	virtual void volumeImpl( Volume vol )=0;

	//	Video bounds
	virtual void onScreenChanged();
	virtual void moveResizeImpl( const Rect &rect )=0;
	void translateToWindow( Rect &rect );
	Rect bounds() const;
	const Size &canvasSize() const;

	//	Streams info implementation
	virtual StreamsInfo *createStreamsInfo()=0;
	virtual void streamsInfoImpl();
	virtual bool switchVideoStreamImpl( int id )=0;
	virtual bool switchAudioStreamImpl( int id )=0;
	virtual bool switchSubtitleStreamImpl( int id )=0;
	StreamsInfo *sInfo();

	void onMediaCompleted();
	void onMediaParsed();
	void onMediaError( const std::string &msg );

	System *system() const;

	enum state { stopped, paused, playing };

private:
	state _state;
	std::string _url;
	bool _loop;
	bool _muted;
	Volume _volume;
	Rect _bounds;
	StreamsInfo *_streamsInfo;
	boost::thread::id _threadId;
	System *_sys;

	// Callbacks
	OnMediaEvent _onMediaChanged;
	OnMediaEvent _onStart;
	OnMediaEvent _onStop;
	OnMediaError _onError;
	boost::signals2::connection _onModeChanged;
	boost::signals2::connection _onAspectChanged;
};

}
