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
#include "qview.h"
#include "system.h"
#include "../../size.h"
#include "../../point.h"
#include "../../inputmanager.h"
#include <QResizeEvent>

namespace canvas {
namespace qt {

QView::QView(QGraphicsScene *scene, Window *win, System *sys) : QGraphicsView(scene, NULL), _win(win), _sys(sys)
{
	this->installEventFilter(this);
}

QView::~QView()
{
}

void QView::resizeEvent(QResizeEvent *event) {
	const QSize s = event->size();
	_win->onResizeEvent( Size(s.width(), s.height()) );
}

void QView::keyPressEvent(QKeyEvent *event) {
	util::key::type key = getKeyCode( event->key() );
	if (key == util::key::null) {
		QWidget::keyPressEvent( event );
	} else {
		_sys->input()->dispatchKey( key, false );
		event->accept();
	}
}

void QView::keyReleaseEvent(QKeyEvent *event) {
	util::key::type key = getKeyCode( event->key() );
	if (key == util::key::null) {
		QWidget::keyReleaseEvent( event );
	} else {
		_sys->input()->dispatchKey( key, true );
		event->accept();
	}
}

void QView::mousePressEvent(QMouseEvent* event) {
	sendMouseNotify(event, true);
}

void QView::mouseReleaseEvent(QMouseEvent* event) {
	sendMouseNotify(event, false);
}

void QView::sendMouseNotify( QMouseEvent* event, bool isPress) {
	if (event->button() <= 5) {
		input::ButtonEvent evt;
		evt.button = event->button();
		evt.x = event->x();
		evt.y = event->y();
		evt.isPress = isPress;
		input::dispatchButtonEvent( _sys, &evt );
		event->accept();
	}
}

void QView::closeEvent(QCloseEvent *event) {
	_sys->onWindowDestroyed();
	event->ignore();
}

bool QView::eventFilter(QObject */*object*/, QEvent *event) {
	if (event->type() == QEvent::WindowActivate) {
		_win->iconify(false);
		return false;
	}
	return false;
}

}
}
