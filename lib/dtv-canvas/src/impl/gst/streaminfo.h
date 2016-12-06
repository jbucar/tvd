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

#pragma once

#include "../../streaminfo.h"

// struct libvlc_media_player_t;
// struct libvlc_media_track_t;
typedef struct _GstElement GstElement;

namespace canvas {
namespace gst {

template<class T>
class GSTStreamInfo : public T {
public:
	explicit GSTStreamInfo( int id ) : _id(id) {}
	virtual ~GSTStreamInfo() {}

	int id() const { return _id; }

private:
	int _id;
};
typedef GSTStreamInfo<VideoStreamInfo> GSTVideoSInfo;
typedef GSTStreamInfo<AudioStreamInfo> GSTAudioSInfo;
typedef GSTStreamInfo<SubtitleStreamInfo> GSTSubtitleSInfo;

class StreamsInfo : public canvas::StreamsInfo {
public:
	StreamsInfo();
	virtual ~StreamsInfo();

	void init( GstElement *pipeline );
	void fin();

	virtual void refreshImpl();

protected:
	virtual void parseImpl();

	void refreshVideo( size_t id );
	void refreshAudio( /*size_t id, libvlc_media_track_t *vTrack */){};
	void refreshSubtitle( /*size_t id, libvlc_media_track_t *vTrack */){};

private:
	GstElement *_pipeline;
};

}
}