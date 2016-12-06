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
#include "../../gl/canvas.h"
#include "../../gl/surface.h"
#include "../../../rect.h"
#include "../../../point.h"
#include "../../../system.h"
#include "../../../surface.h"
#include "../../../streaminfo.h"
#include <util/android/android.h>
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/thread.hpp>
#include <jni.h>
#include <android/native_window_jni.h>

//	number of buffers in our buffer queue, an arbitrary number
#define NB_BUFFERS 50

#define XA_CALL(obj,method,...) (*obj)->method( obj, ##__VA_ARGS__ )

namespace canvas {
namespace android {
namespace omxal {

namespace impl {

static void StreamChangeCallback(
	XAStreamInformationItf caller,
	XAuint32 eventId,
	XAuint32 streamIndex,
	void * pEventData,
	void * pContext )
{
	DTV_ASSERT(pContext);
	MediaPlayer *mp = (MediaPlayer *)pContext;
	mp->streamChangeCallback( caller, eventId, streamIndex, pEventData );
}

// AndroidBufferQueueItf callback to supply MPEG-2 TS packets to the media player
static XAresult AndroidBufferQueueCallback(
	XAAndroidBufferQueueItf caller,
	void *pCallbackContext,        /* input */
	void *pBufferContext,          /* input */
	void *pBufferData,             /* input */
	XAuint32 dataSize,             /* input */
	XAuint32 dataUsed,             /* input */
	const XAAndroidBufferItem *pItems,/* input */
	XAuint32 itemsLength           /* input */)
{
	DTV_ASSERT(pCallbackContext);
	MediaPlayer *mp = (MediaPlayer *)pCallbackContext;
	return mp->bufferQueueCallback( caller, pBufferContext, pBufferData, dataSize, dataUsed, pItems, itemsLength );
}

// prefetch status callback
static void PrefetchStatusCallback( XAPrefetchStatusItf caller,  void *pContext, XAuint32 event ) {
	DTV_ASSERT(pContext);
	MediaPlayer *mp = (MediaPlayer *)pContext;
	mp->prefetchStatusCallback( caller, event );
}

// play status callback
void PlayCallback( XAPlayItf caller, void * pContext, XAuint32 event ) {
	DTV_ASSERT(pContext);
	MediaPlayer *mp = (MediaPlayer *)pContext;
	mp->playCallback( caller, event );
}

}	//	namespace impl

class StreamsInfo : public canvas::StreamsInfo {
public:
	StreamsInfo() {}
	virtual ~StreamsInfo() {}

protected:
	virtual void refreshImpl() {
		LDEBUG("android", "StreamsInfo refresh");
	}

