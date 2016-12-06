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
#include "surface.h"
#include "impl/dummy/dummywebviewer.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include "generated/config.h"
#if CANVAS_HTML_USE_CEF
#include "impl/cef/webviewer.h"
#endif


namespace canvas {

/**
 * Crea una instancia utilizando el @c Surface @b s.
 * @param sys El @c System
 * @param surface @c Surface utilizada para renderizar el contenido html.
 * @return Una nueva instancia de @c WebViewer.
 */
WebViewer *WebViewer::create( System *sys, Surface *surface ) {
#if CANVAS_HTML_USE_CEF
	return new cef::WebViewer( sys, surface );
#else
	UNUSED(sys);
	return new dummy::WebViewer( surface );
#endif
}

/**
 * Constructor base.
 * @param surface @c Surface utilizada para renderizar el contenido html.
 */
WebViewer::WebViewer( canvas::Surface *surface )
	: _surface( surface )
{
	_surface->setCompositionMode( composition::source );
}

/**
 * Destructor base.
 */
WebViewer::~WebViewer( void )
{
}

/**
 * Cambia el tamaño del @c Surface utilizado para renderizar.
 * @param size El nuevo tamaño del @c Surface.
 */
void WebViewer::resize( const Size &size ) {
	LDEBUG("WebViewer", "Surface resized: width=%d, height=%d", size.w, size.h );
}

/**
 * Cambia la posición del @c Surface utilizado para renderizar.
 * @param point La nueva posición del @c Surface.
 */
void WebViewer::move( const Point &point ) {
	LDEBUG("WebViewer", "Surface moved: x=%d, y=%d", point.x, point.y );
}

Surface *WebViewer::surface() {
	DTV_ASSERT(_surface);
	return _surface;
}

/**
 * Setea la función a llamar cuando se termine de cargar la página.
 * @param callback La función a setear.
 */
void WebViewer::setCallbackLoadFinished( const Callback &callback ){
	_callbackFinished = callback;
}

void WebViewer::runCallbackLoadFinished(){
	if (_callbackFinished){
		_callbackFinished();
	}
}
}

