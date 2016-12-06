/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "videoplayer.h"
#include "soundproperties.h"
#include "../device.h"
#include <canvas/mediaplayer.h>
#include <canvas/player.h>
#include <canvas/window.h>
#include <canvas/surface.h>
#include <canvas/system.h>
#include <util/mcr.h>

namespace player {

VideoPlayer::VideoPlayer( Device *dev )
	: GraphicPlayer( dev )
{
	_media = dev->createMediaPlayer();
	_sound = new SoundProperties( _media );

	_media->onStop().connect( boost::bind(&VideoPlayer::stop,this) );
}

VideoPlayer::~VideoPlayer()
{
	delete _sound;
	device()->destroy(_media);
}

bool VideoPlayer::startPlay() {
	if (GraphicPlayer::startPlay()) {
		//	Initialize media player
		_media->moveResize(surface()->getBounds());
		return _media->play( (schema() == schema::file) ? body() : url() );
	}
	return false;
}

void VideoPlayer::stopPlay() {
	_media->stop();
	GraphicPlayer::stopPlay();
}

void VideoPlayer::registerProperties() {
	GraphicPlayer::registerProperties();
	_sound->registerProperties(this);
}

void VideoPlayer::pausePlay( bool pause ) {
	if (pause) {
		_media->pause();
	} else {
		_media->unpause();
	}
}

//	Events
void VideoPlayer::onBoundsChanged( const canvas::Rect &rect ) {
	//	Set bounds
	_media->moveResize( rect );
}

void VideoPlayer::onSizeChanged( const canvas::Size &size ) {
	//	Resize
	_media->resize( size );
}

void VideoPlayer::onPositionChanged( const canvas::Point &point ) {
	//	Move video widget
	_media->move( point );
}

bool VideoPlayer::supportSchemma( schema::type sch ) const {
	return sch == schema::file || sch == schema::rtp || sch == schema::rtsp || sch == schema::sbtvd;
}

canvas::composition::mode VideoPlayer::flushCompositionMode() const {
	return canvas::composition::source;
}

}
