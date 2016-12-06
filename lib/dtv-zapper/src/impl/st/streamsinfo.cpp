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

#include "st.h"
#include "streamsinfo.h"
#include <util/log.h>

extern "C" {
	#include "stapp_main.h"
}

#define PLAYBACK_ID 1

//	Auxiliar definitions for parsing of StreamsInfo
#define ST_STREAM_TYPE_UNKNOWN  0
#define ST_STREAM_TYPE_VIDEO    1
#define ST_STREAM_TYPE_AUDIO    2
#define ST_STREAM_TYPE_SUBTITLE 3

namespace st {

StreamsInfo::StreamsInfo( PLAYREC_PlayInitParams_t *params )
{
	LDEBUG("st", "StreamsInfo[%p] created!", this);
	_playParams = params;
}

StreamsInfo::~StreamsInfo()
{
	LDEBUG("st", "StreamsInfo[%p] destroyed!", this);
	_playParams = NULL;
}

void StreamsInfo::refreshImpl() {
	LDEBUG("st", "StreamsInfo refresh!");
	PLAYREC_PlayParams_t PLAYREC_PlayParams;
	PLAYREC_PlayStreamInfos_t PLAYREC_PlayStreamInfos;

	//	Get play params
	if (PLAYREC_PlayGetParams(PLAYBACK_ID,&PLAYREC_PlayParams) != ST_NO_ERROR) {
		LDEBUG( "st", "Cannot get player parameters" );
		return;
	}

	if (PLAYREC_PlayGetStreamInfos(PLAYBACK_ID,&PLAYREC_PlayStreamInfos) != ST_NO_ERROR) {
		LDEBUG( "st", "Cannot get from player the stream infos" );
		return;
	}

	//	Find if video handle is valid and update VideoStreamInfo
	for (size_t index=0; index<VID_MAX_NUMBER; index++) {
		if (VID_Codec[index].VID_Handle==PLAYREC_PlayParams.VIDHandle) {
			canvas::VideoStreamInfo *vInfo = videoInfo(0);
			vInfo->codecName( PLAYREC_PlayStreamInfos.StreamFormatName );
			vInfo->bitrate( PLAYREC_PlayStreamInfos.VideoBitRateInKBitsPerSec );
			vInfo->size( canvas::Size(PLAYREC_PlayStreamInfos.VideoWidth, PLAYREC_PlayStreamInfos.VideoHeight) );
			vInfo->framerate( PLAYREC_PlayStreamInfos.VideoFrameRate / 1000 );
			vInfo->interleaved( true );
			// This info isn't available throw stapi
			// vInfo->name( const std::string &name );
			// vInfo->language( const std::string &lan );
			// vInfo->aspectRatio( float aspect );
			break;
		}
	}

	//	Find if audio handle is valid and update AudioStreamInfo
	for (size_t index=0; index<AUD_MAX_NUMBER; index++) {
		if (AUD_Codec[index].AUD_Handle==PLAYREC_PlayParams.AUDHandle) {
			canvas::AudioStreamInfo *aInfo = audioInfo(0);
			aInfo->codecName( PLAYREC_PlayStreamInfos.StreamFormatName );
			aInfo->bitrate( PLAYREC_PlayStreamInfos.AudioBitRateInKBitsPerSec );
			aInfo->channels(2);
			// This info isn't available throw stapi
			// aInfo->name( const std::string &name );
			// aInfo->language( const std::string &lan );
			// aInfo->samplerate( int samplerate );
			// aInfo->bitspersample( int bps );
			break;
		}
	}
}

void StreamsInfo::parseImpl() {
	LDEBUG("st", "StreamsInfo parse!");
	int nVideo=0,nAudio=0,nSubt=0;
	for (int i=0; i<_playParams->PidsNum; i++) {
		int type = parseType(_playParams->Pids[i].Type);
		switch (type) {
			case ST_STREAM_TYPE_VIDEO: addVideoInfo(new canvas::VideoStreamInfo()); nVideo++; break;
			case ST_STREAM_TYPE_AUDIO: addAudioInfo(new canvas::AudioStreamInfo()); nAudio++; break;
			case ST_STREAM_TYPE_SUBTITLE: addSubtitleInfo(new canvas::SubtitleStreamInfo()); nSubt++; break;
			default:
				LWARN("st", "StreamsInfo: Unkown stream type");
		}
	}
	if (nVideo>0) {
		currentVideo(0);
	}
	if (nAudio>0) {
		currentAudio(0);
	}
	if (nSubt>0) {
		currentSubtitle(0);
	}
}

int StreamsInfo::parseType( int type ) {
	switch (type) {
		case PLAYREC_STREAMTYPE_MP1V:
		case PLAYREC_STREAMTYPE_MP2V:
		case PLAYREC_STREAMTYPE_MP4V:
		case PLAYREC_STREAMTYPE_H264:
		case PLAYREC_STREAMTYPE_MPEG4P2:
		case PLAYREC_STREAMTYPE_VC1:
		case PLAYREC_STREAMTYPE_MMV:
		case PLAYREC_STREAMTYPE_AVS:
			return ST_STREAM_TYPE_VIDEO;

		case PLAYREC_STREAMTYPE_TTXT:
		case PLAYREC_STREAMTYPE_SUBT:
			return ST_STREAM_TYPE_SUBTITLE;

		case PLAYREC_STREAMTYPE_MP1A:
		case PLAYREC_STREAMTYPE_MP2A:
		case PLAYREC_STREAMTYPE_MP4A:
		case PLAYREC_STREAMTYPE_AC3:
		case PLAYREC_STREAMTYPE_AAC:
		case PLAYREC_STREAMTYPE_HEAAC:
		case PLAYREC_STREAMTYPE_WMA:
		case PLAYREC_STREAMTYPE_DDPLUS:
		case PLAYREC_STREAMTYPE_DTS:
		case PLAYREC_STREAMTYPE_MMA:
		case PLAYREC_STREAMTYPE_LPCM:
		case PLAYREC_STREAMTYPE_DDPULSE:
		case PLAYREC_STREAMTYPE_AUD_ID:
		case PLAYREC_STREAMTYPE_DRA:
			return ST_STREAM_TYPE_AUDIO;

		default:
			return ST_STREAM_TYPE_UNKNOWN;
	}
}

}
