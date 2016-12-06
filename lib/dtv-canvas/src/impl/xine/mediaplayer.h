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

#include "../../mediaplayerimpl.h"

typedef struct xine_s xine_t;
typedef struct xine_stream_s xine_stream_t;
typedef struct xine_audio_port_s xine_audio_port_t;
typedef struct xine_video_port_s xine_video_port_t;

namespace canvas {

class StreamsInfo;

namespace xine {

class MediaPlayer : public canvas::MediaPlayerImpl {
public:
	MediaPlayer( System *sys, xine_t *xine );
	virtual ~MediaPlayer();

	//	Aux
	void dstFrame( Rect &rect );

protected:
	virtual bool startImpl( const std::string &url );
	virtual void stopImpl();
	virtual void pauseImpl( bool needPause );

	virtual void muteImpl( bool needsMute );
	virtual void volumeImpl( Volume vol );

	virtual void moveResizeImpl( const Rect &rect );

	//	Streams info implementation
	virtual canvas::StreamsInfo *createStreamsInfo();
	virtual bool switchVideoStreamImpl( int id );
	virtual bool switchAudioStreamImpl( int id );
	virtual bool switchSubtitleStreamImpl( int id );

private:
	xine_t *_xine;
	xine_video_port_t *_voPort;
	xine_audio_port_t *_aoPort;
	xine_stream_t *_stream;
};

}
}

