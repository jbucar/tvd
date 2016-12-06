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

#include "audiostream.h"
//#include "../../audio/stream.h"
#include "../../audio.h"
#include "../../system.h"
#include <util/log.h>
#include <util/assert.h>

extern "C" {
#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
}

namespace canvas {
namespace ffmpeg {

AudioStream::AudioStream( System *sys )
	: Stream( sys, stream::audio )
{
	_sink = NULL;
}

AudioStream::~AudioStream()
{
	DTV_ASSERT(!_sink);
}

bool AudioStream::initCodec( AVCodecContext *cCtx, AVCodec *codec ) {
#if 0
	//	Create audio stream
	_sink = sys()->audio()->createStream();
	if (!_sink) {
		LERROR( "ffmpeg", "Cannot create audio stream" );
		return false;
	}

	{	//	Setup audio format to negotiate
		audio::AudioFormat fmt;

		if (codec->sample_fmts) {
			for (int i=0; codec->sample_fmts[i] != AV_SAMPLE_FMT_NONE; i++) {
				fmt.formats.push_back( (int)codec->sample_fmts[i] );
			}
		}

		if (codec->supported_samplerates) {
			for (int i=0; codec->supported_samplerates[i] != AV_SAMPLE_FMT_NONE; i++) {
				fmt.rates.push_back( codec->supported_samplerates[i] );
			}
		}

		if (codec->channel_layouts) {
			for (int i=0; codec->channel_layouts[i] != 0; i++) {
				fmt.channels.push_back( codec->channel_layouts[i] );
			}
		}

		if (!_sink->initialize( fmt )) {
			LERROR( "ffmpeg", "Cannot initialize audio stream" );
			sys()->audio()->destroyStream( _sink );
			_sink = NULL;
			return false;
		}


		//	Setup codec
		cCtx->request_sample_fmt = AV_SAMPLE_FMT_S16;//(AVSampleFormat)fmt.format;
		//cCtx->request_channel_layout = 0;//fmt.channelLayout;
	}
#endif

	LDEBUG( "ffmpeg", "Init audio codec: rate=%d, channels=%d, sampleFmt=%d, layout=%lld, req_layout=%lld, reqFmt=%d",
		cCtx->sample_rate, cCtx->channels, cCtx->sample_fmt, cCtx->channel_layout, cCtx->request_channel_layout, cCtx->request_sample_fmt );

	return Stream::initCodec( cCtx, codec );
}

void AudioStream::finCodec( AVCodecContext * ) {
#if 0
	DTV_ASSERT(_sink);
	_sink->finalize();
	sys()->audio()->destroyStream( _sink );
	_sink = NULL;
#endif
}

void AudioStream::processPkt( AVFrame *frame, AVPacket *pkt ) {
	AVCodecContext *cCtx = stream()->codec;

	//	decode audio frame
	int got_frame;
	int ret = avcodec_decode_audio4( cCtx, frame, &got_frame, pkt );
	if (ret < 0) {
		char msg[AV_ERROR_MAX_STRING_SIZE];
		LWARN( "ffmpeg", "Error decoding audio frame: err=%s", av_make_error_string(msg,AV_ERROR_MAX_STRING_SIZE,ret) );
	}
	else {
		/* Some audio decoders decode only part of the packet, and have to be
		* called again with the remainder of the packet data.
		* Also, some decoders might over-read the packet. */
		if (got_frame) {
			//double pts = updatePTS( frame );
			//	int decoded = FFMIN(ret, pkt->size);
			//size_t unpadded_linesize = frame->nb_samples * cCtx->channels * av_get_bytes_per_sample( (AVSampleFormat)frame->format );

			// {
			// 	char msg[AV_TS_MAX_STRING_SIZE];
			// 	av_ts_make_time_string(msg, pts, &stream()->codec->time_base);
			// 	LTRACE( "ffmpeg", "Get audio frame: nb_samples=%d, decoded=%d size=%d, pts=%s",
			// 		frame->nb_samples, decoded, unpadded_linesize, msg );
			// }

			//	Convert planar audio formats to packed formats
			bool convert = false;
			switch (cCtx->sample_fmt) {
				case AV_SAMPLE_FMT_U8:
				case AV_SAMPLE_FMT_S16:
				case AV_SAMPLE_FMT_S32:
				case AV_SAMPLE_FMT_FLT:
				case AV_SAMPLE_FMT_DBL:
					break;
				case AV_SAMPLE_FMT_NONE:
					LERROR( "ffmpeg", "Decode error; Invalid data format" );
					return;
				default:
					convert = true;
			}
			if (convert) {
				LTRACE( "ffmpeg", "TODO: Complete conversion: format=%d", cCtx->sample_fmt );
			}
			else {
				//_sink->addData( pts, frame->extended_data[0], decoded );
			}
		}
	}
}

}
}

