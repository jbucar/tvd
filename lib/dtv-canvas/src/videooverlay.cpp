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
#include "rect.h"
#include "point.h"
#include <util/log.h>

namespace canvas {

/**
 * Constructor base. Luego de crear un instancia se debe llamar al método initialize().
 * @param zIndex El valor de zIndex del @c VideoOverlay con respecto a otros @c VideoOverlay.
 */
VideoOverlay::VideoOverlay( int zIndex )
	: _zIndex(zIndex)
{
	_scale = vos::always;
}

/**
 * Destructor base. Antes de destruir un instancia se debe llamar al método finalize().
 */
VideoOverlay::~VideoOverlay()
{
}

/**
 * Inicializa el estado de la instancia, creando una superficie para renderizado que ocupa la región indicada en @b rect.
 * @param rect La región de la superficie de video.
 * @return true si la inicialización finalizó correctamente, false en caso contrario.
 */
bool VideoOverlay::initialize( const Rect &rect ) {
	LDEBUG( "VideoOverlay", "Initialize: rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h );

	//	Setup
	_rect = rect;

	if(!win()) {
		LWARN( "VideoOverlay", "Cannot create video overlay without a window" );
		return false;
	}

	//	Create component
	if (!create()) {
		LWARN( "VideoOverlay", "Cannot create video overlay component" );
		return false;
	}

	//	Setup
	if (!setup()) {
		return false;
	}

	show();

	win()->restackOverlays();

	return true;
}

/**
 * Finaliza la instancia.
 */
void VideoOverlay::finalize() {
	LDEBUG( "VideoOverlay", "Finalize" );
	hide();
	destroy();
}

bool VideoOverlay::setup() {
	LDEBUG( "VideoOverlay", "Setup overlay" );

	if (!move()) {
		LWARN( "VideoOverlay", "Cannot move overlay" );
		return false;
	}

	if (!resize()) {
		LWARN( "VideoOverlay", "Cannot resize overlay" );
		return false;
	}

	return true;
}

/**
 * Mueve la superficie de renderizado al punto indicado por @b point.
 * @param point La nueva posición del @c VideoOverlay
 * @return true si pudo mover la superficie correctamente, false en caso contrario.
 */
bool VideoOverlay::move( const Point &point ) {
	LDEBUG( "VideoOverlay", "Move: point=(%d,%d)", point.x, point.y );
	_rect=point;
	return move();
}

bool VideoOverlay::move() {
	Point p(_rect);
	win()->translateToWindow( p.x, p.y, scale() );
	return moveImpl(p);
}

bool VideoOverlay::moveResize( const Rect &rect ) {
	LDEBUG( "VideoOverlay", "Move and resize: rect=(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h );
	_rect = rect;
	return moveResize();
}

bool VideoOverlay::moveResize() {
	Rect tmp(_rect);
	win()->translateToWindow(tmp.x, tmp.y, scale());
	win()->scaleToWindow(tmp.w, tmp.h, scale());
	return moveResizeImpl(tmp);
}

bool VideoOverlay::moveResizeImpl( const Rect &rect ) {
	bool res=true;
	res &= moveImpl( Point(rect) );
	res &= resizeImpl( Size(rect) );
	return res;
}

void VideoOverlay::scale( vos::type method ) {
	_scale = method;
}

bool VideoOverlay::scale() const {
	if (_scale == vos::always) {
		return true;
	}
	else if (_scale == vos::notFullScreen) {
		return isFullScreen() ? false : true;
	}
	else {
		return false;
	}
}

bool VideoOverlay::isFullScreen() const {
	return win()->getCanvasSize() == Size(_rect) && Point(_rect) == Point();
}

/**
 * Cambia el tamaño de la superficie de renderizado a @b size.
 * @param size El nuevo tamaño del @c VideoOverlay.
 * @return true si pudo actualizar el tamaño correctamente, false en caso contrario.
 */
bool VideoOverlay::resize( const Size &size ) {
	LDEBUG( "VideoOverlay", "Resize: size=(%d,%d)", size.w, size.h );
	_rect=size;
	return resize();
}

bool VideoOverlay::resize() {
	Size size(_rect);
	win()->scaleToWindow( size.w, size.h, scale() );
	return resizeImpl(size);
}

void VideoOverlay::iconify( bool enable ) {
	if (enable) {
		hide();
	} else {
		show();
	}
}

void VideoOverlay::updateBounds() {
	moveResize();
}

/**
 * @return La región que ocupa el @c VideoOverlay.
 */
Rect VideoOverlay::windowBounds() const {
	Rect tmp(_rect);
	win()->translateToWindow(tmp.x, tmp.y, scale());
	win()->scaleToWindow(tmp.w, tmp.h, scale());
	return tmp;
}

void VideoOverlay::zIndex( int z ) {
	_zIndex = z;
}

/**
 * @return El zIndex del @c VideoOverlay.
 */
int VideoOverlay::zIndex() const {
	return _zIndex;
}

/**
 * Actualiza @b desc con la descripción de la superficie de renderizado.
 * @param[out] desc Identificación de la ventana.
 * @return true si pudo obtener la descripción del @c VideoOverlay correctamente, false en caso contrario.
 */
bool VideoOverlay::getDescription( VideoDescription &desc ) {
#ifdef _WIN32
	desc = NULL;
#else
	desc.display = NULL;
	desc.screenID = -1;
	desc.winID = -1;
#endif
	return true;
}

}
