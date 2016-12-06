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
#include "videooverlay.h"
#include "surface.h"
#include "canvas.h"
#include "qlayer.h"
#include "qview.h"
#include "system.h"
#include <util/cfg/cfg.h>
#include <util/keydefs.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <util/log.h>
#include <QWidget>
#include <QGraphicsProxyWidget>
#include <QString>
#include <QIcon>
#include <QApplication>
#include <QDesktopWidget>

namespace canvas {
namespace qt {

Window::Window( System *sys )
{
	_surface = NULL;

	//	Create main widget
	QGraphicsScene *scene = new QGraphicsScene();
	_main = new QView(scene, this, sys);

	//	Create canvas layer
	_layer = new QLayer(this);
	addItem( _layer, 2 );
}

bool Window::init() {
	//	Setup
	_main->setStyleSheet("background:black");
	_main->setWindowTitle( title().c_str() );
	_main->setFrameStyle ( QFrame::NoFrame );

	if (!icon().empty()) {
		QIcon qicon(icon().c_str());
		_main->window()->setWindowIcon( qicon );
	}

	// 	init size
	initSize();

	_main->show();

	_layer->setFocus();
	return true;
}

Window::~Window()
{
	DEL(_layer);
	DEL(_surface);
	DEL(_main);
}

void Window::addItem( QWidget *w, int zIndex )  {
	QGraphicsProxyWidget *proxy=_main->scene()->addWidget( w );
	proxy->setZValue( zIndex );
}

QWidget *Window::main() const {
	return _main;
}

QWidget *Window::canvasWidget() const {
	return _layer;
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	//	Create surface
	const Size &s = canvas->size();
	_surface = new Surface( (canvas::qt::Canvas *)canvas, Rect(0,0,s.w,s.h) );

	//	Setup layer for canvas
	_layer->setFixedSize( s.w, s.h );
	_layer->setImg(_surface->getImage());
	return true;
}

void Window::finLayer( canvas::Canvas * /*canvas*/ ) {
	DEL(_surface);
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Window::renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	DTV_ASSERT( surface == _surface );
	UNUSED(surface);
	BOOST_FOREACH( const Rect &r, dirtyRegions ) {
		_layer->updateRect(r);
	}
}

//	Overlays
bool Window::supportVideoOverlay() const {
	return true;
}

canvas::VideoOverlay *Window::createOverlayInstance( int zIndex ) const {
	return new VideoOverlay( (Window *)this, zIndex );
}

//	Size methods
void Window::setSizeImpl( const Size &s ) {
	_main->setGeometry(0, 0, s.w, s.h);
	_main->setMaximumSize(s.w, s.h);
	_main->setMinimumSize(s.w, s.h);
}

void Window::onResizeEvent( const Size &size ) {
	_layer->setFixedSize(size.w, size.h);
	_layer->updateRect(Rect(0, 0, size.w, size.h));
}

bool Window::supportFullScreen() const {
	return true;
}

Size Window::screenSize() const {
	QRect rect = qApp->desktop()->screenGeometry();
	return Size(rect.width(),rect.height());
}

void Window::fullScreenImpl( bool enable ) {
	if (enable) {
		_main->showFullScreen();
	} else {
		_main->showNormal();
	}
}

void Window::iconifyImpl( bool enable ) {
	if (enable) {
		_main->showMinimized();
	} else  {
		_main->show();
	}
}

}
}

