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

#include "../../../mediaplayer.h"
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>

class ANativeWindow;
class _jobject;
typedef _jobject* jobject;
struct _JNIEnv;
typedef _JNIEnv JNIEnv;

namespace util {
	class Buffer;
}

namespace canvas {

class System;
class StreamsInfo;
namespace gl { class Canvas; class Surface; }

namespace android {
namespace omxal {

class MediaPlayer : public canvas::MediaPlayer {
public:
	explicit MediaPlayer( canvas::System *sys );
	virtual ~MediaPlayer();

	//	Feed data
	virtual void feed( util::Buffer *buf );

	//	Implementation
	void streamChangeCallback(
		XAStreamInformationItf caller,
		XAuint32 eventId,
		XAuint32 streamIndex,
		void * pEventData
	);
	XAresult bufferQueueCallback(
        XAAndroidBufferQueueItf caller,
        void *pBufferContext,
        void *pBufferData,
        XAuint32 dataSize,
        XAuint32 dataUsed,
        const XAAndroidBufferItem *pItems,
        XAuint32 itemsLength
	);
	void prefetchStatusCallback(
		XAPrefetchStatusItf caller,
		XAuint32 event
	);
	void playCallback(
		XAPlayItf caller,
		XAuint32 event
	);

protected:
	virtual bool startImpl( const std::string &url );
	virtual void stopImpl();
	virtual void pauseImpl( bool needPause );

	virtual void muteImpl( bool needsMute );
	virtual void volumeImpl( Volume vol );

	virtual void moveResizeImpl( const Rect &rect );

	//	Streams info implementation
	virtual canvas::StreamsInfo *createStreamsInfo();
	virtual bool switchVideoStreamImpl( int id );
	virtual bool switchAudioStreamImpl( int id );
	virtual bool switchSubtitleStreamImpl( int id );

	//	Aux initialization
	bool initialize( const Rect &rect, const std::string &url );
	bool initialize( const std::string &url );
	void finalize();
	bool createEngine();

private:
	JNIEnv *_jni;
	canvas::gl::Canvas *_canvas;
	canvas::gl::Surface *_surface;
	jobject _jniSurfaceTexture;
	jobject _jniSurface;	
	ANativeWindow *_window;
	XAObjectItf _oEngine;
	XAEngineItf _iEngine;
	XAObjectItf _oMixer;
	XAObjectItf _oPlayer;
	XAPlayItf   _iPlayerPlay;
	XAStreamInformationItf _iPlayerStreamInfo;
	XAVolumeItf _iPlayerVol;
	XAAndroidBufferQueueItf _iPlayerBufferQueue;
	XAPrefetchStatusItf _iPlayerPrefetchStatus;
	boost::condition_variable _cWakeup;
	boost::mutex _mutex;
	bool _enqueue;
	int _cache;
};

}
}
}

