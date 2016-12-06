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
#include "../../point.h"
#include "../../system.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/url.h>
#include <util/task/dispatcher.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <stdlib.h>
#include <string.h>

#define GST_MAX_VOL 10
#define GST_MIN_VOL 0

namespace canvas {
namespace gst {

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

static GstBusSyncReply gst_setupWinID(GstBus *, GstMessage *message, void *user_data) {
	GstBusSyncReply returnValue = GST_BUS_DROP;
	if (! gst_is_video_overlay_prepare_window_handle_message (message)) {
		returnValue = GST_BUS_PASS;
	} else {
		MediaPlayer *mp = (MediaPlayer *) user_data;
		DTV_ASSERT(mp);
		mp->setWinId();
	}
	return returnValue;
}

MediaPlayer::MediaPlayer( System *sys )
	: canvas::MediaPlayerImpl( sys )
{
	_loop = NULL;
	_pipeline = NULL;
	_needsMute = false;
	_volume = -1;
}

MediaPlayer::~MediaPlayer()
{
}

//	Initialization
bool MediaPlayer::startImpl( const std::string &url ) {
	LDEBUG( "gst", "Initialize: url=%s", url.c_str() );
	gst_init(NULL, NULL);
	_loop = g_main_loop_new (NULL, FALSE);
	if (!_loop) {
		LDEBUG( "gst", "Error, cannot create gst loop" );
		return false;
	}
	
	_pipeline = gst_element_factory_make ("playbin", "play");
	if (!_pipeline) {
		LDEBUG( "gst", "Error, cannot get playbin2 gst element" );
		gst_object_unref(_loop);
		return false;
	}

	{	//	Setup uri
		std::string uri;
		util::Url tmpUrl(url);
		if (tmpUrl.type() == "rtp") {
			uri = "udp://" + url.substr(6, url.length() - 6);
		}
		else if (tmpUrl.type() == "udp") {
			uri = url.c_str();
		}
		else {
			uri = "file://" + url.substr(0, url.length());
		}

		LDEBUG( "gst", "Start play: uri=%s", uri.c_str() ); 
		g_object_set (G_OBJECT (_pipeline), "uri", uri.c_str(), NULL);
		g_object_set (G_OBJECT(_pipeline), "flags", 0x00000417, NULL); // saco deinterlace
	}

	system()->dispatcher()->registerTarget( this, "canvas::gst::MediaPlayer" );
	setupOverlay();
	{	//	Add bus watcher
		GstBus *bus;
		bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
		gst_bus_add_watch(bus, gst_bus_call, this);
		gst_bus_set_sync_handler (bus, (GstBusSyncHandler) gst_setupWinID, this, NULL);
		gst_object_unref(bus);
		g_signal_connect (_pipeline, "video-tags-changed", G_CALLBACK (cb_video_changed), this);
	}
	
	if (_needsMute) {
		g_object_set(G_OBJECT(_pipeline), "mute", _needsMute ? TRUE : FALSE, NULL);
	}
	
	if (_volume != -1) {
		gdouble vol = _volume;
		g_object_set(G_OBJECT(_pipeline), "volume", vol, NULL);
	}

	_thread = boost::thread( boost::bind(&MediaPlayer::onRun,this) );

	static_cast<StreamsInfo*>(sInfo())->init(_pipeline);

	//	play the media_player
	gst_element_set_state(_pipeline, GST_STATE_PLAYING);

	return true;
}

void MediaPlayer::setWinId(){
	VideoDescription desc;
	if (getVideoDescription( desc )) {
#ifndef _WIN32
		LDEBUG( "gst", "Using WinID=0x%x", desc.winID );
		guintptr window_handle = desc.winID;

		GstElement *videosink;
		g_object_get(G_OBJECT(_pipeline), "video-sink", &videosink, NULL);
		gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (videosink), window_handle);
#endif
	}
}

void MediaPlayer::stopImpl() {
	LDEBUG( "gst", "stop" );
	DTV_ASSERT(_loop);
	DTV_ASSERT(_pipeline);

	static_cast<StreamsInfo*>(sInfo())->fin();
	system()->dispatcher()->unregisterTarget( this );

	//	Dettach to stop async events
	gst_element_set_state(_pipeline, GST_STATE_NULL);
	g_main_loop_quit(_loop);
	
	_thread.join();
	LDEBUG( "gst", "join" );

	gst_object_unref(GST_OBJECT(_pipeline));
	g_main_loop_unref(_loop);
	destroyOverlay();
}

void MediaPlayer::pauseImpl( bool pause ) {
	LDEBUG( "gst", "Pause: param=%d", pause );
	DTV_ASSERT(_pipeline);
	//	Pause playing
	gst_element_set_state(_pipeline, pause ? GST_STATE_PAUSED : GST_STATE_PLAYING);
}

//	Volume
void MediaPlayer::muteImpl( bool needMute ) {
	LDEBUG( "gst", "Mute: param=%d", needMute );
	if (_pipeline) {
		g_object_set(G_OBJECT(_pipeline), "mute", needMute ? 1 : 0, NULL);
	} else {
		_needsMute = needMute;
	}
}

void MediaPlayer::volumeImpl( Volume volume ) {
	LDEBUG( "gst", "Volume: param=%d", volume );

	float relation = (float)(GST_MAX_VOL-GST_MIN_VOL) / (float)(MAX_VOLUME-MIN_VOLUME);
	_volume = boost::math::iround( ((float)volume * relation + (float)GST_MIN_VOL) );

	if (_pipeline) {
		DTV_ASSERT( volume >= GST_MIN_VOL && volume <= GST_MAX_VOL);
		g_object_set(G_OBJECT(_pipeline), "volume", _volume, NULL);
	}
}

//	Aux
canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int /*id*/ ) {
	return true; //TODO
}

bool MediaPlayer::switchAudioStreamImpl( int /*id*/ ) {
	return true; //TODO
}

bool MediaPlayer::switchSubtitleStreamImpl( int /*id*/ ) {
	return true; //TODO
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

void MediaPlayer::onRun() {
	LDEBUG( "gst", "Thread started" );
	g_main_loop_run(_loop);
	LDEBUG( "gst", "Thread end" );
}

}
}

