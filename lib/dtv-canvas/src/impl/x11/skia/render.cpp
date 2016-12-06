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

#include "render.h"
#include "../window.h"
#include "../../../surface.h"
#include "../../../canvas.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <SkBitmap.h>
#include <SkCanvas.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace canvas {
namespace x11 {
namespace skia {

Render::Render( Window *win )
	: x11::Render( win )
{
	_surface = NULL;
}
	
Render::~Render()
{
}

bool Render::initLayer( Canvas *canvas ) {
	//	Get canvas size
	const Size &size = canvas->size();

	_bitmap = new SkBitmap();
	_bitmap->setConfig(SkBitmap::kARGB_8888_Config, size.w, size.h );
	_bitmap->allocPixels();

	int height = size.h;
	int stride = size.w*4;

	//	Setup data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = canvas->size();
	img.length = stride*height;	
	img.data = (unsigned char *)_bitmap->getPixels();
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = stride;
	img.dataOffset = 0;

	//	Create surface for layer
	_surface = canvas->createSurface( &img );
	if (!_surface) {
		LERROR("x11::skia::Render", "Cannot create canvas surface");
		return false;
	}

	return true;
}

void Render::finLayer( Canvas * /*canvas*/ ) {
	DEL(_surface);
	DEL(_bitmap);
}

Surface *Render::lockLayer() {
	DTV_ASSERT( _surface );
	return _surface;
}

bool Render::supportVideoOverlay() const {
	return true;
}

void Render::blit( Surface *surface, const std::vector<Rect> & /*dirtyRegions*/, Pixmap dest ) {
	DTV_ASSERT( surface == _surface );
	UNUSED(surface);

	// Draw the bitmap to the screen.
	XWindowAttributes xwa;
	XGetWindowAttributes( display(), win(), &xwa );
	XImage *video = XGetImage( display(), dest, 0, 0, xwa.width, xwa.height , AllPlanes, ZPixmap );
	SkBitmap *videoBmp = new SkBitmap();
	videoBmp->setConfig(SkBitmap::kARGB_8888_Config, xwa.width, xwa.height );
	videoBmp->setPixels(video->data);
	SkPaint p;
	p.setFilterLevel( SkPaint::kHigh_FilterLevel );
	p.setXfermodeMode( SkXfermode::kSrcOver_Mode );
	SkCanvas canvas(*videoBmp);

	SkRect srcRect = SkRect::MakeXYWH( 0, 0, (SkScalar)_bitmap->width(), (SkScalar)_bitmap->height() );
	const Rect &scaledBnds = window()->targetWindow();
	SkRect dstRect = SkRect::MakeXYWH( (SkScalar)scaledBnds.x, (SkScalar)scaledBnds.y, (SkScalar)scaledBnds.w, (SkScalar)scaledBnds.h );
	canvas.drawBitmapRectToRect( *_bitmap, &srcRect, dstRect, &p );

	XGCValues v;
	v.graphics_exposures = false;
	GC gc = XCreateGC( display(), dest, GCGraphicsExposures, &v );
	XPutImage( display(), dest, gc, video, 0, 0, 0, 0, video->width, video->height );

	XFreeGC( display(), gc );
	DEL( videoBmp );
	XDestroyImage( video );
}

}
}
}

