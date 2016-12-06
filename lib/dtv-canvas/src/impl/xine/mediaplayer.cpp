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
#include "../../streaminfo.h"
#include "../../point.h"
#include "../../rect.h"
#include <util/assert.h>
#include <util/log.h>
#include <xine.h>
#include <xine/xineutils.h>

namespace canvas {
namespace xine {

static void frame_output_cb(
	void *data,
	int /*video_width*/,
	int /*video_height*/,
	double video_pixel_aspect,
	int *dest_x,
	int *dest_y,
	int *dest_width,
	int *dest_height,
	double *dest_pixel_aspect,
	int *win_x,
	int *win_y)
{
	MediaPlayer *mp = (MediaPlayer *)data;
	DTV_ASSERT(mp);

	Rect rect;
	mp->dstFrame( rect );
	
	*dest_x      = rect.x;
	*dest_y      = rect.y;
	*win_x       = rect.x;
	*win_y       = rect.y;
	*dest_width  = rect.w;
	*dest_height = rect.h;
	*dest_pixel_aspect = video_pixel_aspect;
}

class StreamsInfo : public canvas::StreamsInfo {
public:
	StreamsInfo() {}
	virtual ~StreamsInfo() {}

protected:
	virtual void refreshImpl() {
		LDEBUG("xine", "StreamsInfo refresh");
		// TODO:
	}

	virtual void parseImpl() {
		LDEBUG("xine", "StreamsInfo parse");
		// TODO:
	}
};

MediaPlayer::MediaPlayer( System *sys, xine_t *xine )
	: MediaPlayerImpl( sys )
{
	_xine   = xine;
	DTV_ASSERT(_xine);
	_stream = NULL;
	_aoPort = NULL;
	_voPort = NULL;
}

MediaPlayer::~MediaPlayer( void )
{
	DTV_ASSERT(!_stream);
	DTV_ASSERT(!_aoPort);
	DTV_ASSERT(!_voPort);
}

//	Initialization
bool MediaPlayer::startImpl( const std::string &url ) {
	LDEBUG("xine", "Start: url=%s", url.c_str());

	{	//	Setup visual
		setupOverlay();
		VideoDescription desc;
		if (getVideoDescription( desc )) {
			LDEBUG( "xine", "Using WinID=0x%x", desc.winID );
			
			x11_visual_t vis;
			vis.display         = desc.display;
			vis.screen          = desc.screenID;
			vis.d               = desc.winID;
			vis.frame_output_cb = frame_output_cb;
			vis.user_data       = this;

			//double pixel_aspect   = size.w / size.h;
			// /if (fabs(pixel_aspect - 1.0) < 0.01) {
			// 	pixel_aspect = 1.0;
			// }

			_voPort = xine_open_video_driver(_xine, "auto", XINE_VISUAL_TYPE_X11, (void *)&vis);
			if (!_voPort) {
				LERROR("xine", "Fail creating video driver");
				stopImpl();
				return false;
			}
		}

		_aoPort = xine_open_audio_driver(_xine, "auto", NULL);
		if (!_aoPort) {
			LERROR("xine", "Fail creating audio driver");
			stopImpl();
			return false;
		}
	}

	//	Create stream
	_stream = xine_stream_new(_xine, _aoPort, _voPort);
	if (!_stream) {
		LERROR("xine", "Fail creating stream");
		stopImpl();
		return false;
	}

	if (!xine_open( _stream, url.c_str() )) {
		LERROR("xine", "Fail to open stream! error_code=%d", xine_get_error(_stream));
		stopImpl();
		return false;
	}

	xine_play( _stream, 0, 0 );
	return true;
}

void MediaPlayer::stopImpl() {
	LDEBUG("xine", "Stop");

	if (_stream) {
		xine_stop( _stream );
		xine_close( _stream );
		xine_dispose( _stream );
		_stream = NULL;
	}

	if (_aoPort) {
		xine_close_audio_driver(_xine, _aoPort);
		_aoPort = NULL;		
	}
	
	if (_voPort) {
		xine_close_video_driver(_xine, _voPort);
		_voPort = NULL;
	}

	destroyOverlay();
}

void MediaPlayer::pauseImpl( bool needPause ) {
	LDEBUG("xine", "Pause: value=%d", needPause);
}

void MediaPlayer::muteImpl( bool needsMute ) {
	LDEBUG("xine", "mute: value=%d", needsMute);
}

void MediaPlayer::volumeImpl( Volume vol ) {
	LDEBUG("xine", "volume: value=%d", vol);
}

void MediaPlayer::moveResizeImpl( const Rect &rect ) {
	LDEBUG("xine", "Move and resize: rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h);
}

canvas::StreamsInfo *MediaPlayer::createStreamsInfo() {
	return new StreamsInfo();
}

bool MediaPlayer::switchVideoStreamImpl( int id ) {
	LDEBUG("xine", "Switch video stream, id=%d", id);
	return true;
}

bool MediaPlayer::switchAudioStreamImpl( int id ) {
	LDEBUG("xine", "Switch audio stream, id=%d", id);
	return true;
}

bool MediaPlayer::switchSubtitleStreamImpl( int id ) {
	LDEBUG("xine", "Switch subtitle stream, id=%d", id);
	return true;
}

//	Aux
void MediaPlayer::dstFrame( Rect &rect ) {
	getBounds( rect );
	rect.x = 0;
	rect.y = 0;
}

}
}
