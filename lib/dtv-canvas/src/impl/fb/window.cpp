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
#include "../../surface.h"
#include "../../canvas.h"
#include "generated/config.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/cfg/cfg.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <stropts.h>
#include <unistd.h>

namespace canvas {
namespace fb {
	
Window::Window()
{	
	_fd = -1;
	_screensize = 0;
	_mem = NULL;
	_surface = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	LINFO("fb::Window", "Initialize");

	// open the frame buffer file for reading & writing
	_fd = open ( FB_DEVICE, O_RDWR );
	if (!_fd) {
		LERROR("fb::Window", "can't open framebuffer device");
		return false;
	}

	if (ioctl (_fd, FBIOGET_FSCREENINFO, &finfo)) {
		LERROR("fb::Window", "reading fixed information");
		finalize();
		return false;
	}

	if (ioctl (_fd, FBIOGET_VSCREENINFO, &vinfo)) {
		LERROR("fb::Window", "reading variable information");
		finalize();
		return false;
	}

	//	TODO: Set screen size	

	LINFO("fb::Window", "Frambuffer is %dx%d, %dbpp", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	//	Calculates size
	_screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	//	Map the device to memory 
	_mem = (unsigned char*)mmap (0, _screensize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
	if (_mem == (void *)-1) {
		_mem = NULL;
		LERROR("fb::Window", "failed to map framebuffer device to memory");
		finalize();
		return false;
	}

	return true;
}

void Window::finalize() {
	if (_mem) {
		munmap (_mem, _screensize);
		_mem = NULL;
	}

	if (_fd > 0) {
		close (_fd);
		_fd = -1;
	}
}

//	Layer methods
bool Window::initLayer( Canvas *canvas ) {
	if (canvas->size() != size()) {
		LERROR("fb::Window", "canvas and screen size differ");
		return false;
	}

	//	Setup data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = canvas->size();
	img.data = _mem;
	img.length = _screensize;
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = img.size.w*4;
	img.dataOffset = 0;

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
	return _surface;
}

void Window::renderLayerImpl( Surface */*surface*/, const std::vector<Rect> &/*dirtyRegions*/ ) {
}

void Window::iconifyImpl( bool /*enable*/ ) {
}

}
}

