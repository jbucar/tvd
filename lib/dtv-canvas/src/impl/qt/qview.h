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

#include <QGraphicsView>

namespace canvas {
namespace qt {

class Window;

util::key::type getKeyCode( int symbol );

class QView : public QGraphicsView {
public:
	QView(QGraphicsScene *scene, Window *win, System *sys);
	virtual ~QView();

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void keyPressEvent( QKeyEvent *event );
	virtual void keyReleaseEvent( QKeyEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event );
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void closeEvent( QCloseEvent *event );

	virtual bool eventFilter(QObject *object, QEvent *event);

	//	Aux mouse event
	void sendMouseNotify( QMouseEvent* event, bool isPress);

private:
	Window *_win;
	System *_sys;

};


}
}