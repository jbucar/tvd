/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-zapper implementation.

    DTV-zapper is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-zapper is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-zapper.

    DTV-zapper es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-zapper se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "surface.h"
#include "../canvas.h"
#include "../surface.h"
#include <util/assert.h>

namespace canvas {
namespace layer {

Surface::Surface( Canvas *canvas, int id, int zIndex )
	: Layer( id, zIndex ), _canvas(canvas)
{
	_surface = NULL;
}

Surface::~Surface()
{
	DTV_ASSERT(!_surface);
}

bool Surface::initialize() {
	const Size &size=_canvas->size();
	Rect rect(0,0,size.w,size.h);
	_surface = _canvas->createSurface( rect );
	DTV_ASSERT(_surface);
	_surface->autoFlush( true );
	_surface->setColor( Color() ); // Black, fully transparent
	_surface->clear();
	_surface->setZIndex( zIndex() );
	return true;
}

void Surface::finalize() {
	_canvas->destroy( _surface );
}

void Surface::setEnable( bool state ) {
	_surface->setVisible( state );
}

void Surface::clear() {
	_surface->clear();
}

void Surface::flush() {
	_surface->flush();
}

bool Surface::setAlpha( util::BYTE alpha ) {
	return _surface->setOpacity( alpha );
}

canvas::Surface *Surface::getSurface() {
	return _surface;
}

}
}

