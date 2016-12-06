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
#include "stream.h"
#include "videostream.h"
#include "audiostream.h"
#include "streaminfo.h"
#include "../../window.h"
#include "../../system.h"
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <boost/foreach.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
}

namespace canvas {
namespace ffmpeg {

namespace impl {

static void do_log( void * /*ptr*/, int /*level*/, const char* /*fmt*/, va_list /*vl*/ ) {	// TODO
	// switch (level) {
	// 	case AV_LOG_PANIC:
	// 		verbose_level = LOG_EMERG;
	// 		break;
	// 	case AV_LOG_FATAL:
	// 		verbose_level = LOG_CRIT;
	// 		break;
	// 	case AV_LOG_ERROR:
	// 		verbose_level = LOG_ERR;
	// 		verbose_mask |= VB_LIBAV;
	// 		break;
	// 	case AV_LOG_DEBUG:
	// 	case AV_LOG_VERBOSE:
	// 	case AV_LOG_INFO:
	// 		verbose_level = LOG_DEBUG;
	// 		verbose_mask |= VB_LIBAV;
	// 		break;
	// 	case AV_LOG_WARNING:
	// 		verbose_mask |= VB_LIBAV;
	// 		break;
	// 	default:
	// 		return;
	// }
}

static int decode_interrupt_cb( void *ctx ) {
	DTV_ASSERT(ctx);
	MediaPlayer *mp = (MediaPlayer *)ctx;
	return mp->stopInput();
}

}	//	namespace impl


MediaPlayer::MediaPlayer( System *sys )
	: canvas::MediaPlayer( sys )
{
	_ic = NULL;
	_exit = true;

	_streams[stream::video] = new VideoStream( sys );
	_streams[stream::audio] = new AudioStream( sys );
	_streams[stream::subtitle] = new Stream( sys, stream::subtitle );
}

MediaPlayer::~MediaPlayer()
{
	for (int i=0; i<stream::last; i++) {
		delete _streams[i];
	}
}

bool MediaPlayer::startImpl( const std::string &url ) {
	LINFO( "ffmpeg", "MediaPlayer: start url: %s", url.c_str() );

	//	Setup log
	av_log_set_level( AV_LOG_ERROR );
	av_log_set_callback(impl::do_log);
	av_log_set_flags(AV_LOG_SKIP_REPEATED);

	//	register all codecs, demux and protocols
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	_exit = false;

	if (!open( url )) {
		LERROR("ffmpeg", "Fail to open url: %s", url.c_str());
		return false;
	}

	for (int i=0; i<stream::last; i++) {
		_streams[i]->start();
	}
	_iThread = boost::thread( boost::bind(&MediaPlayer::inputThread,this) );

	return true;
}

void MediaPlayer::stopImpl() {
	LINFO( "ffmpeg", "MediaPlayer: stop" );

	//	Wakeup input thread
	_exit = true;
	_iWakeup.notify_all();
	_iThread.join();

	for (int i=0; i<stream::last; i++) {
		_streams[i]->stop();
	}

	//	Close streams
	close();

	av_lockmgr_register(NULL);
	avformat_network_deinit();
}

void MediaPlayer::pauseImpl( bool /*needPause*/ ) {
}

//	Volume
void MediaPlayer::muteImpl( bool /*needMute*/ ) {
}

void MediaPlayer::volumeImpl( Volume /*vol*/ ) {
}

void MediaPlayer::moveResizeImpl( const Rect & /*size*/ ) {
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	LDEBUG("ffmpeg", "Switch video stream id=%d", id);
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	LDEBUG("ffmpeg", "Switch audio stream id=%d", id);
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	LDEBUG("ffmpeg", "Switch subtitle stream id=%d", id);
	return true;
}

bool MediaPlayer::open( const std::string &url ) {
	//	Setup AV format
	_ic = avformat_alloc_context();
	_ic->interrupt_callback.callback = impl::decode_interrupt_cb;
	_ic->interrupt_callback.opaque = this;

	//	Open stream
	int err = avformat_open_input( &_ic, url.c_str(), NULL, NULL );
	if (err < 0) {
		LERROR( "ffmpeg", "cannot open input file: file=%s, err=%d", url.c_str(), err );
		close();
		return false;
	}

	// 	if (genpts)
	// 		ic->flags |= AVFMT_FLAG_GENPTS;

	//	Try find streams
	err = avformat_find_stream_info(_ic, NULL);
	if (err < 0) {
		LERROR( "ffmpeg" "Could not find codec parameters: url=%s", url.c_str() );
		close();
		return false;
	}

	if (_ic->pb) {
		_ic->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use url_feof() to test for the end
	}

	for (unsigned int i=0; i<_ic->nb_streams; i++) {
		_ic->streams[i]->discard = AVDISCARD_ALL;
	}

	av_dump_format(_ic, 0, url.c_str(), 0);

	{	//	Open streams
		bool isOpen=false;
		for (size_t i=0; i<stream::last; i++) {
			isOpen |= _streams[i]->open( _ic );
		}

		return isOpen;
	}
}

void MediaPlayer::close() {
	//	Close streams
	for (int i=0; i<stream::last; i++) {
		_streams[i]->close();
	}

	if (_ic) {
		avformat_close_input(&_ic);
		_ic = NULL;
	}
}

//	Input aux
int MediaPlayer::stopInput() const {
	return _exit ? 1 : 0;
}

bool MediaPlayer::areQueuesFull() const {
	bool isFull = false;
	for (int i=0; i<stream::last; i++) {
		isFull |= _streams[i]->isFull();
	}
	return isFull;
}

void MediaPlayer::inputThread() {
	AVPacket pkt1, *pkt = &pkt1;
	bool isFull = false;
	bool exit = false;

	LDEBUG( "ffmpeg", "Input thread started" );

	//	Loop!
	while (!exit) {
		{	//	Wair for full condition or exit
			boost::unique_lock<boost::mutex> lock( _iMutex );
			while (!_exit && isFull) {
				_iWakeup.timed_wait( lock, boost::get_system_time()+boost::posix_time::milliseconds(10) );
				isFull = areQueuesFull();
			}

			exit = _exit;
		}

		//	Read next frame
		int ret = av_read_frame(_ic, pkt);
		if (ret < 0) {
			if (ret == AVERROR_EOF || url_feof(_ic->pb)) {
				LDEBUG( "ffmpeg", "Read frame error: EOF" );
				exit = true;
			}

			if (_ic->pb && _ic->pb->error) {
				LERROR( "ffmpeg", "Read frame error: pb error" );
				break;
			}

			continue;
		}

		//	Enqueue packet
		Stream *s = getStreamByIndex( pkt->stream_index );
		if (s) {
			isFull = s->push( pkt );
		}
		else {
			LWARN( "ffmpeg", "Stream index error: index=%d", pkt->stream_index );
			av_free_packet(pkt);
		}
	}	//	end loop!

	LDEBUG( "ffmpeg", "Input thread stopped" );
}

//	Streams
Stream *MediaPlayer::getStreamByIndex( int sIndex ) const {
	for (int i=0; i<stream::last; i++) {
		if (_streams[i]->index() == sIndex) {
			return _streams[i];
		}
	}
	return NULL;
}

Stream *MediaPlayer::getStream( stream::type t ) const {
	DTV_ASSERT( t >= 0 && t < stream::last );
	return _streams[t];
}

//	Clocks
double MediaPlayer::getExternalClock() const {
	// int64_t ti;
	// ti = av_gettime();
	// return _externalClock + ((ti - _externalClockTime) * 1e-6);
	return 0.0;
}

double MediaPlayer::getMasterClock() const {
	double val;
	Stream *vStream = getStream( stream::video );
	Stream *aStream = getStream( stream::audio );
	DTV_ASSERT( vStream || aStream );

	if (_syncClock == sync::video) {
		val = vStream ? vStream->clock() : aStream->clock();
	} else if (_syncClock == sync::audio) {
		val = aStream ? aStream->clock() : vStream->clock();
	} else {
		val = getExternalClock();
	}

	return val;
}

}
}

