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

#include "mediaplayerimpl.h"
#include "videooverlay.h"
#include "point.h"
#include "window.h"
#include "system.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

namespace canvas {

MediaPlayerImpl::MediaPlayerImpl( System *sys ) : MediaPlayer( sys )
{
	_video = NULL;
}

MediaPlayerImpl::~MediaPlayerImpl()
{
	DTV_ASSERT(!_video);
}

bool MediaPlayerImpl::setupOverlay() {
	DTV_ASSERT(!_video);
	Window *win = system()->window();
	canvas::Rect rect = bounds();
	if (rect.isEmpty()) {
		rect = Rect(Point(0,0), win->getCanvasSize());
	}

	if (win->supportVideoOverlay()) {
		//	Create video overlay
		_video = win->createVideoOverlay();
		if (!_video) {
			LWARN( "MediaPlayerImpl", "Cannot create a new video overlay" );
			return false;
		}
		_video->scale( vos::notFullScreen );

		//	Initialize video overlay
		if (!_video->initialize( rect )) {
			win->destroyVideoOverlay( _video );
			LWARN( "MediaPlayerImpl", "Cannot initialize a video overlay" );
			return false;
		}
	}

	return true;
}

void MediaPlayerImpl::destroyOverlay() {
	if (_video) {
		_video->finalize();
		system()->window()->destroyVideoOverlay( _video );
	}
}

bool MediaPlayerImpl::getVideoDescription( VideoDescription &desc ) {
	if (_video) {
		return _video->getDescription( desc );
	}
	return false;
}

void MediaPlayerImpl::moveResizeImpl( const Rect &rect ) {
	if (_video) {
		_video->moveResize( rect );
	}
}

void MediaPlayerImpl::getBounds( Rect &rect ) const {
	if (_video) {
		rect=_video->windowBounds();
	}
	else {
		rect = Rect();
	}
}

}