	virtual void parseImpl() {
		LDEBUG("android", "StreamsInfo parse");
	}
};

MediaPlayer::MediaPlayer( canvas::System *sys ) : canvas::MediaPlayer( sys )
{
	_jni = NULL;
	_canvas = (canvas::gl::Canvas *)sys->canvas();
	_surface = NULL;

	_window = NULL;
	_oEngine = NULL;
	_iEngine = NULL;
	_oMixer = NULL;
	_oPlayer = NULL;
	_iPlayerPlay = NULL;
	_iPlayerStreamInfo = NULL;
	_iPlayerVol = NULL;
	_iPlayerBufferQueue = NULL;
	_iPlayerPrefetchStatus = NULL;

	//	State
	_enqueue = false;
	_cache = 0;
}

MediaPlayer::~MediaPlayer()
{
}

//	Initialization
bool MediaPlayer::initialize( const Rect &rect, const std::string &url ) {
	LDEBUG("android", "MediaPlayer::Initialize: rect=(%d,%d,%d,%d), url=%s",
		rect.x, rect.y, rect.w, rect.h, url.c_str());

	_surface = (canvas::gl::Surface *)_canvas->createSurface( rect );
	if (!_surface) {
		LERROR( "android", "Cannot create video overlay surface" );
		return false;
	}
	//_surface->autoFlush( true );
	_surface->setColor( ::canvas::Color() ); // Black, fully transparent
	_surface->clear();
	_surface->setZIndex( 400 );

	_jni = util::android::glue()->attachCurrentThread();

	{	//	_jniSurfaceTexture = new SurfaceTexture(_surface->texture());
		jclass jSurfaceTextureClass = (jclass)_jni->NewGlobalRef( _jni->FindClass("android/graphics/SurfaceTexture") );
		jmethodID jSurfaceTexture_Ctor = _jni->GetMethodID( jSurfaceTextureClass, "<init>", "(I)V" );
		_jniSurfaceTexture = _jni->NewGlobalRef( _jni->NewObject( jSurfaceTextureClass, jSurfaceTexture_Ctor, (int)_surface->texture() ) );
		if (!_jniSurfaceTexture) {
			LERROR( "android", "Cannot create java surface texture" );
			return false;
		}
	}

	{	//	_jniSurface = new Surface( _jniSurfaceTexture );
		jclass jSurfaceClass = (jclass)_jni->NewGlobalRef( _jni->FindClass("android/view/Surface") );
		jmethodID jSurface_Ctor = _jni->GetMethodID( jSurfaceClass, "<init>", "(Landroid/graphics/SurfaceTexture;)V" );
		_jniSurface = _jni->NewGlobalRef( _jni->NewObject( jSurfaceClass, jSurface_Ctor, _jniSurfaceTexture ) );
		if (!_jniSurface) {
			LERROR( "android", "Cannot create java surface texture" );
			return false;
		}
	}

	_window = ANativeWindow_fromSurface( _jni, _jniSurface );
	LDEBUG( "android", "Use native window: _window=%p", _window );

	return initialize( url );
}

bool MediaPlayer::initialize( const std::string &url ) {
	XAresult res;

	LDEBUG("android", "MediaPlayer::Initialize: url=%s", url.c_str());

	if (!createEngine()) {
		return false;
	}

	DTV_ASSERT( _window );

	// configure data source
	XADataLocator_AndroidBufferQueue loc_abq = { XA_DATALOCATOR_ANDROIDBUFFERQUEUE, NB_BUFFERS };
	XADataFormat_MIME format_mime = { XA_DATAFORMAT_MIME, XA_ANDROID_MIME_MP2TS, XA_CONTAINERTYPE_MPEG_TS };
	XADataSource dataSrc = { &loc_abq, &format_mime };

	// configure audio sink
	XADataLocator_OutputMix loc_outmix = { XA_DATALOCATOR_OUTPUTMIX, _oMixer };
	XADataSink audioSink = { &loc_outmix, NULL };

	// configure image video sink
	XADataLocator_NativeDisplay loc_nd = {
		XA_DATALOCATOR_NATIVEDISPLAY,        // locatorType
		(void*)_window,                      // the video sink must be an ANativeWindow created from a Surface or SurfaceTexture
		NULL                                 // hDisplay
	};
	XADataSink imageVideoSink = { &loc_nd, NULL };

	// declare interfaces to use
	XAboolean required[] = { XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE };
	XAInterfaceID iidArray[] = { XA_IID_PLAY, XA_IID_ANDROIDBUFFERQUEUESOURCE, XA_IID_STREAMINFORMATION, XA_IID_PREFETCHSTATUS };

	// create media player
	res = (*_iEngine)->CreateMediaPlayer(
		_iEngine,
		&_oPlayer,
		&dataSrc,
		NULL,
		&audioSink,
		&imageVideoSink,
		NULL,
		NULL,
		sizeof(iidArray)/sizeof(XAInterfaceID),
		iidArray,
		required
	);
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot create engine object" );
		return false;
	}

	// realize the player
	res = (*_oPlayer)->Realize( _oPlayer, XA_BOOLEAN_FALSE );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot realize player object" );
		return false;
	}

	// get the play interface
	res = (*_oPlayer)->GetInterface( _oPlayer, XA_IID_PLAY, &_iPlayerPlay );
	if (XA_RESULT_SUCCESS != res || !_iPlayerPlay) {
		LERROR( "android", "Cannot get play interface" );
		return false;
	}

	//	specify which events we want to be notified of
	res = (*_iPlayerPlay)->SetCallbackEventsMask(
		_iPlayerPlay,
		XA_PLAYEVENT_HEADATEND | XA_PLAYEVENT_HEADATMARKER | XA_PLAYEVENT_HEADATNEWPOS | XA_PLAYEVENT_HEADMOVING | XA_PLAYEVENT_HEADSTALLED
	);
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set play event mask" );
		return false;
	}

