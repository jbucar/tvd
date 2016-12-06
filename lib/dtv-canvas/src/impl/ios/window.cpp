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
#include "../../canvas.h"
#include "../skia/surface.h"
#include "../skia/canvas.h"
#include <util/assert.h>
#include <util/mcr.h>
#include <util/log.h>
#include <util/ios/ios.h>

namespace canvas {
namespace ios {

Window::Window()
{
	_surface = NULL;
}

Window::~Window()
{
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	DTV_ASSERT(!_surface);
	const Size &s = canvas->size();

	_pixels = (util::DWORD*) malloc( s.w * s.h * sizeof(util::DWORD));

	int stride = s.w * sizeof(util::DWORD);
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = canvas->size();
	img.length = stride*s.h;
	img.data = (unsigned char*) _pixels;
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = stride;
	img.dataOffset = 0;
	_surface = new canvas::skia::Surface( static_cast<canvas::skia::Canvas*>(canvas), &img );

	util::ios::initLayer( _pixels, s.w, s.h );

	return true;
}

void Window::finLayer( canvas::Canvas * /*canvas*/ ) {
	DEL(_surface);
	free( _pixels );
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Window::renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> & /*dirtyRegions*/ ) {
	Size s = surface->getSize();
	util::ios::renderLayer();
}

//	Size methods
bool Window::supportFullScreen() const {
	return true;
}

bool Window::supportIconify() const {
	return false;
}

void Window::fullscreenImpl( bool /*enable*/ ) {
}

void Window::iconifyImpl( bool /*enable*/ ) {
}

Size Window::screenSize() const {
	int w,h;
	util::ios::screenSize( w, h );
	return Size(w,h);
}

}
}
