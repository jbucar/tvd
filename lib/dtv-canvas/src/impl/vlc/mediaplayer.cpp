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
#include "streaminfo.h"
#include "../../point.h"
#include "../../types.h"
#include "../../window.h"
#include "../../system.h"
#include <util/task/dispatcher.h>
#include <util/url.h>
#include <util/assert.h>
#include <util/log.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <vlc/libvlc_version.h>
#include <vlc/vlc.h>
#include <stdlib.h>
#include <string.h>

#define VLC_MAX_VOL 100
#define VLC_MIN_VOL 40

namespace canvas {
namespace vlc {

namespace impl {

static void vlc_event(const libvlc_event_t *p_event, void *p_user_data) {
	if (p_event->type == libvlc_MediaPlayerEndReached) {
		DTV_ASSERT(p_user_data);
		MediaPlayer *mp = (MediaPlayer *)p_user_data;
		mp->onEndReached();
	}
	else if (p_event->type == libvlc_MediaParsedChanged) {
		DTV_ASSERT(p_user_data);
		MediaPlayer *mp = (MediaPlayer *)p_user_data;
		mp->onParsedEvent();
	}
}

}	//	namespace impl


MediaPlayer::MediaPlayer( System *sys, Player *player )
	: canvas::MediaPlayerImpl( sys )
{
	_player = player;
	_mp = NULL;
	_needsMute = false;
	_volume = -1;
}

MediaPlayer::~MediaPlayer()
{
	if (_mp) {
		libvlc_media_player_release( _mp );
	}
}

bool MediaPlayer::getParam( util::Url *url, const std::string &param1, const std::string &param2, std::string &result ) {
	bool ret=false;
	int pid;
	int type;
	if (url->getParam( param1, pid ) &&
		url->getParam( param2, type ))
	{
		result  = boost::lexical_cast<std::string>(pid);
		result += ":";
		result += boost::lexical_cast<std::string>(type);
		ret=true;
	}
	return ret;
}

bool MediaPlayer::startImpl( const std::string &data ) {
	util::Url url(data);
	LDEBUG( "vlc", "Start: url=%s", data.c_str() );
	DTV_ASSERT(!_mp);

	//	Create media from url
	libvlc_media_t *m;
	if (url.type() == "srvdtv") {
		m = libvlc_media_new_path( instance(), "/dev/dvb/adapter0/dvr0" );
		if (m) {
			std::string pmt = "ts-extra-pmt=";
			int streams = 0;
			std::string audio, video;

			//	Audio
			if (getParam( &url, "audioPID", "audioType", audio )) {
				streams++;
			}

			//	Video
			if (getParam( &url, "videoPID", "videoType", video )) {
				streams++;
			}

			if (!streams) {
				LWARN( "vlc", "No stream to play" );
				return false;
			}

			pmt += boost::lexical_cast<std::string>(streams);
			pmt += "=";

			if (!video.empty()) {
				pmt += video;
			}

			if (!audio.empty()) {
				pmt += ",";
				pmt += audio;
			}

			{
				int pcrPID;
				if (url.getParam( "pcrPID", pcrPID )) {
					pmt += ",";
					pmt += boost::lexical_cast<std::string>(pcrPID);
					pmt += ":pcr";
				}
			}

			LDEBUG( "vlc", "PMT parameters: pmt=%s", pmt.c_str() );

			//	Add vlc options
			libvlc_media_add_option( m, "demux=ts" );
			libvlc_media_add_option( m, pmt.c_str() );
		}
	}
	else if (url.type().empty()) {
		m=libvlc_media_new_path( instance(), data.c_str() );
	}
	else {
		m=libvlc_media_new_location( instance(), data.c_str() );
	}

	if (!m) {
		LERROR( "vlc", "Cannot initialize new media from url: url=%s", data.c_str() );
		return false;
	}

	//	Add common parameters
	libvlc_media_add_option( m, "verbose=1000" );
	libvlc_media_add_option( m, "no-osd" );
	libvlc_media_add_option( m, "ffmpeg-hw" );

	//	Create a media player playing environement
	_mp = libvlc_media_player_new_from_media(m);

	//	No need to keep the media now
	libvlc_media_release (m);

	if (!_mp) {
		LERROR( "vlc", "Cannot create media player from url: url=%s", data.c_str() );
		return false;
	}

	{	//	SetUp overlay and get WindowID
		setupOverlay();
		VideoDescription desc;
		if (getVideoDescription( desc )) {
		#ifndef _WIN32
			LDEBUG( "vlc", "Using WinID=0x%x", desc.winID );
			libvlc_media_player_set_xwindow( _mp, desc.winID );
		#else
			LDEBUG( "vlc", "Using WinID=0x%p", desc );
			libvlc_media_player_set_hwnd( _mp, desc );
		#endif
		}
	}

	{	//	Attach to stop asyn events
		system()->dispatcher()->registerTarget( this, "canvas::vlc::MediaPlayer" );
		libvlc_event_manager_t *mgr=libvlc_media_player_event_manager( _mp );
		libvlc_event_attach( mgr, libvlc_MediaPlayerEndReached, impl::vlc_event, this );
	}

	{	//	Setup audio
		if ( _needsMute ) {
			libvlc_audio_set_mute( _mp, 1 );
		}
		if (_volume != -1 ) {
			libvlc_audio_set_volume( _mp, _volume );
		}
	}

	{	// Parse media and attach callback
		libvlc_media_t *media = libvlc_media_player_get_media(_mp);
		libvlc_event_manager_t *mgr=libvlc_media_event_manager(media);
		libvlc_event_attach( mgr, libvlc_MediaParsedChanged, impl::vlc_event, this );
	}

	static_cast<StreamsInfo*>(sInfo())->init(_mp);
	if (libvlc_media_player_play( _mp )!=0) {
		LWARN("vlc", "Fail to start playing %s", data.c_str());
		return false;
	}

	return true;
}

void MediaPlayer::stopImpl() {
	LDEBUG( "vlc", "Stop" );
	DTV_ASSERT(_mp);
	static_cast<StreamsInfo*>(sInfo())->fin();

	{	//	Dettach to stop asyn events
		system()->dispatcher()->unregisterTarget( this );
		libvlc_event_manager_t *mgr=libvlc_media_player_event_manager( _mp );
		libvlc_event_detach( mgr, libvlc_MediaPlayerEndReached, impl::vlc_event, this );
	}
	{
		libvlc_media_t *media = libvlc_media_player_get_media(_mp);
		libvlc_event_manager_t *mgr = libvlc_media_event_manager(media);
		libvlc_event_detach( mgr, libvlc_MediaParsedChanged, impl::vlc_event, this );
	}

	//	Stop playing
	libvlc_media_player_stop(_mp);

	//	Release player
	libvlc_media_player_release( _mp );
	_mp = NULL;

	destroyOverlay();
}

void MediaPlayer::pauseImpl( bool pause ) {
	LDEBUG( "vlc", "Pause: param=%d", pause );
	//	Pause playing
	libvlc_media_player_set_pause(_mp, pause ? 1 : 0 );
}

//	Volume
void MediaPlayer::muteImpl( bool needsMute ) {
	LDEBUG( "vlc", "Mute: param=%d", needsMute );
	if (_mp) {
		libvlc_audio_set_mute( _mp, needsMute ? 1 : 0 );
	} else {
		_needsMute = needsMute;
	}
}

void MediaPlayer::volumeImpl( Volume vol ) {
	float relation = (float)(VLC_MAX_VOL-VLC_MIN_VOL) / (float)(MAX_VOLUME-MIN_VOLUME);
	_volume = boost::math::iround( ((float)vol * relation + (float)VLC_MIN_VOL) );

	LDEBUG( "vlc", "Volume: param=%d normalized=%d", vol, _volume );
	if (_mp) {
		libvlc_audio_set_volume( _mp, _volume );
	}
}

//	Aux
libvlc_instance_t *MediaPlayer::instance() const {
	return _player->instance();
}

void MediaPlayer::onEndReached() {
	system()->dispatcher()->post( this, boost::bind(&MediaPlayer::onMediaCompleted,this) );
}

void MediaPlayer::onParsedEvent() {
	system()->dispatcher()->post( this, boost::bind(&MediaPlayer::onMediaParsed,this) );
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	bool result = false;
	const VideoStreamInfo *vInfo = streamsInfo().videoStreamInfo(id);
	if (vInfo) {
		result = libvlc_video_set_track(_mp, static_cast<const VLCVideoSInfo*>(vInfo)->id()) == 0;
	}
	if (!result) {
		LWARN("MediaPlayer", "Fail to switch to video stream with id=%d", id);
	}
	return result;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	bool result = false;
	const VLCAudioSInfo *sInfo = static_cast<const VLCAudioSInfo*>(streamsInfo().audioStreamInfo(id));
	if (sInfo) {
		result = libvlc_audio_set_track(_mp, sInfo->id()) == 0;
	}
	if (!result) {
		LWARN("MediaPlayer", "Fail to switch to audio stream with id=%d", id);
	}
	return result;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	bool result = false;
	const SubtitleStreamInfo *sInfo = streamsInfo().subtitleStreamInfo(id);
	if (sInfo) {
		return libvlc_video_set_spu(_mp, static_cast<const VLCSubtitleSInfo*>(sInfo)->id()) == 0;
	}
	if (!result) {
		LWARN("MediaPlayer", "Fail to switch to subtitle stream with id=%d", id);
	}
	return result;
}

}
}