	//	Set play callback
	res = (*_iPlayerPlay)->RegisterCallback( _iPlayerPlay, impl::PlayCallback, this );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot register callback on player play interface" );
		return false;
	}

	//	Set a marker
	res = (*_iPlayerPlay)->SetMarkerPosition( _iPlayerPlay, 5000 );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set maker position on player play interface" );
		return false;
	}

	//	Set position update period
	res = (*_iPlayerPlay)->SetPositionUpdatePeriod( _iPlayerPlay, 2000 );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set postion update period on player play interface" );
		return false;
	}

	// get the volume interface
	res = (*_oPlayer)->GetInterface( _oPlayer, XA_IID_VOLUME, &_iPlayerVol );
	if (XA_RESULT_SUCCESS != res || !_iPlayerVol) {
		LERROR( "android", "Cannot get volume interface" );
		return false;
	}

	// get the Android buffer queue interface
	res = (*_oPlayer)->GetInterface( _oPlayer, XA_IID_ANDROIDBUFFERQUEUESOURCE, &_iPlayerBufferQueue );
	if (XA_RESULT_SUCCESS != res || !_iPlayerBufferQueue) {
		LERROR( "android", "Cannot get android buffer interface" );
		return false;
	}

	// specify which events we want to be notified of
	res = (*_iPlayerBufferQueue)->SetCallbackEventsMask( _iPlayerBufferQueue, XA_ANDROIDBUFFERQUEUEEVENT_PROCESSED );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set buffer queue event mask callback" );
		return false;
	}

	// register the callback from which OpenMAX AL can retrieve the data to play
	res = (*_iPlayerBufferQueue)->RegisterCallback( _iPlayerBufferQueue, impl::AndroidBufferQueueCallback, this );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot register buffer queue callback" );
		return false;
	}

	// get the stream information interface (for video size)
	res = (*_oPlayer)->GetInterface( _oPlayer, XA_IID_STREAMINFORMATION, &_iPlayerStreamInfo );
	if (XA_RESULT_SUCCESS != res || !_iPlayerStreamInfo) {
		LERROR( "android", "Cannot get stream information interface" );
		return false;
	}

	// we want to be notified of the video size once it's found, so we register a callback for that
	res = (*_iPlayerStreamInfo)->RegisterStreamChangeCallback( _iPlayerStreamInfo, impl::StreamChangeCallback, this );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot register stream change callback" );
		return false;
	}

	// get the prefetch status interface
	res = (*_oPlayer)->GetInterface( _oPlayer, XA_IID_PREFETCHSTATUS, &_iPlayerPrefetchStatus );
	if (XA_RESULT_SUCCESS != res || !_iPlayerPrefetchStatus) {
		LERROR( "android", "Cannot get player prefetch status interface" );
		return false;
	}

	// register prefetch status callback
	res = (*_iPlayerPrefetchStatus)->RegisterCallback( _iPlayerPrefetchStatus, impl::PrefetchStatusCallback, this );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot register callback on player prefetch status interface" );
		return false;
	}
	res = (*_iPlayerPrefetchStatus)->SetCallbackEventsMask( _iPlayerPrefetchStatus, XA_PREFETCHEVENT_FILLLEVELCHANGE | XA_PREFETCHEVENT_STATUSCHANGE );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set events mask on player prefetch status interface" );
		return false;
	}

	// prepare the player
	res = (*_iPlayerPlay)->SetPlayState( _iPlayerPlay, XA_PLAYSTATE_PAUSED );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot set player state to paused" );
		return false;
	}

	// // set the volume
	// res = (*_iPlayerVol)->SetVolumeLevel( _iPlayerVol, 0 );
	// if (XA_RESULT_SUCCESS != res) {
	// 	LERROR( "android", "Cannot set volume" );
	// 	return false;
	// }

	return true;
}

