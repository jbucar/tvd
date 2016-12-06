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

#include "mediaplayer.h"
#include "streamsinfo.h"
#include "player.h"
#include "st.h"
#include <util/cfg/configregistrator.h>
#include <util/url.h>
#include <util/log.h>
#include <canvas/types.h>
#include <canvas/streaminfo.h>
#include <canvas/window.h>
#include <canvas/system.h>

extern "C" {
	#include "stapp_main.h"
}

#define PLAYBACK_ID 1

namespace st {

MediaPlayer::MediaPlayer( canvas::System *sys )
	: canvas::MediaPlayer( sys )
{
	_init = (PLAYREC_PlayInitParams_t *)calloc( 1, sizeof(PLAYREC_PlayInitParams_t) );
}

MediaPlayer::~MediaPlayer()
{
	free( _init );
}

//	Media
bool MediaPlayer::startImpl( const std::string &data ) {
	ST_ErrorCode_t err;
	util::Url url(data);
	std::string type=url.type();
	LDEBUG( "st", "Initialize: url=%s", data.c_str() );

	//	Create media from url
	if (type == "srvdtv") {
		int streams = 0;
		std::string audio, video;

		memset(_init, 0, sizeof(PLAYREC_PlayInitParams_t));
		sprintf(_init->SourceName, "%d", 0);
		strncpy(_init->DestinationName, "SWTS", sizeof(_init->DestinationName)-1);

		if (!addStream( &url, true )) {
			LWARN( "st", "Cannot play audio stream" );
		} else {
			streams++;
		}

		if (!addStream( &url, false )) {
			LWARN( "st", "Cannot play video stream" );
		} else {
			streams++;
		}

		if (!streams) {
			LWARN( "st", "No stream to play" );
			return false;
		}

		{	//	PCR
			int pcrPID;
			if (url.getParam( "pcrPID", pcrPID )) {
				bool pidFound = false;
				for (int i = 0; i < _init->PidsNum; i++) {
					if (_init->Pids[i].Pid == pcrPID) {
						pidFound = true;
						break;
					}
				}

				if (!pidFound) {
					LINFO( "st", "Add PCR stream: pid=%04x", pcrPID );

					//	Add PCR pid
					_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_PCR;
					_init->Pids[_init->PidsNum].Pid  = pcrPID;
					_init->PidsNum++;
				}
			}
		}

		LINFO("st", "Play stream: src=%s, dst=%s, pids=%d",
			_init->SourceName, _init->DestinationName, _init->PidsNum );

		//  Finish setup init structure
	#ifdef ST_7109
		_init->Metadata                  = NULL;
		_init->WmdrmNDHeader             = NULL;
	#endif
		_init->IsFilePartiallyDownloaded = FALSE;

		//	Initialize player
		err = PLAYREC_PlayInit(PLAYBACK_ID, _init);
		if (err != ST_NO_ERROR) {
			LWARN("st", "cannot initialize player");
			return false;
		}

		//	Start player
		PLAYREC_PlayStartParams_t start;
		memset(&start, 0, sizeof(PLAYREC_PlayStartParams_t));
		start.VideoLayerIdList[0] = 0;
		start.VideoLayerIdList[1] = 1;
		start.VideoLayerIdList[PLAYREC_MAX_VIEWPORTS] = 0xFFFFFFFF;
		start.VideoEnableDisplay = TRUE;
		start.VideoDecodeOnce    = FALSE;
		start.AudioEnableOutput  = st::isMuted() ? FALSE : TRUE;
		start.StartSpeed         = PLAYREC_PLAY_SPEED_ONE;
		start.StartPositionInMs  = 0;
		strncpy(start.SourceName, _init->SourceName, sizeof(start.SourceName)-1);
		strncpy(start.DestinationName, _init->DestinationName, sizeof(start.DestinationName)-1);
		start.PidsNum = _init->PidsNum;
		for (int i = 0; i < _init->PidsNum; i++) {
			start.Pids[i].Type = _init->Pids[i].Type;
			start.Pids[i].Pid  = _init->Pids[i].Pid;
		}

		err = PLAYREC_PlayStart(PLAYBACK_ID, &start);
		if (err != ST_NO_ERROR) {
			LWARN("st", "cannot start player");
			playTerm();
			return false;
		}

		LINFO("st", "Play stream end" );
		return true;
	}
	else {
		int loops=0;
		STAUD_StartParams_t AUD_StartParams;
		memset(&AUD_StartParams,0,sizeof(AUD_StartParams));
		AUD_StartParams.SamplingFrequency  = 44100;
		AUD_StartParams.StreamContent      = STAUD_STREAM_CONTENT_MPEG1;
		AUD_StartParams.StreamID           = STAUD_IGNORE_ID;
		AUD_StartParams.StreamType         = STAUD_STREAM_TYPE_ES;
		err=AUD_InjectStart(
			AUDIO_ID,
			AUDIO_INPUT_ID,
			(U8 *)data.c_str(),
			&AUD_StartParams,
			loops
#ifndef ST_VERONA
			,8
			,0
#endif
		);
		if (err!=ST_NO_ERROR) {
			LWARN("st", "cannot start player: url=%s", data.c_str() );
			return false;
		}

		return true;
	}
}


void MediaPlayer::stopImpl() {
	ST_ErrorCode_t err;
	PLAYREC_PlayStopParams_t stop;

	LDEBUG("st", "Stop begin");

	util::Url data(url());
	std::string type=data.type();
	if (type == "srvdtv") {
		memset(&stop,0,sizeof(PLAYREC_PlayStopParams_t));
		stop.VideoStopMethod            = STVID_STOP_NOW;
		stop.VideoFreezeMethod.Field    = STVID_FREEZE_FIELD_TOP;
		stop.VideoFreezeMethod.Mode     = STVID_FREEZE_MODE_NONE;
		stop.VideoDisableDisplay        = TRUE;
		stop.AudioStopMethod            = STAUD_STOP_NOW;
		stop.AudioFadingMethod.FadeType = STAUD_FADE_SOFT_MUTE;

		err=PLAYREC_PlayStop(PLAYBACK_ID, &stop);
		if (err != ST_NO_ERROR) {
			LWARN("st", "cannot stop player");
		}
		else {
			playTerm();
		}
	}
	else if (type == "audio") {
		err=AUD_InjectStop(
			AUDIO_ID,
			AUDIO_INPUT_ID,
			FALSE
		);
		if (err!=ST_NO_ERROR) {
			LWARN("st", "cannot stop player");
		}
	}

	LDEBUG("st", "Stop end");
}

void MediaPlayer::pauseImpl( bool needPause ) {
	LDEBUG("st", "Pause=%d", needPause);
}

bool MediaPlayer::parsedOnPlay() const {
	return true;
}

bool MediaPlayer::switchVideoStreamImpl( int /*id*/ ) {
	// TODO
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int /*id*/ ) {
	// TODO
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int /*id*/ ) {
	// TODO
	return true;
}

//	Volume
void MediaPlayer::muteImpl( bool /*needMute*/ ) {
	//	TODO
}

void MediaPlayer::volumeImpl( canvas::Volume /*vol*/ ) {
	//	TODO
}

//	Aux
void MediaPlayer::playTerm() {
	PLAYREC_PlayTermParams_t term;
	memset(&term,0,sizeof(PLAYREC_PlayTermParams_t));
	ST_ErrorCode_t err=PLAYREC_PlayTerm(PLAYBACK_ID, &term);
	if (err != ST_NO_ERROR) {
		LWARN("st", "cannot terminate player");
	}
}

bool MediaPlayer::addStream( util::Url *url, bool isAudio ) {
	bool ok=false;
	int pid;
	int type;
	std::string strType, strPID;

	if (isAudio) {
		strType = "audioType";
		strPID = "audioPID";
	}
	else {
		strType = "videoType";
		strPID = "videoPID";
	}

	if (url->getParam( strPID, pid ) &&
		url->getParam( strType, type ))
	{
		if (isAudio) {
			ok=addAudioStream( type, pid );
		}
		else {
			ok=addVideoStream( type, pid );
		}
	}

	return ok;
}

bool MediaPlayer::addVideoStream( int type, int pid ) {
	bool ok=true;
	LINFO( "st", "Add video stream: type=%d, pid=%04x", type, pid );

	// Video Codecs
	switch (type) {
		case STTAPI_STREAM_VIDEO_MPEG1: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_MP1V;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_VIDEO_MPEG2: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_MP2V;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_VIDEO_MPEG4: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_H264;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_VIDEO_VC1: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_VC1;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_VIDEO_AVS: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_AVS;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		default:
			ok=false;
			break;
	};

	if (ok) {
		_init->PidsNum++;
	}

	return ok;
}

bool MediaPlayer::addAudioStream( int type, int pid ) {
	bool ok=true;

	LINFO( "st", "Add audio stream: type=%d, pid=%04x", type, pid );

	// Audio Codecs:
	switch (type) {
		case STTAPI_STREAM_AUDIO_MPEG1: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_MP1A;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_MPEG2: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_MP2A;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_WMA: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_WMA;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_AAC_ADTS: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_AAC;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_AAC_LATM: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_HEAAC;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_AAC_RAW1:
		case STTAPI_STREAM_AUDIO_AAC_RAW2: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_HEAAC;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_AC3: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_AC3;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_DTS:
		case STTAPI_STREAM_AUDIO_DTSHD:
		case STTAPI_STREAM_AUDIO_DTSHD_XLL:
		case STTAPI_STREAM_AUDIO_DTSHD_2: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_DTS;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		case STTAPI_STREAM_AUDIO_DDPLUS:
		case STTAPI_STREAM_AUDIO_DDPLUS_2: {
			_init->Pids[_init->PidsNum].Type = PLAYREC_STREAMTYPE_DDPLUS;
			_init->Pids[_init->PidsNum].Pid  = pid;
			break;
		}
		default :
			ok=false;
			break;
	}

	if (ok) {
		_init->PidsNum++;
	}

	return ok;
}

void MediaPlayer::moveResizeImpl( const canvas::Rect &rect ) {
	ST_ErrorCode_t ErrCode;
	S32 inX, inY, outX, outY, newOutX, newOutY;
	U32 inW, inH, outW, outH, newOutW, newOutH;
	STVID_WindowParams_t stcWindowParams;
	STVID_ViewPortHandle_t viewPortHnd;
	STVTG_TimingMode_t stcTimingMode;
	STVTG_ModeParams_t stcModeParams;
	const canvas::Size &canvasSize = this->canvasSize();

	LDEBUG( "st", "setVideoWindow: rect=(%d,%d,%d,%d)",
		rect.x, rect.y, rect.w, rect.h );

	//	Get viewport handle
	for (size_t i = 0; i<VID_MAX_NUMBER; i++) {
		for (size_t j = 0; j<VID_LAYER_MAX_NUMBER; j++) {
			//	ViewPort is valid?
			if (VID_Codec[i].VID_ViewPortHandle[j] == NULL) {
				continue;
			}

			viewPortHnd = VID_Codec[i].VID_ViewPortHandle[j];
			memset(&stcWindowParams,0,sizeof(STVID_WindowParams_t));
			stcWindowParams.Align = STVID_WIN_ALIGN_VCENTRE_HCENTRE;
			stcWindowParams.Size  = STVID_WIN_SIZE_DONT_CARE;

			//	Set window mode to manual
			ErrCode = STVID_SetOutputWindowMode( viewPortHnd, FALSE, &stcWindowParams );
			if (ErrCode != ST_NO_ERROR) {
				LWARN( "st", "Cannot set window mode to manual" );
				continue;
			}

			//	Get windows size
			ErrCode = STVID_GetIOWindows( viewPortHnd, &inX, &inY, &inW, &inH, &outX, &outY, &outW, &outH );
			if (ErrCode != ST_NO_ERROR) {
				LWARN( "st", "Cannot get window size" );
				continue;
			}
			LDEBUG( "st", "Window size: in=(%d,%d,%d,%d), out=(%d,%d,%d,%d)",
				inX, inY, inW, inH, outX, outY, outW, outH );

			//	Get screen mode/timing
			ErrCode = STVTG_GetMode( VTG_Handle[j], &stcTimingMode, &stcModeParams );
			if (ErrCode != ST_NO_ERROR) {
				LWARN( "st", "Cannot get screen mode/timing" );
				continue;
			}
			LDEBUG( "st", "Screen size: in=(%d,%d)",
				stcModeParams.ActiveAreaWidth, stcModeParams.ActiveAreaHeight );

			//	Need set full screen!
			if (isFullScreen()) {
				newOutX = 0;
				newOutY = 0;
				newOutW = stcModeParams.ActiveAreaWidth;
				newOutH = stcModeParams.ActiveAreaHeight;
			}
			else {
				newOutX = (rect.x * stcModeParams.ActiveAreaWidth) / canvasSize.w;
				newOutY = (rect.y * stcModeParams.ActiveAreaHeight) / canvasSize.h;
				newOutW = (rect.w * stcModeParams.ActiveAreaWidth) / canvasSize.w;
				newOutH = (rect.h * stcModeParams.ActiveAreaHeight) / canvasSize.h;
			}

			LDEBUG( "st", "Set window size: handle=(%d,%d), out=(%d,%d,%d,%d)",
				i, j, newOutX, newOutY, newOutW, newOutH );

			// Set windows size
			ErrCode = STVID_SetIOWindows( viewPortHnd, inX, inY, inW, inH, newOutX, newOutY, newOutW, newOutH );
			if (ErrCode != ST_NO_ERROR) {
				LWARN( "st", "Cannot set window size" );
				continue;
			}

			//	Set aspect ratio (ignore pan-scan/letter-box)
			if (STVID_SetDisplayAspectRatioConversion( VID_Codec[i].VID_ViewPortHandle[j], STVID_DISPLAY_AR_CONVERSION_IGNORE ) != ST_NO_ERROR) {
				LWARN("st", "Cannot set aspect ratio conversion to ignore");
				continue;
			}
		}
	}
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo(_init);
}

}
