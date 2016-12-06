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
#include "avsource.h"
#include "../../gl/canvas.h"
#include "../../gl/surface.h"
#include "../../../rect.h"
#include "../../../point.h"
#include "../../../system.h"
#include "../../../surface.h"
#include <util/android/android.h>
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/thread.hpp>
#include <jni.h>
#include <android/native_window_jni.h>
#include <media/stagefright/dtvmediasource.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/foundation/ABuffer.h>

namespace canvas {
namespace android {
namespace stagefright {

namespace impl {

static dtv::DTVMediaSource _wrapper;

static a::status_t start( void *ctx, a::MetaData *params ) {
	AVFormatSource *av = (AVFormatSource *)ctx;
	DTV_ASSERT(ctx);
	return av->start( params );
}

static a::status_t stop( void *ctx ) {
	AVFormatSource *av = (AVFormatSource *)ctx;
	DTV_ASSERT(ctx);
	return av->stop();	
}

a::sp<a::MetaData> getFormat( void *ctx ) {
	AVFormatSource *av = (AVFormatSource *)ctx;
	DTV_ASSERT(ctx);
	return av->getFormat();	
}

a::status_t read( void *ctx, a::MediaBuffer **buffer, const a::MediaSource::ReadOptions *options ) {
	AVFormatSource *av = (AVFormatSource *)ctx;
	DTV_ASSERT(ctx);
	return av->read( buffer, options );	
}

}

MediaPlayer::MediaPlayer( canvas::System *sys )
{
	_canvas = (canvas::gl::Canvas *)sys->canvas();
	_surface = NULL;
	_src = NULL;
	_jni = NULL;
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
	if (!_jni) {
		LERROR( "android", "Cannot attach to java context" ); 
		return false;
	}

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
	
	return this->initialize( url );
}

bool MediaPlayer::initialize( const std::string &url ) {
	LDEBUG("android", "MediaPlayer::Initialize: url=%s", url.c_str());

	_src = new AVFormatSource();
	if (!_src->initialize( url )) {
		LERROR("android", "Cannot initialize mediaplayer: url=%s", url.c_str());		
		return false;
	}
	return true;
}

void MediaPlayer::finalize() {
	LDEBUG("android", "MediaPlayer::Finalize");

	if (_src) {
		//	TODO: Wakeup thread
		_src->finalize();
		DEL(_src);
	}

	if (_surface) {
		canvas::Surface *s = _surface;
		_canvas->destroy( s );
		_surface = NULL;
	}

	util::android::glue()->detachCurrentThread( &_jni );
}

//	Stream operations
void MediaPlayer::play() {
	LDEBUG("android", "Play");
}

void MediaPlayer::stop() {
	LDEBUG("android", "Stop");
}

void MediaPlayer::pause( bool needPause ) {
	LDEBUG("android", "Pause: value=%d", needPause);
}

//	Volume
void MediaPlayer::mute( bool needMute ) {
 	LDEBUG("android", "mute: value=%d", needMute);
}

void MediaPlayer::setVolume( Volume vol ) {
	LDEBUG("android", "volume: value=%d", vol);
}

Volume MediaPlayer::getVolume() const {
	return 10;
}

//	Position/size
void MediaPlayer::setFullScreen() {
	LDEBUG("android", "set full screen" );	
}

void MediaPlayer::move( const Point &point ) {
	LDEBUG("android", "move(%d,%d)", point.x, point.y);
}

void MediaPlayer::resize( const Size &size ) {
	LDEBUG("android", "resize(%d,%d)", size.w, size.h);
}

//	Feed data
void MediaPlayer::feed( util::Buffer *buf ) {
	DTV_ASSERT(buf);
}

void MediaPlayer::process() {
	LINFO("android", "Process video thread begin" );

	//	Setup wrapper
	impl::_wrapper.context = _src;
	impl::_wrapper.start = impl::start;
	impl::_wrapper.stop = impl::stop;
	impl::_wrapper.getFormat = impl::getFormat;
	impl::_wrapper.read = impl::read;
		
	// Initialize the AVFormatSource from a video file
	a::sp<a::MediaSource> videoSource = new dtv::DTVSource(&impl::_wrapper);

	a::sp<ANativeWindow> mNativeWindow = _window;

	// Once we get an MediaSource, we can encapsulate it with the OMXCodec now
	a::OMXClient mClient;
	mClient.connect();
	a::sp<a::MediaSource> mVideoDecoder = a::OMXCodec::Create(mClient.interface(), videoSource->getFormat(), false, videoSource, NULL, 0, mNativeWindow);
	mVideoDecoder->start();

	// Just loop to read decoded frames from mVideoDecoder
	for (;;) {
		a::MediaBuffer *mVideoBuffer;
		a::MediaSource::ReadOptions options;
		a::status_t err = mVideoDecoder->read(&mVideoBuffer, &options);
		if (err == a::OK) {
			LDEBUG( "android", "Render video frame: size=%d", mVideoBuffer->range_length() );
			if (mVideoBuffer->range_length() > 0) {
				// If video frame availabe, render it to mNativeWindow
				a::sp<a::MetaData> metaData = mVideoBuffer->meta_data();
				int64_t timeUs = 0;
				metaData->findInt64(a::kKeyTime, &timeUs);
				native_window_set_buffers_timestamp(mNativeWindow.get(), timeUs * 1000);
				err = mNativeWindow->queueBuffer(mNativeWindow.get(), mVideoBuffer->graphicBuffer().get());
				if (err == 0) {
					metaData->setInt32(a::kKeyRendered, 1);
				}
			}
			mVideoBuffer->release();
		}
	}

	// Finally release the resources
	videoSource.clear();
	mVideoDecoder->stop();
	mVideoDecoder.clear();
	mClient.disconnect();

	LINFO("android", "Process video thread end" );	
}

}
}
}