void MediaPlayer::finalize() {
	LDEBUG("android", "MediaPlayer::Finalize");
	// destroy streaming media player object, and invalidate all associated interfaces
	if (_oPlayer) {
		(*_oPlayer)->Destroy(_oPlayer);
		_oPlayer = NULL;
		_iPlayerPlay = NULL;
		_iPlayerBufferQueue = NULL;
		_iPlayerPrefetchStatus = NULL;
		_iPlayerStreamInfo = NULL;
		_iPlayerVol = NULL;
	}

	// destroy output mix object, and invalidate all associated interfaces
	if (_oMixer) {
		(*_oMixer)->Destroy(_oMixer);
		_oMixer = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (_oEngine) {
		(*_oEngine)->Destroy(_oEngine);
		_oEngine = NULL;
		_iEngine = NULL;
	}

	if (_surface) {
		canvas::Surface *s = _surface;
		_canvas->destroy( s );
		_surface = NULL;
	}

	util::android::glue()->detachCurrentThread( &_jni );
}

//	Stream operations
bool MediaPlayer::startImpl( const std::string &url ) {
	bool check = initialize(bounds(), url);
	if (check) {
		LDEBUG("android", "Play");
		DTV_ASSERT(_iPlayerPlay);
		XAresult res = (*_iPlayerPlay)->SetPlayState( _iPlayerPlay, XA_PLAYSTATE_PLAYING );
		DTV_ASSERT( XA_RESULT_SUCCESS == res );

		//	Setup queue state
		_mutex.lock();
		_enqueue = true;
		_cache = -1;
		_mutex.unlock();
		_cWakeup.notify_all();
	} else {
		LERROR("android", "Fail to initialize android media player");
	}
	return check;
}

void MediaPlayer::stopImpl() {
	LDEBUG("android", "Stop");

	_mutex.lock();
	_enqueue = false;
	_mutex.unlock();
	_cWakeup.notify_all();

	DTV_ASSERT(_iPlayerPlay);
	XAresult res = (*_iPlayerPlay)->SetPlayState( _iPlayerPlay, XA_PLAYSTATE_STOPPED );
	DTV_ASSERT( XA_RESULT_SUCCESS == res );

	finalize();
}

void MediaPlayer::pauseImpl( bool needPause ) {
	LDEBUG("android", "Pause: value=%d", needPause);
	DTV_ASSERT(_iPlayerPlay);
	XAresult res = (*_iPlayerPlay)->SetPlayState( _iPlayerPlay, needPause ? XA_PLAYSTATE_PAUSED : XA_PLAYSTATE_PLAYING );
	DTV_ASSERT( XA_RESULT_SUCCESS == res );
}

//	Volume
void MediaPlayer::muteImpl( bool needsMute ) {
	LDEBUG("android", "mute: value=%d", needsMute);
}

void MediaPlayer::volumeImpl( Volume vol ) {
	LDEBUG("android", "volume: value=%d", vol);
	if (_iPlayerVol) {
		XAresult res = (*_iPlayerVol)->SetVolumeLevel( _iPlayerVol, vol );
		DTV_ASSERT( XA_RESULT_SUCCESS == res );
	}
}

//	Position/size
void MediaPlayer::moveResizeImpl( const Rect &rect ) {
	LDEBUG("android", "Move and resize rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h);
}

//	Feed data
void MediaPlayer::feed( util::Buffer *buf ) {
	DTV_ASSERT(buf);

	{	//	Wait for signal to enqueue
		boost::unique_lock<boost::mutex> lock( _mutex );
		while (_enqueue && _cache >= NB_BUFFERS) {
			_cWakeup.wait( lock );
		}

		if (_enqueue && _cache < NB_BUFFERS) {
			//LDEBUG("android", "MediaPlayer::feed: buf=%p, enqueue=%d, cache=%d", buf->buffer(), _enqueue, _cache );

			DTV_ASSERT(_iPlayerBufferQueue);
			XAresult res;
			if (_cache == -1) {
				//	Signal discontinuity
				XAAndroidBufferItem items[1];
				items[0].itemKey = XA_ANDROID_ITEMKEY_DISCONTINUITY;
				items[0].itemSize = 0;
				// DISCONTINUITY message has no parameters,
				//   so the total size of the message is the size of the key
				//   plus the size if itemSize, both XAuint32
				res = (*_iPlayerBufferQueue)->Enqueue(
					_iPlayerBufferQueue,
					NULL,
					buf->data(),
					buf->length(),
					items,
					sizeof(XAuint32)*2
				);
				_cache = 1;
			}
			else {
				// XAAndroidBufferItem items[1];
				// items[0].itemKey = XA_ANDROID_ITEMKEY_BUFFERQUEUEEVENT;
				// items[0].itemSize = 4;

				res = (*_iPlayerBufferQueue)->Enqueue(
					_iPlayerBufferQueue,
					NULL,
					buf->data(),
					buf->length(),
					NULL,
					0
				);
				_cache++;
			}
			DTV_ASSERT(XA_RESULT_SUCCESS == res);
		}
	}
}

//	Aux
bool MediaPlayer::createEngine() {
	XAresult res;

	// create engine
	res = xaCreateEngine( &_oEngine, 0, NULL, 0, NULL, NULL );
	if (XA_RESULT_SUCCESS != res || !_oEngine) {
		LERROR( "android", "Cannot create engine object" );
		return false;
	}

	// realize the engine
	res = (*_oEngine)->Realize( _oEngine, XA_BOOLEAN_FALSE );
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot realize engine object" );
		return false;
	}

	// get the engine interface, which is needed in order to create other objects
	res = (*_oEngine)->GetInterface( _oEngine, XA_IID_ENGINE, &_iEngine );
	if (XA_RESULT_SUCCESS != res || !_iEngine) {
		LERROR( "android", "Cannot get engine interface" );
		return false;
	}

	// create output mix
	res = (*_iEngine)->CreateOutputMix( _iEngine, &_oMixer, 0, NULL, NULL);
	if (XA_RESULT_SUCCESS != res || !_oMixer) {
		LERROR( "android", "Cannot create mixer object" );
		return false;
	}

	// realize the output mix
	res = (*_oMixer)->Realize( _oMixer, XA_BOOLEAN_FALSE);
	if (XA_RESULT_SUCCESS != res) {
		LERROR( "android", "Cannot realize engine object" );
		return false;
	}

	return true;
}

void MediaPlayer::streamChangeCallback(
		XAStreamInformationItf caller,
		XAuint32 eventId,
		XAuint32 streamIndex,
		void * /*pEventData*/ )
{
	LDEBUG( "android", "streamChangeCallback called for stream %u", streamIndex );

	if (eventId == XA_STREAMCBEVENT_PROPERTYCHANGE) {
		/* From spec 1.0.1:
		"This event indicates that stream property change has occurred.
		The streamIndex parameter identifies the stream with the property change.
		The pEventData parameter for this event is not used and shall be ignored."
		*/
		XAresult res;
		XAuint32 domain;
		DTV_ASSERT(caller);
		res = (*caller)->QueryStreamType(caller, streamIndex, &domain);
		DTV_ASSERT( XA_RESULT_SUCCESS == res );
		if (domain == XA_DOMAINTYPE_VIDEO) {
			XAVideoStreamInformation videoInfo;
			res = (*caller)->QueryStreamInformation(caller, streamIndex, &videoInfo);
			DTV_ASSERT( XA_RESULT_SUCCESS == res );
			LDEBUG("android", "Found video size %u x %u, codec ID=%u, frameRate=%u, bitRate=%u, duration=%u ms",
				videoInfo.width, videoInfo.height, videoInfo.codecId, videoInfo.frameRate,
				videoInfo.bitRate, videoInfo.duration );
		}
		else {
			LWARN( "android", "Unexpected OpenMAX stream type domain %u", domain );
		}
	}
	else {
		LWARN( "android", "Unexpected OpenMAX stream event ID %u", eventId );
	}
}

XAresult MediaPlayer::bufferQueueCallback(
	XAAndroidBufferQueueItf /*caller*/,
	void * /*pBufferContext*/,
	void * /*pBufferData*/,
	XAuint32 /*dataSize*/,
	XAuint32 /*dataUsed*/,
	const XAAndroidBufferItem * /*pItems*/,
	XAuint32 /*itemsLength*/ )
{
	// LDEBUG( "android", "Buffer queue called: pBufferData=%p, dataSize=%d, dataUsed=%d, pItems=%p, itemsLength=%d",
	// 	pBufferData, dataSize, dataUsed, pItems, itemsLength );

	_mutex.lock();
	if (_cache > 0) {
		_cache--;
	}
	_mutex.unlock();
	_cWakeup.notify_all();

	return XA_RESULT_SUCCESS;
}

void MediaPlayer::prefetchStatusCallback( XAPrefetchStatusItf caller, XAuint32 event ) {
	XAresult res;

	//	Status change
	if (event & XA_PREFETCHEVENT_STATUSCHANGE) {
		XAuint32 status;
		res = (*caller)->GetPrefetchStatus(caller, &status);
		DTV_ASSERT( XA_RESULT_SUCCESS == res );

		XAuint32 state;
		res = (*_iPlayerPlay)->GetPlayState( _iPlayerPlay, &state );
		DTV_ASSERT( XA_RESULT_SUCCESS == res );

		LDEBUG( "android", "PrefetchEventCallback: status changed: status=%d, player_state=%d", status, state );

		//TODO: Handle status
		// XA_PREFETCHSTATUS_UNDERFLOW 1
		// XA_PREFETCHSTATUS_SUFFICIENTDATA 2
		// XA_PREFETCHSTATUS_OVERFLOW 3
	}
	else if (event & XA_PREFETCHEVENT_FILLLEVELCHANGE) {
		//	Get Level
		XApermille level = 0;
		res = (*caller)->GetFillLevel(caller, &level);
		DTV_ASSERT( XA_RESULT_SUCCESS == res );

		LDEBUG( "android", "PrefetchEventCallback: level changed: level=%d", level );
	}
	else {
		LWARN( "android", "PrefetchEventCallback: unknown event: event=%08x", event );
	}
}

void MediaPlayer::playCallback( XAPlayItf /*caller*/, XAuint32 event ) {
	LDEBUG( "android", "PlayEventCallback: event=%08x", event );
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	LDEBUG( "android", "Switch video stream: id=%d", id );
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	LDEBUG( "android", "Switch audio stream: id=%d", id );
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	LDEBUG( "android", "Switch subtitle stream: id=%d", id );
	return true;
}

}
}
}

