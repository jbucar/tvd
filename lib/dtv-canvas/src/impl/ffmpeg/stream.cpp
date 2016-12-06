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

#include "stream.h"
#include "packetqueue.h"
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libavcodec/avfft.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
}

namespace canvas {
namespace ffmpeg {

namespace impl {

static int workaround_bugs = 1;
static int fast = 0;
static int lowres = 0;
static int idct = FF_IDCT_AUTO;
static enum AVDiscard skip_frame       = AVDISCARD_DEFAULT;
static enum AVDiscard skip_idct        = AVDISCARD_DEFAULT;
static enum AVDiscard skip_loop_filter = AVDISCARD_DEFAULT;
static int error_concealment = 3;
static int decoder_reorder_pts = -1;


static const char *streamNames[] = {
	"Audio",
	"Video",
	"Subtitles",
	NULL
};

static AVMediaType getType( stream::type sType ) {
	switch (sType) {
		case stream::audio:
			return AVMEDIA_TYPE_AUDIO;
			break;
		case stream::video:
			return AVMEDIA_TYPE_VIDEO;
			break;
		case stream::subtitle:
			return AVMEDIA_TYPE_SUBTITLE;
			break;
		default:
			LERROR( "ffmpeg", "Unknown AV media type: sType=%d", sType );
			return AVMEDIA_TYPE_UNKNOWN;
	};
}

}

Stream::Stream( System *sys, stream::type type )
	: _sys(sys), _type( type )
{
	_queue = new PacketQueue();
	_context = NULL;
	_index = -1;
	_stream = NULL;

	_currentPtsDrift = 0.0;
}

Stream::~Stream()
{
	delete _queue;
	DTV_ASSERT(!_context);
	DTV_ASSERT(!_stream);
	DTV_ASSERT(_index == -1);
}

stream::type Stream::type() const {
	return _type;
}

bool Stream::open( AVFormatContext *context ) {
	DTV_ASSERT(!_stream);

	bool result=false;

	//	Find stream
	int sIndex = av_find_best_stream(
		context,
		impl::getType(_type),
		-1,
		-1,
		NULL,
		0
	);

	//	There is a stream?
	if (sIndex >= 0) {
		//	Check stream index
		DTV_ASSERT( sIndex < (int)context->nb_streams );
		AVStream *stream = context->streams[sIndex];

		//	Open codec
		result=openCodec( stream->codec );
		if (result) {
			_index = sIndex;
			stream->discard = AVDISCARD_DEFAULT;
			_stream = stream;
			_context = context;

			LDEBUG( "ffmpeg", "Open stream: type=%s, index=%d, codec(%d,%s)",
				impl::streamNames[_type], _index, stream->codec->codec_id, stream->codec->codec->name );
		}
	}

	return result;
}

void Stream::close() {
	LDEBUG( "ffmpeg", "Close stream: type=%s", impl::streamNames[_type] );

	if (_stream) {
		AVCodecContext *avctx = _stream->codec;
		_stream->discard = AVDISCARD_ALL;

		finCodec( avctx );
		avcodec_flush_buffers( avctx );
		avcodec_close(avctx);

		_index = -1;
		_stream = NULL;
		_context = NULL;
	}
}

bool Stream::isOpen() const {
	return _index != -1;
}

int Stream::index() const {
	return _index;
}

void Stream::start() {
	if (isOpen()) {
		LDEBUG( "ffmpeg", "Start stream: type=%s", impl::streamNames[_type] );
		_queue->start();
		_thread = boost::thread( boost::bind(&Stream::processData,this) );
	}
}

void Stream::stop() {
	if (isOpen()) {
		LDEBUG( "ffmpeg", "Stop stream: type=%s", impl::streamNames[_type] );
		_queue->stop();
		_thread.join();
	}
}

bool Stream::push( struct AVPacket *pkt ) {
	DTV_ASSERT( isOpen() );
	_queue->push(pkt);
	return _queue->isFull();
}

bool Stream::isFull() const {
	return _queue->isFull();
}

//	Aux
void Stream::processData() {
	AVPacket pkt;
	bool exit=false;
	int ret;

	LINFO( "ffmpeg", "Begin stream thread: type=%s", impl::streamNames[_type] );

	av_init_packet(&pkt);
	AVFrame *frame = av_frame_alloc();
	beginProcess();

	while (!exit) {
		//	Get packet from queue
		ret = _queue->get( &pkt );
		if (ret > 0) {
			LTRACE( "ffmpeg", "Process packet: type=%s", impl::streamNames[_type] );
			av_frame_unref(frame);
			processPkt( frame, &pkt );
			av_free_packet( &pkt );
		}
		else if (ret < 0) {
			exit=true;
		}
	}

	endProcess();

	if (frame) {
		av_frame_free(&frame);
	}

	LINFO( "ffmpeg", "End stream thread: type=%s", impl::streamNames[_type] );
}

void Stream::beginProcess() {
}

void Stream::processPkt( AVFrame * /*frame*/, AVPacket * /*pkt*/ ) {
}

void Stream::endProcess() {
	_queue->flush();
}

bool Stream::openCodec( AVCodecContext *avctx ) {
	AVCodec *codec=NULL;

	DTV_ASSERT(avctx);
	DTV_ASSERT(avctx->codec_type == impl::getType(_type));

	//	Setup codec
	if (!_codecName.empty()) {
		codec = avcodec_find_decoder_by_name( _codecName.c_str() );
	}
	if (!codec) {
		codec = avcodec_find_decoder(avctx->codec_id);
		if (!codec) {
			LERROR( "ffmpeg", "Cannot find codec: codec_id=%d", avctx->codec_id );
			return false;
		}
	}

	//	Setup codec context
	if (!initCodec( avctx, codec )) {
		LERROR( "ffmpeg", "Cannot setup codec" );
		return false;
	}

	//	Open codec
	if (avcodec_open2(avctx, codec, NULL) < 0) {
		LERROR( "ffmpeg", "Cannot open codec" );
		return false;
	}

	return true;
}

bool Stream::initCodec( AVCodecContext *avctx, AVCodec *codec ) {
	avctx->workaround_bugs = impl::workaround_bugs;
	avctx->lowres = impl::lowres;
	if (avctx->lowres > codec->max_lowres) {
		LWARN( "ffmpeg", "The maximum value for lowres supported by the decoder is %d", codec->max_lowres);
		avctx->lowres = codec->max_lowres;
	}
	avctx->idct_algo         = impl::idct;
	avctx->skip_frame        = impl::skip_frame;
	avctx->skip_idct         = impl::skip_idct;
	avctx->skip_loop_filter  = impl::skip_loop_filter;
	avctx->error_concealment = impl::error_concealment;

	if (avctx->lowres)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;

	if (impl::fast)
		avctx->flags2 |= CODEC_FLAG2_FAST;

	if (codec->capabilities & CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;

	return true;
}

void Stream::finCodec( AVCodecContext * ) {
}

double Stream::clock() const {
	DTV_ASSERT( isOpen() );
	return _currentPtsDrift + (double)av_gettime() / 1000000.0;
}

double Stream::updatePTS( AVFrame *frame ) {
	uint64_t pts_int;

	if (impl::decoder_reorder_pts == -1) {
		pts_int = av_frame_get_best_effort_timestamp(frame);
	} else if (impl::decoder_reorder_pts) {
		pts_int = frame->pkt_pts;
	} else {
		pts_int = frame->pkt_dts;
	}

	if (pts_int == uint64_t(AV_NOPTS_VALUE)) {
		pts_int = 0;
	}

	double pts = (double)pts_int * av_q2d( _stream->time_base );

	//	compute the exact PTS for the picture if it is omitted in the stream
	//	pts1 is the dts of the pkt / pts of the frame
	// if (pts) {
	// 	//	update video clock with pts, if present
	// 	_clock = pts;
	// } else {
	// 	pts = _clock;
	// }

	return pts;
}

AVStream *Stream::stream() const {
	return _stream;
}

AVFormatContext *Stream::context() const {
	return _context;
}

System *Stream::sys() const {
	return _sys;
}

}
}

