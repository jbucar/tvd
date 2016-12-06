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

#include "streaminfo.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <vlc/libvlc_version.h>
#include <vlc/vlc.h>
#include <boost/math/special_functions/round.hpp>

#if LIBVLC_VERSION_INT < LIBVLC_VERSION(2,1,2,0)
struct libvlc_media_track_t {
};
#endif

namespace canvas {
namespace vlc {

StreamsInfo::StreamsInfo()
{
	_mp = NULL;
}

StreamsInfo::~StreamsInfo()
{
	_mp = NULL;
}

void StreamsInfo::init( libvlc_media_player_t *mp ) {
	LDEBUG("vlc", "StreamsInfo init!");
	DTV_ASSERT(mp);
	_mp = mp;
}

void StreamsInfo::fin() {
	LDEBUG("vlc", "StreamsInfo fin!");
	_mp = NULL;
}

void StreamsInfo::parseImpl() {
	LDEBUG("vlc", "StreamsInfo parse!");
	if (_mp) {
		libvlc_track_description_t *first = libvlc_video_get_track_description(_mp);
		libvlc_track_description_t *tDesc = first;
		int current = libvlc_video_get_track(_mp);
		int pos=0;
		while (tDesc) {
			if (tDesc->i_id >= 0) {
				VideoStreamInfo *vInfo = new VLCVideoSInfo(tDesc->i_id);
				vInfo->name( tDesc->psz_name );
				addVideoInfo(vInfo);
				if (tDesc->i_id == current) {
					currentVideo(pos);
				}
				pos++;
			}
			tDesc = tDesc->p_next;
		}
		libvlc_track_description_list_release(first);

		tDesc = first = libvlc_audio_get_track_description(_mp);
		current = libvlc_audio_get_track(_mp);
		pos=0;
		while (tDesc) {
			if (tDesc->i_id >= 0) {
				AudioStreamInfo *aInfo = new VLCAudioSInfo(tDesc->i_id);
				aInfo->name( tDesc->psz_name );
				addAudioInfo(aInfo);
				if (tDesc->i_id == current) {
					currentAudio(pos);
				}
				pos++;
			}
			tDesc = tDesc->p_next;
		}
		libvlc_track_description_list_release(first);

		tDesc = first = libvlc_video_get_spu_description(_mp);
		current = libvlc_video_get_spu(_mp);
		pos=0;
		while (tDesc) {
			if (tDesc->i_id >= 0) {
				SubtitleStreamInfo *ssInfo = new VLCSubtitleSInfo(tDesc->i_id);
				ssInfo->name( tDesc->psz_name );
				addSubtitleInfo(ssInfo);
				if (tDesc->i_id == current) {
					currentSubtitle(pos);
				}
				pos++;
			}
			tDesc = tDesc->p_next;
		}
		libvlc_track_description_list_release(first);
	}
}

void StreamsInfo::refreshImpl() {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(2,1,3,0)
	LDEBUG("vlc", "StreamsInfo refresh!");
	if (_mp) {
		size_t vCount=0, aCount=0, sCount=0;
		size_t vTotal = videoCount();
		size_t aTotal = audioCount();
		size_t sTotal = subtitleCount();

		libvlc_media_t *media = libvlc_media_player_get_media(_mp);
		libvlc_media_track_t **tracks;
		int cant = libvlc_media_tracks_get( media, &tracks );

		for (int i=0; i<cant; i++) {
			switch (tracks[i]->i_type) {
				case libvlc_track_video:
					if (vCount<vTotal) {
						refreshVideo(vCount, tracks[i]);
						vCount++;
					}
					break;
				case libvlc_track_audio:
					if (aCount<aTotal) {
						refreshAudio(aCount, tracks[i]);
						aCount++;
					}
					break;
				case libvlc_track_text:
					if (sCount<sTotal) {
						refreshSubtitle(sCount, tracks[i]);
						sCount++;
					}
					break;
				default:
					LDEBUG("vlc", "Unkonwn track type. id=%d", tracks[i]->i_id);
					break;
			}
		}
		libvlc_media_tracks_release( tracks, cant );
	}
#else
	LWARN("vlc", "The version of libvlc is incompatible: vlc_version=%d.%d.%d", LIBVLC_VERSION_MAJOR, LIBVLC_VERSION_MINOR, LIBVLC_VERSION_REVISION);
#endif
}

void StreamsInfo::refreshVideo( size_t id, libvlc_media_track_t *track ) {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(2,1,3,0)
	DTV_ASSERT(track->i_type == libvlc_track_video);
	VideoStreamInfo *vInfo = videoInfo(id);
	LTRACE("vlc", "Refresh video stream info! id=%d, vInfo=%p", id, vInfo);

	vInfo->language(track->psz_language ? track->psz_language : "");
	vInfo->bitrate(track->i_bitrate);
	Size tmpSize(track->video->i_width, track->video->i_height);
	vInfo->size(tmpSize);
	unsigned sn = track->video->i_sar_num;
	unsigned sd = track->video->i_sar_den;
	float den = (float) (tmpSize.h*sd);
	float ratio = 0.0f;
	if (den != 0.0f) {
		ratio = (float)(tmpSize.w*sn) / den;
	}
	vInfo->aspectRatio(ratio);
	int fps = 0;
	if (track->video->i_frame_rate_den > 0) {
		float tmp = (float)track->video->i_frame_rate_num / (float)track->video->i_frame_rate_den;
		fps = boost::math::iround(tmp);
	} else {
		fps = track->video->i_frame_rate_num;
	}
	vInfo->framerate(fps);
	vInfo->interleaved(track->video->i_frame_rate_den > 0);
	char tmpCodec[5];
	memcpy(tmpCodec, &track->i_codec, 4);
	tmpCodec[4] = 0;
	vInfo->codecName(tmpCodec);
#else
	UNUSED(id);
	UNUSED(track);
	LWARN("vlc", "The version of libvlc is incompatible: vlc_version=%d.%d.%d", LIBVLC_VERSION_MAJOR, LIBVLC_VERSION_MINOR, LIBVLC_VERSION_REVISION);
#endif
}

void StreamsInfo::refreshAudio( size_t id, libvlc_media_track_t *track ) {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(2,1,3,0)
	DTV_ASSERT(track->i_type == libvlc_track_audio);
	AudioStreamInfo *aInfo = audioInfo(id);
	LTRACE("vlc", "Refresh audio stream info! id=%d, aInfo=%p", id, aInfo);

	aInfo->language(track->psz_language ? track->psz_language : "");
	aInfo->bitrate(track->i_bitrate);
	aInfo->channels(track->audio->i_channels);
	aInfo->samplerate(track->audio->i_rate);
	aInfo->bitspersample((track->audio->i_rate>0) ? boost::math::iround((float)track->i_bitrate / (float)track->audio->i_rate) : 0);
	char tmpCodec[5];
	memcpy(tmpCodec, &track->i_codec, 4);
	tmpCodec[4] = 0;
	aInfo->codecName(tmpCodec);
#else
	UNUSED(id);
	UNUSED(track);
	LWARN("vlc", "The version of libvlc is incompatible: vlc_version=%d.%d.%d", LIBVLC_VERSION_MAJOR, LIBVLC_VERSION_MINOR, LIBVLC_VERSION_REVISION);
#endif
}

void StreamsInfo::refreshSubtitle( size_t id, libvlc_media_track_t *track ) {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(2,1,3,0)
	DTV_ASSERT(track->i_type == libvlc_track_text);
	SubtitleStreamInfo *sInfo = subtitleInfo(id);
	LTRACE("vlc", "Refresh subtitle stream info! id=%d, sInfo=%p", id, sInfo);
	sInfo->language( track->psz_language ? track->psz_language : "" );
	sInfo->encoding( track->subtitle->psz_encoding ? track->subtitle->psz_encoding : "" );
#else
	UNUSED(id);
	UNUSED(track);
	LWARN("vlc", "The version of libvlc is incompatible: vlc_version=%d.%d.%d", LIBVLC_VERSION_MAJOR, LIBVLC_VERSION_MINOR, LIBVLC_VERSION_REVISION);
#endif
}

}
}
