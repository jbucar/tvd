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

#include "surfacewrapper.h"
#include <canvas/surface.h>
#include <util/mcr.h>

namespace player {
namespace mcanvas {

SurfaceWrapper::SurfaceWrapper( canvas::Surface *surface, bool isPrimary )
	: _surface(surface)
{
	_isPrimary = isPrimary;
	_flipH = false;
	_flipV = false;
	_scaledSize = _surface->getSize();
	_rotation = 0;
}

SurfaceWrapper::~SurfaceWrapper()
{
	canvas::Surface::destroy( _surface );
}

canvas::Surface *SurfaceWrapper::surface() {
	return _surface;
}

void SurfaceWrapper::setFlip( bool horizontal, bool vertical ) {
	_flipH = horizontal;
	_flipV = vertical;
}

bool SurfaceWrapper::isFlippedH() const {
	return _flipH;
}

bool SurfaceWrapper::isFlippedV() const {
	return _flipV;
}

void SurfaceWrapper::setScaledSize( const canvas::Size &size ) {
	_scaledSize = size;
}

canvas::Size SurfaceWrapper::getScaledSize() const {
	return _scaledSize;
}

void SurfaceWrapper::replaceSurface( canvas::Surface *newSurface ) {
	canvas::Surface::destroy(_surface);
	_surface = newSurface;
	_scaledSize = _surface->getSize();
}

bool SurfaceWrapper::needScale() const {
	return _flipH || _flipV || (_surface->getSize() != _scaledSize);
}

void SurfaceWrapper::rotation( int degrees ) {
	_rotation = degrees;
}

int SurfaceWrapper::rotation() const {
	return _rotation;
}

bool SurfaceWrapper::isPrimary() const {
	return _isPrimary;
}

}
}
