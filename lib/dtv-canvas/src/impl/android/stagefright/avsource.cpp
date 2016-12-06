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

#include "avsource.h"
#include <util/assert.h>
#include <util/log.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaDefs.h>
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif
extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

namespace canvas {
namespace android {
namespace stagefright {

AVFormatSource::AVFormatSource()
{
	_dataSource = NULL;
	_videoTrack = NULL;
	
	//	register all codecs, demux and protocols
	avcodec_register_all();
	av_register_all();
}

AVFormatSource::~AVFormatSource()
{
}

bool AVFormatSource::initialize( const std::string &url ) {
	DTV_ASSERT(!_dataSource);
	DTV_ASSERT(!_videoTrack);

	LDEBUG( "android", "Initialize av format: url=%s", url.c_str() );
	
	_dataSource = avformat_alloc_context();
	if (!_dataSource) {
		LERROR( "android", "Cannot create av format context" );
		return false;
	}
	
	int err = avformat_open_input( &_dataSource, url.c_str(), NULL, NULL );
	if (err < 0) {
		LERROR( "android", "Cannot open stream" );
		return false;
	}

	//	Find video stream
	int videoIndex = -1;
	for (unsigned int i = 0; i < _dataSource->nb_streams; i++) {
		if (_dataSource->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}

	if (videoIndex < 0) {
		LERROR( "android", "Cannot find video stream" );
		return false;
	}
	_videoTrack = _dataSource->streams[videoIndex]->codec;

	{	//	Create format
		size_t bufferSize = (_videoTrack->width * _videoTrack->height * 3) / 2;
		_group.add_buffer(new a::MediaBuffer(bufferSize));
		_format = new a::MetaData;
	}

	switch (_videoTrack->codec_id) {
		case CODEC_ID_H264: {
			_format->setCString( a::kKeyMIMEType, a::MEDIA_MIMETYPE_VIDEO_AVC );
			if (_videoTrack->extradata[0] == 1) {
				_format->setData( a::kKeyAVCC, a::kTypeAVCC, _videoTrack->extradata, _videoTrack->extradata_size );
			}
			break;
		}
		default: {
			LERROR( "android", "Invalid codec ID" );
			return false;
		}
	}

	_format->setInt32( a::kKeyWidth, _videoTrack->width );
	_format->setInt32( a::kKeyHeight, _videoTrack->height );

	return true;
}

void AVFormatSource::finalize() {
	av_close_input_file(_dataSource);
	_dataSource = NULL;
	_videoTrack = NULL;
}

a::sp<a::MetaData> AVFormatSource::getFormat() {
	return _format;
}

a::status_t AVFormatSource::start( a::MetaData * /*params*/ ) {
	return a::OK;
}

a::status_t AVFormatSource::stop() {
	return a::OK;
}

a::status_t AVFormatSource::read( a::MediaBuffer **buffer, const a::MediaSource::ReadOptions * /*options*/ ) {
	AVPacket packet;
	a::status_t ret;
	bool found = false;

	while (!found) {
		ret = av_read_frame( _dataSource, &packet );
		if (ret < 0) {
			return a::ERROR_END_OF_STREAM;
		}

		if (packet.stream_index == _videoIndex) {
			// if (mConverter) {
			// 	av_bitstream_filter_filter(mConverter, mVideoTrack, NULL, &packet.data, &packet.size, packet.data, packet.size, packet.flags & AV_PKT_FLAG_KEY);
			// }

			ret = _group.acquire_buffer(buffer);
			if (ret == a::OK) {
				memcpy((*buffer)->data(), packet.data, packet.size);
				(*buffer)->set_range(0, packet.size);
				(*buffer)->meta_data()->clear();
				(*buffer)->meta_data()->setInt32(a::kKeyIsSyncFrame, packet.flags & AV_PKT_FLAG_KEY);
				(*buffer)->meta_data()->setInt64(a::kKeyTime, packet.pts);
			}
			found = true;
		}
		
		av_free_packet(&packet);
	}

	return ret;	
}

}
}
}

