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

#include "streaminfo.h"
#include <aml/aml.h>
#include <aml/ts.h>
#include <util/log.h>
#include <util/assert.h>
#include <string.h>

namespace aml {

template<class T>
class GSTStreamInfo : public T {
public:
	explicit GSTStreamInfo( int id ) : _id(id) {}
	virtual ~GSTStreamInfo() {}

	int id() const { return _id; }

private:
	int _id;
};
typedef GSTStreamInfo<canvas::VideoStreamInfo> GSTVideoSInfo;
typedef GSTStreamInfo<canvas::AudioStreamInfo> GSTAudioSInfo;
typedef GSTStreamInfo<canvas::SubtitleStreamInfo> GSTSubtitleSInfo;

StreamsInfo::StreamsInfo()
{
	_pipeline = NULL;
	_ts = NULL;
}

StreamsInfo::~StreamsInfo()
{
}

void StreamsInfo::init( ts::Player *ts ) {
	LDEBUG("aml", "StreamsInfo init!");
	_pipeline = NULL;
	_ts = ts;
}

void StreamsInfo::init( GstElement *pipeline ) {
	LDEBUG("aml", "StreamsInfo init!");
	DTV_ASSERT(pipeline);
	_pipeline = pipeline;
	_ts = NULL;
}

void StreamsInfo::fin() {
	LDEBUG("aml", "StreamsInfo fin!");
	_pipeline = NULL;
	_ts = NULL;
}

void StreamsInfo::parseImpl() {
	LDEBUG("aml", "StreamsInfo parse!");

	if (_pipeline) {
		// gint n_video = -1;
		// gint current_video = -1;
		// g_object_get( _pipeline, "n-video", &n_video, NULL );
		// g_object_get( _pipeline, "current-video", &current_video, NULL );

		// for( gint i=0; i < n_video; i++ ) {
		// 	LDEBUG("aml", "Parse stream %d", i);
		// 	VideoStreamInfo *vInfo = new GSTVideoSInfo( i );
		// 	addVideoInfo( vInfo );
		// }
		// currentVideo(current_video);
		// refreshImpl();
	}
	else {
		addVideoInfo(new canvas::VideoStreamInfo());
		currentVideo(0);

		addAudioInfo(new canvas::AudioStreamInfo());
		currentAudio(0);

		addSubtitleInfo(new canvas::SubtitleStreamInfo());
		currentSubtitle(0);
	}
}

void StreamsInfo::refreshImpl() {
	LDEBUG("aml", "StreamsInfo refresh!");

	if (_ts) {
		{	// Refresh video info
			int w, h, fps;
			if (_ts->getVideoInfo( w, h, fps )) {
				canvas::VideoStreamInfo *vInfo = videoInfo(0);
				vInfo->size( canvas::Size(w, h) );
				vInfo->framerate( fps );
				vInfo->interleaved( true );
			}
		}

		{	// Refresh audio info
			int nChannels, rate;
			if (_ts->getAudioInfo( nChannels, rate )) {
				canvas::AudioStreamInfo *aInfo = audioInfo(0);
				aInfo->samplerate(rate);
				aInfo->channels(nChannels);
			}
		}
	}
}

}
