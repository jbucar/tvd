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

#include "htmlplayer.h"
#include "../device.h"
#include <canvas/webviewer.h>
#include <util/mcr.h>
#include <boost/bind.hpp>

namespace player {

HtmlPlayer::HtmlPlayer( Device *dev )
	: GraphicPlayer( dev )
{
}

HtmlPlayer::~HtmlPlayer()
{
}

bool HtmlPlayer::startPlay() {
	bool result=false;
	if (GraphicPlayer::startPlay()) {
		//	Create web viewer
		_html = device()->createWebViewer( surface() );
		if (_html) {
			//	Load url
			_html->load( url() );
			result=true;
		}
	}
	return result;
}

void HtmlPlayer::stopPlay() {
	_html->stop();
	GraphicPlayer::stopPlay();
	DEL(_html);
}

//	Events
void HtmlPlayer::onSizeChanged( const canvas::Size &size ) {
	//	Redraw html
	_html->resize( size );
}

void HtmlPlayer::onPositionChanged( const canvas::Point &point ) {
	_html->move( point );
}

void HtmlPlayer::refresh() {
	_html->draw();
}

bool HtmlPlayer::supportRemote() const {
	return true;
}

}
