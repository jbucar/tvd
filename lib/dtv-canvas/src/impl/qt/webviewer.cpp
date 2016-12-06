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

#include "webviewer.h"
#include "qlayer.h"
#include "window.h"
#include "../../surface.h"
#include "../../canvas.h"
#include <QWebView>
#include <QSize>
#include <QUrl>
#include <QWidget>

namespace canvas {
namespace qt {

WebViewer::WebViewer( canvas::System *sys, canvas::Surface *surface, Window *win )
	: canvas::WebViewer(surface), _sys(sys)
{
	_view = new QWebView();
	win->addItem( _view, 1 );
}

WebViewer::~WebViewer( void )
{
	delete _view;
}

void WebViewer::resize( const Size &size ) {
	_view->resize(size.w,size.h);
}

void WebViewer::move( const Point &point ) {
	_view->move(point.x,point.y);
}

bool WebViewer::load( const std::string &file ) {
	//	Configure transparent surface
	surface()->flushCompositionMode(composition::source);

	//	Positioning and resize webview
	resize( surface()->getSize() );
	move( surface()->getLocation() );

	//	Load view
	QUrl url;
	url = QUrl(file.c_str());
	_view->load(url);

	//	Show
	_view->show();

	return true;
}

QWebView *WebViewer::getView() {
	return _view;
}

void WebViewer::stop() {
}

void WebViewer::draw() {
}

}
}
