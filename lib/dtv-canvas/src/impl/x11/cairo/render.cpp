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
#include <X11/Xlib.h>
#include <cairo-xlib.h>

namespace canvas {
namespace x11 {
namespace cairo {

Render::Render( Window *win )
	: x11::Render( win )
{
	_surface = NULL;
	_sLayer = NULL;
}

Render::~Render()
{
}

bool Render::initLayer( Canvas *canvas ) {
	//	Get canvas size
	const Size &size = canvas->size();

	//	Create surface layer
	_sLayer = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, size.w, size.h );
	if (cairo_surface_status(_sLayer) != CAIRO_STATUS_SUCCESS) {
		LERROR("x11::Window", "cannot create surface");
		return false;
	}
	cairo_surface_flush (_sLayer);

	int height = cairo_image_surface_get_height (_sLayer);
	int stride = cairo_image_surface_get_stride (_sLayer);

	//	Setup data
	ImageData img;
	memset(&img,0,sizeof(ImageData));
	img.size = canvas->size();
	img.length = stride*height;
	img.data = cairo_image_surface_get_data (_sLayer);
	img.bitsPerPixel = 32;
	img.bytesPerPixel = 4;
	img.stride = stride;
	img.dataOffset = 0;

	//	Create surface for layer
	_surface = canvas->createSurface( &img );
	if (!_surface) {
		LERROR("x11::Window", "Cannot create canvas surface");
		return false;
	}

	return true;
}

void Render::finLayer( Canvas * /*canvas*/ ) {
	DEL(_surface);

	cairo_surface_destroy(_sLayer);
	_sLayer = NULL;
}

Surface *Render::lockLayer() {
	DTV_ASSERT( _surface );
	return _surface;
}

bool Render::supportVideoOverlay() const {
	return true;
}

void Render::blit( Surface *surface, const std::vector<Rect> &/*dirtyRegions*/, Pixmap dest ) {
	DTV_ASSERT( surface == _surface );
	UNUSED(surface);

	//	Create window surface
	const Size &s = size();
	cairo_surface_t *sWindow = cairo_xlib_surface_create( display(), dest, DefaultVisual(display(), 0), s.w, s.h );
	if (cairo_surface_status(sWindow) != CAIRO_STATUS_SUCCESS) {
		LERROR("x11::Window", "Cannot create cairo surface");
		return;
	}

	//	Create context
	cairo_t *cr = cairo_create( sWindow );
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy( sWindow );
		LERROR("x11::Window", "Cannot create cairo context");
		return;
	}

	cairo_surface_mark_dirty(_sLayer);

	const Rect &scaledBds = window()->targetWindow();
	Size canvSize = canvasSize();
	double fx = double(scaledBds.w) / double(canvSize.w);
	double fy = double(scaledBds.h) / double(canvSize.h);
	cairo_scale( cr, fx, fy );
	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_GOOD );

	cairo_surface_flush( _sLayer );
	cairo_set_source_surface(cr, _sLayer, scaledBds.x/fx, scaledBds.y/fy);
	cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
	cairo_paint(cr);

	cairo_surface_destroy( sWindow );
	cairo_destroy( cr );
}

}
}
}

