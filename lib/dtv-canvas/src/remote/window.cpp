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
#include "server.h"
#include "memory.h"
#include "../canvas.h"
#include "../surface.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>

namespace canvas {
namespace remote {

Window::Window()
{
	_surface = NULL;
	_bitmapMem = NULL;
	_dirtyMem = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	initSize();

	_dirtyMem = Memory::createClient(CANVAS_SHARED_DIRTY);
	if (!_dirtyMem->initialize()) {
		LERROR( "remote::Window", "Cannot initialize dirty region memory" );
		DEL(_dirtyMem);
		return false;
	}

	_bitmapMem = Memory::createClient(CANVAS_SHARED_MEMORY);
	if (!_bitmapMem->initialize()) {
		LERROR( "remote::Window", "Cannot initialize bitmap memory" );
		DEL(_dirtyMem);
		DEL(_bitmapMem);
		return false;
	}

	return true;
}

void Window::finalize() {
	if (_bitmapMem) {
		_bitmapMem->finalize();
		DEL( _bitmapMem );
	}

	if (_dirtyMem) {
		_dirtyMem->finalize();
		DEL( _dirtyMem );
	}
}

//	Layer methods
bool Window::initLayer( Canvas *canvas ) {
	if (canvas->size() != size()) {
		LERROR("remote::Window", "canvas and window size differ");
		return false;
	}

	//	Setup data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.data = static_cast<unsigned char*>(_bitmapMem->lock());
	img.size = canvas->size();
	img.length = _bitmapMem->size();
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = img.size.w*4;
	img.dataOffset = 0;

	_bitmapMem->unlock();

	_surface = canvas->createSurface( &img );
	if (!_surface) {
		LERROR("remote::Window", "Cannot create a canvas surface");
		return false;
	}
	return true;
}

void Window::finLayer( Canvas * /*canvas*/ ) {
	DEL(_surface);
}

Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	//	Lock surface
	_bitmapMem->lock();
	return _surface;
}

void Window::renderLayerImpl( Surface *sur, const std::vector<Rect> &dirtyRegions ) {
	DTV_ASSERT(sur == _surface);
	UNUSED(sur);

	unsigned char *data = static_cast<unsigned char*>(_dirtyMem->lock());
	size_t len = _dirtyMem->size();

	util::DWORD size;
	memcpy(&size, data, sizeof(size));

	if ((dirtyRegions.size()+size+1)*sizeof(Rect) > len) {
		size = 1;

		//	Copy size
		memcpy(data, &size, sizeof(size));
		int pos=sizeof(Rect);

		//	Copy rect from all window
		const Size &s = this->size();
		Rect rect(0,0,s.w,s.h);
		memcpy(data+pos, &rect, sizeof(Rect) );
		//pos += sizeof(Rect);
	}
	else {
		int pos = (size+1) * sizeof(Rect);
		size += dirtyRegions.size();

		//	Copy size
		memcpy(data, &size, sizeof(size));

		//	Copy all dirty rects
		util::DWORD nRects = dirtyRegions.size();
		for (util::DWORD i=0; i<nRects; i++) {
			const Rect &rect=dirtyRegions[i];
			memcpy(data+pos, &rect, sizeof(Rect) );
			pos += sizeof(Rect);
		}
	}
	_dirtyMem->unlock();

	//	Signal remote process
	_bitmapMem->post();
}

void Window::unlockLayer( Surface * /*surface*/ ) {
	_bitmapMem->unlock();
}

}
}
