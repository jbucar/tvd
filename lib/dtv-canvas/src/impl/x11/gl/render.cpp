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
#include "../../gl/surface.h"
#include "../../gl/canvas.h"
#include "../../../canvas.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <GL/glew.h>
#include <GL/glx.h>

namespace canvas {
namespace x11 {
namespace gl {

Render::Render( Window *win )
	: x11::Render( win )
{
	_display = NULL;
	_fbConfigs = NULL;
	_context = NULL;
	_glxWin = 0;
	_visualInfo = NULL;
	_surface = NULL;
	_blitingShaders = 0;
}

Render::~Render()
{
}

bool Render::init() {
	_display = display();
	_context = glXCreateNewContext( _display, _fbConfigs[0], GLX_RGBA_TYPE, NULL, True );
	return true;
}

void Render::fin() {
	glXDestroyContext( _display, _context );
	_display = NULL;
	XFree( _fbConfigs );
	XFree( _visualInfo );
	_visualInfo = NULL;
}

void *Render::visualInfo( Display *dpy ) {
	if (!_visualInfo) {
		static int attrs[] =  {
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT | GLX_PIXMAP_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			None
		};
		int scr = DefaultScreen( dpy );
		int nConfigs = 0;
		_fbConfigs = glXChooseFBConfig( dpy, scr, attrs, &nConfigs );
		if (!_fbConfigs) {
			LERROR("x11::gl::Render", "Fail to get framebuffer configs");
			return NULL;
		} else {
			LDEBUG("x11::gl::Render", "Get %d framebuffer configs. Using the first one", nConfigs);
		}
		_visualInfo = glXGetVisualFromFBConfig( dpy, _fbConfigs[0] );
		DTV_ASSERT( _visualInfo );
	}
	return _visualInfo;
}

bool Render::initLayer( Canvas *canvas ) {
	LDEBUG("x11::gl::Render", "initLayer()");

	//	Get canvas size
	const Size &size = canvas->size();

	//	Create window
	_glxWin = glXCreateWindow( _display, _fbConfigs[0], win(), NULL );

	//	Make current
	if (!glXMakeContextCurrent( _display, _glxWin, _glxWin, _context )) {
		LERROR("x11::gl::Render", "Unable to create glx context");
		return false;
	}

	//	Setup gl
	if (!canvas::gl::initGL()) {
		return false;
	}

	Rect rect( 0, 0, size.w, size.h );
	_surface = new canvas::gl::Surface( canvas, rect );
	if (!_surface) {
		glXMakeContextCurrent( _display, 0, 0, 0 );
		glXDestroyContext( _display, _context );
		LERROR("x11::gl::Render", "Cannot create canvas win surface");
		return false;
	}

	XSync( _display, True );

	return true;
}

void Render::finLayer( Canvas * /*canvas*/ ) {
	LDEBUG("x11::gl::Render", "Fin layer");
	glXMakeContextCurrent( _display, 0, 0, 0 );
	glXDestroyWindow( _display, _glxWin );
	DEL(_surface);
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
	UNUSED( surface );

	GLXPixmap glxPix = glXCreatePixmap( _display, _fbConfigs[0], dest, NULL );
	if (!glXMakeContextCurrent( _display, glxPix, glxPix, _context )) {
		LERROR("x11::gl::Render", "Unable to make current glx context for pixmap");
		return;
	}
	const Rect &bounds = window()->targetWindow();
	_surface->renderToFB( 0, bounds, size() );
	if (!glXMakeContextCurrent( _display, _glxWin, _glxWin, _context )) {
		LERROR("x11::gl::Render", "Unable to make current glx context for window");
		return;
	}
	glXDestroyPixmap( _display, glxPix );
}


}
}
}
