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

#include "videooverlay.h"
#include "window.h"
#include "../../color.h"
#include "../../surface.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <QWidget>

namespace canvas {
namespace qt {

VideoOverlay::VideoOverlay( Window *win, int zIndex )
	: canvas::VideoOverlay( zIndex ), _win(win)
{
	_widget = NULL;
}

VideoOverlay::~VideoOverlay()
{
	delete _widget;
}

bool VideoOverlay::create() {
	LDEBUG("qt::VideoOverlay", "Create");
	_widget = new QWidget( _win->main() );
	return true;
}

void VideoOverlay::destroy() {
	LDEBUG("qt::VideoOverlay", "Stop");
	DEL(_widget);
}

void VideoOverlay::show() {
	_widget->show();
}

void VideoOverlay::hide() {
	_widget->hide();
}

bool VideoOverlay::moveImpl( const Point &point ) {
	_widget->move( point.x, point.y );
	return true;
}

bool VideoOverlay::resizeImpl( const Size &size ) {
	_widget->setFixedSize( size.w, size.h );
	return true;
}

canvas::Window *VideoOverlay::win() const {
	return _win;
}

bool VideoOverlay::getDescription( VideoDescription &desc ) {
	DTV_ASSERT(_widget);

#ifdef _WIN32
	desc = _widget->winId();
#else
	desc.display = NULL;
	desc.screenID = 0;
	desc.winID = _widget->winId();
#endif
	return true;
}

}
}

