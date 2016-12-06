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
#include "streaminfo.h"
#include <aml/aml.h>
#include <aml/ts.h>
#include <canvas/point.h>
#include <canvas/rect.h>
#include <canvas/types.h>
#include <canvas/streaminfo.h>
#include <canvas/window.h>
#include <canvas/screen.h>
#include <canvas/system.h>
#include <util/task/dispatcher.h>
#include <util/buffer.h>
#include <util/url.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/math/special_functions/round.hpp>
#ifdef ENABLE_GST_VIDEO
#	include <gst/gst.h>
#	define GST_MAX_VOL 10
#	define GST_MIN_VOL 0
#endif

namespace aml {

using namespace canvas;

#ifdef ENABLE_GST_VIDEO

namespace impl {

static gboolean gst_bus_call(GstBus * /*bus*/, GstMessage *msg, void *user_data) {
	MediaPlayer *mp = (MediaPlayer *)user_data;
	DTV_ASSERT(mp);

	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS: {
			LDEBUG( "gst", "End-of-stream" );
			mp->endLoop();
			break;
		}
		case GST_MESSAGE_BUFFERING: {
			gint percent;
			gst_message_parse_buffering (msg, &percent);
			mp->onBuffering( percent );
			break;
		}
		case GST_MESSAGE_ERROR: {
			GError *err;
			gst_message_parse_error(msg, &err, NULL);
			mp->onError( err->message );
			g_error_free(err);
			break;
		}
		default:
			break;
	}

	return true;
}

static void cb_video_changed(GstElement */*playbin*/, gint /*stream*/, gpointer user_data) {
	DTV_ASSERT(user_data);
	MediaPlayer *mp = (MediaPlayer *)user_data;
	mp->onParsedEvent();
}

}

#endif	//	ENABLE_VIDEO

MediaPlayer::MediaPlayer( System *sys )
	: canvas::MediaPlayer( sys )
{
	_loop = NULL;
	_pipeline = NULL;
	_needsMute = false;
	_volume = -1;

	_feed = false;
	_tsPlayer = new ts::Player();
}

MediaPlayer::~MediaPlayer()
{
	delete _tsPlayer;
	DTV_ASSERT(!_loop);
	DTV_ASSERT(!_pipeline);
}

//	Media operations
bool MediaPlayer::startImpl( const std::string &data ) {
	LDEBUG( "aml", "Start: url=%s", data.c_str() );
	util::Url url(data);

	video::enable(true);

	if (url.type() == "feed" || url.type() == "srvdtv") {
		DTV_ASSERT(_feed == false);

		ts::Params params;
		params.videoPID = -1;
		params.audioPID = -1;

		{	//	Video
			int pid;
			int type;
			if (url.getParam( "videoPID", pid ) &&
				url.getParam( "videoType", type ))
			{
				params.videoPID = pid;
				params.videoType = type;
			}
		}

		{	//	Audio
			int pid;
			int type;
			if (url.getParam( "audioPID", pid ) &&
				url.getParam( "audioType", type ))
			{
				params.audioPID = pid;
				params.audioType = type;
			}
		}

		_feed = true;
		static_cast<StreamsInfo*>(sInfo())->init(_tsPlayer);
		return _tsPlayer->play( params );
	}
	else {
#ifdef USE_GST_VIDEO
		_loop = g_main_loop_new (NULL, FALSE);
		if (!_loop) {
			LDEBUG( "aml", "Error, cannot create gst loop" );
			return false;
		}

		_pipeline = gst_element_factory_make ("playbin2", "play");
		if (!_pipeline) {
			LDEBUG( "aml", "Error, cannot get playbin2 gst element" );
			gst_object_unref(_loop);
			_loop = NULL;
			return false;
		}

		{	//	Add bus watcher
			GstBus *bus;
			bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
			gst_bus_add_watch(bus, impl::gst_bus_call, this);
			gst_object_unref(bus);
		}

		//	Setup vide changed event
		g_signal_connect (_pipeline, "video-tags-changed", G_CALLBACK (impl::cb_video_changed), this);

		{	//	Setup uri
			std::string uri;
			if (url.type() == "rtp") {
				uri = "udp://" + data.substr(6, data.length() - 6);
			}
			else if (url.type() == "udp") {
				uri = data.c_str();
			}
			else if (url.type().empty()) {
				uri = "file://" + data;
			}
			else {
				uri = data;
			}

			LDEBUG( "aml", "Start play: uri=%s", uri.c_str() );
			g_object_set (G_OBJECT (_pipeline), "uri", uri.c_str(), NULL);
			//g_object_set (G_OBJECT(_pipeline), "flags", 0x00000417, NULL); // saco deinterlace
		}

		system()->dispatcher()->registerTarget( this, "aml::MediaPlayer" );

		if (_needsMute) {
			g_object_set(G_OBJECT(_pipeline), "mute", _needsMute ? TRUE : FALSE, NULL);
		}

		if (_volume != -1) {
			volumeImpl( _volume );
		}

		_thread = boost::thread( boost::bind(&MediaPlayer::onRun,this) );

		static_cast<StreamsInfo*>(sInfo())->init(_pipeline);

		//	play the media_player
		gst_element_set_state(_pipeline, GST_STATE_PLAYING);

#endif
		return true;
	}

	return false;
}

