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

#include "mediaplayeradapter.h"
#include "../../../zapper.h"
#include <canvas/mediaplayer.h>
#include <canvas/player.h>
#include <canvas/system.h>
#include <util/assert.h>

namespace zapper {
namespace middleware {
namespace ginga {

MediaPlayerAdapter::MediaPlayerAdapter( Zapper *zapper )
{
	canvas::System *sys = zapper->sys();
	_media = sys->player()->create( sys );
	DTV_ASSERT(_media);
}

MediaPlayerAdapter::~MediaPlayerAdapter()
{
	canvas::MediaPlayer::destroy( _media );
}

const std::string MediaPlayerAdapter::name() {
	return "MediaPlayer";
}

bool MediaPlayerAdapter::playImpl( const std::string &url ) {
	if (_media) {
		_media->setFullScreen();
		_media->play( url );
		return true;
	}
	return false;
}

void MediaPlayerAdapter::stopImpl() {
	_media->stop();
}

void MediaPlayerAdapter::moveResizeImpl( int x, int y, int w, int h ) {
	canvas::Rect r(x,y,w,h);
	_media->moveResize( r );
}

}
}
}

