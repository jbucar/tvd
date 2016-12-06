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

#include "videostream.h"
#include "../../window.h"
#include <util/string.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/vaapi.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
}

namespace canvas {
namespace ffmpeg {

//#define DUMP_PACKETS

#ifdef DUMP_PACKETS
static void dump_pkt( AVCodecContext *avctx, AVPacket *pkt ) {
	LTRACE( "ffmpeg", "Dump video packet: pkt=(%p,%d)", pkt, pkt->size );

	{
		static int count=0;
		std::string frameName = util::format( "frame_%08x.", count );

		if (avctx->extradata_size > 0) {
			std::string data = frameName + "extra";
			FILE *f = fopen( data.c_str(),"w+");
			if (f) {
				fwrite( avctx->extradata, 1, avctx->extradata_size, f );
				fclose(f);
			}
		}

		if (pkt->size > 0) {
			std::string data = frameName + "data";
			FILE *f = fopen( data.c_str(),"w+");
			if (f) {
				fwrite( pkt->data, 1, pkt->size, f );
				fclose(f);
			}
		}

		count++;
	}
}
#endif

VideoStream::VideoStream( System *sys )
	: Stream( sys, stream::video )
{
	_nFrames = 0;
	_imgConvertCtx = NULL;

	_exit = false;
}

VideoStream::~VideoStream()
{
	DTV_ASSERT(!_imgConvertCtx);
}

bool VideoStream::initCodec( AVCodecContext *avctx, AVCodec *codec ) {
	if (!Stream::initCodec( avctx, codec )) {
		return false;
	}

	//if (_win->render()->name() == "vaapi") {
	// struct vaapi_context hw_ctx;
    // memset( &hw_ctx, 0, sizeof(hw_ctx) );
    // p_va->hw_ctx.display    = p_va->p_display;
    // p_va->hw_ctx.config_id  = p_va->i_config_id;
	// p_va->hw_ctx.context_id = p_va->i_context_id;
	// avctx->hwaccel_context = &hw_ctx;
	//}

	if (!createSWS()) {
		return false;
	}

	LDEBUG( "ffmpeg", "Init video codec: width=%d, height=%d", avctx->width, avctx->height );

	return true;
}

void VideoStream::beginProcess() {
	LDEBUG( "ffmpeg", "Begin video process" );
	_exit = false;
	_refreshThread = boost::thread( boost::bind(&VideoStream::refreshThread,this) );
	Stream::beginProcess();
}

void VideoStream::processPkt( AVFrame *frame, AVPacket *pkt ) {
	AVCodecContext *avctx = stream()->codec;

#ifdef DUMP_PACKETS
	dump_pkt( avctx, pkt );
#endif

	//	Try decode picture
	int got_picture;
	int ret = avcodec_decode_video2( avctx, frame, &got_picture, pkt );
	if (ret < 0) {
		char msg[AV_ERROR_MAX_STRING_SIZE];
		LWARN( "ffmpeg", "Error decoding video frame: err=%s", av_make_error_string(msg,AV_ERROR_MAX_STRING_SIZE,ret) );
	}
	else if (got_picture) {
		LTRACE( "ffmpeg", "Got picture: extradata=%d, pkt=%d", avctx->extradata_size, pkt->size );

		//	Update clock
		/*double pts =*/ updatePTS( frame );

		//	TODO: DROP packet!

		// //	Alloc frame
		// AVPicture pict;
		// void *vFrame = NULL;
		// while (!vFrame) {
		// 	vFrame=_win->allocFrame( (void **)pict.data );
		// 	if (!vFrame) {
		// 		//	Wait for free frame
		// 		boost::unique_lock<boost::mutex> lock( _mutex );
		// 		while (!_exit && _nFrames == (int)_frames.size()) {
		// 			_cWakeup.wait( lock );
		// 		}

		// 		if (_exit) {
		// 			LWARN( "ffmpeg", "Cannot alloc video frame" );
		// 			return;
		// 		}
		// 	}
		// }

		// //	Fill picture and swap components
		// pict.linesize[0] = _pitches[0];
		// pict.linesize[1] = _pitches[2];
		// pict.linesize[2] = _pitches[1];

		// uint8_t *swap = pict.data[1];
		// pict.data[1] = pict.data[2];
		// pict.data[2] = swap;

		// //	Scale
		// sws_scale(
		// 	_imgConvertCtx,
		// 	_frame->data, _frame->linesize,
		// 	0,
		// 	stream()->codec->height,
		// 	pict.data,
		// 	pict.linesize
		// );

		// //	Enqueue frame
		// _mutex.lock();
		// _frames.push_back( vFrame );
		// _mutex.unlock();
		// _cWakeup.notify_all();

		//vp->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->ic, is->video_st, src_frame);
	}
}

void VideoStream::endProcess() {
	LINFO( "ffmpeg", "End video process" );
	_exit=true;
	_cWakeup.notify_all();
	_refreshThread.join();

	Stream::endProcess();
	if (_imgConvertCtx) {	//	TODO: Move to close/stop?
		sws_freeContext(_imgConvertCtx);
		_imgConvertCtx = NULL;
	}
}

void VideoStream::refreshThread() {
	bool exit=false;
	void *frame;

	LDEBUG( "ffmpeg", "Refresh thread started" );

	while (!exit) {
		frame = NULL;

		{	//	Lock!
			boost::unique_lock<boost::mutex> lock( _mutex );
			while (!_exit && _frames.empty()) {
				_cWakeup.wait( lock );
			}

			if (_exit) {
				exit=true;
			}
			else if (!_frames.empty()) {
				frame = _frames.front();
				_frames.pop_front();
			}
		}	//	Unlock!

		if (frame) {
			//	Render video
			// _win->renderFrame( frame );
			// _win->freeFrame( frame );

			//	Notify video thread that a frame was free
			_cWakeup.notify_all();

			boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
		}
	}

	LDEBUG( "ffmpeg", "Refresh thread stopped" );
}

bool VideoStream::createSWS() {
	DTV_ASSERT(!_imgConvertCtx);

	// //	Create sws context
	// char chroma[5];
	// unsigned int width, height, lines[3];
	// _nFrames=_win->getFormat(
	// 	chroma,
	// 	&width,
	// 	&height,
	// 	_pitches,
	// 	lines
	// );

	// LTRACE( "ffmpeg", "Create context for format: nFrames=%d, chroma=%s, size=(%d,%d), pitches=(%d,%d,%d), lines=(%d,%d,%d)",
	// 	_nFrames, chroma, width, height,
	// 	_pitches[0], _pitches[1], _pitches[2],
	// 	lines[0], lines[1], lines[2]
	// );

	// //	Get dstPixFmt from chroma
	// PixelFormat dstPixFmt;
	// if (!strcmp(chroma,"YV12")) {
	// 	dstPixFmt = PIX_FMT_YUV420P;
	// }
	// else if (!strcmp(chroma,"NV12")) {
	// 	dstPixFmt = PIX_FMT_NV12;
	// }
	// else {
	// 	LERROR("ffmpeg", "Cannot get compatible frame format" );
	// 	return false;
	// }

	// AVStream *st = stream();
	// _imgConvertCtx = sws_getContext(
	// 	st->codec->width,	st->codec->height,
	// 	st->codec->pix_fmt,
	// 	width, height,
	// 	dstPixFmt,
	// 	SWS_BICUBIC,
	// 	NULL, NULL, NULL
	// );
	// if (!_imgConvertCtx) {
	// 	LERROR("ffmpeg", "Cannot initialize the conversion context" );
	// 	return false;
	// }
	return true;
}

}
}