void MediaPlayer::stopImpl() {
	LDEBUG( "aml", "Stop" );

	if (_feed) {
		_feed = false;
		_tsPlayer->stop();
	}
	else if (_loop) {
#ifdef USE_GST_VIDEO
		DTV_ASSERT(_pipeline);

		static_cast<StreamsInfo*>(sInfo())->fin();
		system()->dispatcher()->unregisterTarget( this );

		//	Dettach to stop async events
		gst_element_set_state(_pipeline, GST_STATE_NULL);
		g_main_loop_quit(_loop);

		LDEBUG( "aml", "join thread" );
		_thread.join();

		gst_object_unref(GST_OBJECT(_pipeline));
		_pipeline = NULL;
		g_main_loop_unref(_loop);
		_loop = NULL;
#endif
	}
}

void MediaPlayer::pauseImpl( bool pause ) {
	LDEBUG( "aml", "Pause: param=%d, _feed=%d", pause, _feed );
	if (_loop) {
#ifdef USE_GST_VIDEO
		gst_element_set_state(_pipeline, pause ? GST_STATE_PAUSED : GST_STATE_PLAYING);
#endif
	}
}

bool MediaPlayer::parsedOnPlay() const {
	return true;
}

//	Position/size
void MediaPlayer::moveResizeImpl( const Rect &rect ) {
	LDEBUG( "aml", "Move and resize: rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h );
	Rect tmp(rect);
	translateToWindow( tmp );
	video::setAxis( tmp.x, tmp.y, tmp.x+tmp.w, tmp.y+tmp.h );
}

//	Volume
void MediaPlayer::muteImpl( bool needMute ) {
	LDEBUG( "aml", "Mute: param=%d", needMute );
	if (_feed) {
		_tsPlayer->setMute( needMute );
	}
	else if (_loop) {
#ifdef USE_GST_VIDEO
		g_object_set(G_OBJECT(_pipeline), "mute", needMute ? 1 : 0, NULL);
#endif
	} else {
		_needsMute = needMute;
	}
}

void MediaPlayer::volumeImpl( Volume vol ) {
	if (_feed) {
		float tmp =  (vol * 1.0) / float(MAX_VOLUME);
		LDEBUG( "aml", "Volume: param=%d, set=%f", vol, tmp );
		_tsPlayer->setVolume( tmp );
	}
	else if (_loop) {
#ifdef USE_GST_VIDEO
		float relation = (float)(GST_MAX_VOL-GST_MIN_VOL) / (float)(MAX_VOLUME-MIN_VOLUME);
		long volume = boost::math::iround( ((float)vol * relation + (float)GST_MIN_VOL) );
		DTV_ASSERT( volume >= GST_MIN_VOL && volume <= GST_MAX_VOL);
		g_object_set(G_OBJECT(_pipeline), "volume", volume, NULL);
#endif
	}
	else {
		_volume = vol;
	}
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	LDEBUG("aml", "Switch video stream: id=%d", id);
	//TODO:
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	LDEBUG("aml", "Switch audio stream: id=%d", id);
	//TODO:
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	LDEBUG("aml", "Switch subtitle stream: id=%d", id);
	//TODO:
	return true;
}

//	Feed data
void MediaPlayer::feed( util::Buffer * /*buf*/ ) {	//	TODO
	if (_feed) {
		// int ret;
		// int isize = 0;
		// util::BYTE *buffer = buf->bytes();
		// int len = buf->length();
		// do {
		// 	ret = codec_write( &_codec, buffer+isize, len );
		// 	if (ret < 0) {
		// 		if (errno != EAGAIN) {
		// 			LERROR( "aml", "Write data failed, errno %d", errno);
		// 			break;
		// 		}
		// 	}
		// 	else {
		// 		isize += ret;
		// 	}
		// } while(isize < len);
	}
}

//	Gstreamer notifications
void MediaPlayer::onRun() {
	LDEBUG( "aml", "Mediaplayer thread started" );
#ifdef USE_GST_VIDEO
	g_main_loop_run(_loop);
#endif
	LDEBUG( "gst", "Mediaplayer thread end" );
}

void MediaPlayer::endLoop() {
	system()->dispatcher()->post( this, boost::bind(&MediaPlayer::onMediaCompleted,this) );
}

void MediaPlayer::onError( const std::string &errMsg ) {
	system()->dispatcher()->post( this, boost::bind(&MediaPlayer::onMediaError,this, errMsg) );
}

void MediaPlayer::onParsedEvent() {
	system()->dispatcher()->post( this, boost::bind(&MediaPlayer::onMediaParsed,this) );
}

void MediaPlayer::onBuffering( int percent ) {
	LDEBUG( "gst", "Buffering %d", percent );
	// if (percent == 100) {
	// 	gst_element_set_state (_pipeline, GST_STATE_PLAYING);
	// } else {
	// 	gst_element_set_state (_pipeline, GST_STATE_PAUSED);
	// }
}

}

