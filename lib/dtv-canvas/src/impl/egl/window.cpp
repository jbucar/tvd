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
#include "../gl/surface.h"
#include "../gl/canvas.h"
#include <util/cfg/cfg.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <GLES2/gl2.h>

namespace canvas {
namespace egl {

Window::Window()
{
	_display = NULL;
	_config = NULL;
	_context = NULL;
	_window = NULL;
	_surface = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	EGLint majorVersion;
	EGLint minorVersion;

	_display = eglGetDisplay( nativeDisplay() );
	if (_display==EGL_NO_DISPLAY) {
		LERROR("egl", "Unnable to get EGLDisplay");
		return false;
	}

	if (eglInitialize( _display, &majorVersion, &minorVersion) == EGL_FALSE) {
		LERROR("egl", "Unnable to initialize EGLDisplay");
		return false;
	}

	if (eglBindAPI( EGL_OPENGL_ES_API ) == EGL_FALSE) {
		LERROR("egl", "Unnable to bind with EGL_OPENGL_ES_API");
		if (eglTerminate( _display ) == EGL_FALSE) {
			LERROR("egl", "Fail to terminate EGLDisplay");
		}
		return false;
	}

	{	//	Create context from config
		int numConfigs;
		EGLint confAttribList[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_NONE
		};
		if (eglChooseConfig( _display, confAttribList, &_config, 1, &numConfigs ) == EGL_FALSE) {
			LERROR("egl", "Unable to choose EGLConfig");
			return false;
		}

		const EGLint attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
		_context = eglCreateContext( _display, _config, EGL_NO_CONTEXT, attribs );
		if (_context==EGL_NO_CONTEXT) {
			LERROR("egl", "Unnable to create EGL Context");
			return false;
		}
	}

	//	Force full screen
	util::cfg::setValue("gui.window.fullscreen", true);

	return canvas::Window::init();
}

void Window::fin() {
	if (eglDestroyContext( _display, _context ) == EGL_FALSE) {
		LERROR("egl", "Fail to destroy egl context");
	}

	if (eglTerminate( _display ) == EGL_FALSE) {
		LERROR("egl", "Fail to terminate EGLDisplay");
	}
}

//	Getters
bool Window::supportFullScreen() const {
	return true;
}

bool Window::supportIconify() const {
	return false;
}

//	Layer methods
bool Window::initLayer( Canvas *canvas ) {
	LDEBUG("egl", "initLayer");

	//	Create window
	EGLint attribList[] = { EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE };
	_window = eglCreateWindowSurface( _display, _config, nativeWindow(), attribList );
	if (_window==EGL_NO_SURFACE) {
		LERROR("egl", "Unable to create window surface");
		return false;
	}

	//	Make context current
	eglMakeCurrent( _display, _window, _window, _context );

	{	//	Get window size
		EGLint w, h;
		eglQuerySurface( _display, _window, EGL_WIDTH, &w);
		eglQuerySurface( _display, _window, EGL_HEIGHT, &h);
		LDEBUG( "egl", "Using surface size: size=(%d,%d)", w, h );
	}

	//	Setup gl
	if (!canvas::gl::initGL()) {
		return false;
	}

	{	//	Create layer
		const Size &size = canvas->size();
		_surface = new canvas::gl::Surface( canvas, Rect(0,0,size.w,size.h) );
		if (!_surface) {
			LERROR("egl", "Cannot create canvas surface");
			return false;
		}
	}

	return true;
}

void Window::finLayer( Canvas * /*canvas*/ ) {
	LDEBUG("egl", "finLayer");

	eglMakeCurrent( _display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

	if (eglDestroySurface( _display, _window ) == EGL_FALSE) {
		LERROR("egl", "Fail to destroy egl window surface");
	}

	DEL(_surface);
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT( _surface );
	return _surface;
}

void Window::renderLayerImpl( canvas::Surface *surface, const std::vector<Rect> & /*dirtyRegions*/ ) {
	LDEBUG("egl", "Blit");
	DTV_ASSERT( surface == _surface );
	UNUSED( surface );

	eglMakeCurrent( _display, _window, _window, _context );

	EGLint w, h;
	eglQuerySurface( _display, _window, EGL_WIDTH, &w);
	eglQuerySurface( _display, _window, EGL_HEIGHT, &h);

	Size size(w, h);
	_surface->renderToFB( 0, targetWindow(), size, true );

	if (eglSwapBuffers(_display, _window) == EGL_FALSE) {
		LWARN("egl", "Unable to swap buffers. Error code: %d", eglGetError());
	}
}

//	Getters
EGLNativeDisplayType Window::nativeDisplay() const {
	return EGL_DEFAULT_DISPLAY;
}

}
}
