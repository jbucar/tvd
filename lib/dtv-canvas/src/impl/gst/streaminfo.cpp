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
#include <gst/gst.h>

namespace canvas {
namespace gst {

StreamsInfo::StreamsInfo()
{
	_pipeline = NULL;
}

StreamsInfo::~StreamsInfo()
{
	_pipeline = NULL;
}

void StreamsInfo::init( GstElement *pipeline ) {
	LDEBUG("gst", "StreamsInfo init!");
	DTV_ASSERT(pipeline);
	_pipeline = pipeline;
}

void StreamsInfo::fin() {
	LDEBUG("gst", "StreamsInfo fin!");
	_pipeline = NULL;
}

void StreamsInfo::parseImpl() {
	LDEBUG("gst", "StreamsInfo parse!");
	if (_pipeline) {
		gint n_video = -1;
		gint current_video = -1;
		g_object_get( _pipeline, "n-video", &n_video, NULL );
		g_object_get( _pipeline, "current-video", &current_video, NULL );
		
		for( gint i=0; i < n_video; i++ ) {
			LDEBUG("gst", "Parse stream %d", i);
			VideoStreamInfo *vInfo = new GSTVideoSInfo( i );
			addVideoInfo( vInfo );
		}
		currentVideo(current_video);
		refreshImpl();
	}
}

void StreamsInfo::refreshImpl() {
}

void StreamsInfo::refreshVideo( size_t /*id*/ ) {
}

}
}