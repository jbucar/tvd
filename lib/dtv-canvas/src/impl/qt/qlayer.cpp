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

#include "window.h"
#include "qlayer.h"
#include <util/log.h>
#include <QKeyEvent>
#include <QPainter>
#include <QRect>

namespace canvas {
namespace qt {

QLayer::QLayer( Window *win ) : _win(win){
	setStyleSheet("background:transparent");
	setAttribute(Qt::WA_NoSystemBackground);
}

QImage *QLayer::getImage() {
	return _img;
}

void QLayer::setImg( QImage *img ) {
	_img=img;
}

void QLayer::paintEvent ( QPaintEvent *event ) {
	//	Painting is clipped to region() during the processing of a paint event
	QPainter p(this);
	p.setClipRegion( event->region() );
	p.setCompositionMode( QPainter::CompositionMode_SourceOver );

	const Rect &scaledBnds = _win->targetWindow();
	QPoint pos(scaledBnds.x, scaledBnds.y);
	QImage tmp = _img->scaled(scaledBnds.w, scaledBnds.h);
	p.drawImage( pos, tmp );
}

void QLayer::updateRect( const Rect &/*r*/ ) {
	// update() does not cause an immediate repaint;
	//instead it schedules a paint event for processing when Qt returns to the main event loop.
	//This permits Qt to optimize for more speed and less flicker than a call to repaint() does.
	update();
}

}	// End qt namespace
} 	// End canvas namespace
