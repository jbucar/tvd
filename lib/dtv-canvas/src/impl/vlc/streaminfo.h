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

struct libvlc_media_player_t;
struct libvlc_media_track_t;

namespace canvas {
namespace vlc {

template<class T>
class VLCStreamInfo : public T {
public:
	explicit VLCStreamInfo( int id ) : _id(id) {}
	virtual ~VLCStreamInfo() {}

	int id() const { return _id; }

private:
	int _id;
};
typedef VLCStreamInfo<VideoStreamInfo> VLCVideoSInfo;
typedef VLCStreamInfo<AudioStreamInfo> VLCAudioSInfo;
typedef VLCStreamInfo<SubtitleStreamInfo> VLCSubtitleSInfo;

class StreamsInfo : public canvas::StreamsInfo {
public:
	StreamsInfo();
	virtual ~StreamsInfo();

	void init( libvlc_media_player_t *mp );
	void fin();

protected:
	virtual void parseImpl();
	virtual void refreshImpl();

	void refreshVideo( size_t id, libvlc_media_track_t *vTrack );
	void refreshAudio( size_t id, libvlc_media_track_t *vTrack );
	void refreshSubtitle( size_t id, libvlc_media_track_t *vTrack );

private:
	libvlc_media_player_t *_mp;
};

}
}