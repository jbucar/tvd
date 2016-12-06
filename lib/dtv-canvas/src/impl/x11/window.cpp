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

#define XCOMPOSITE_MIN_VERSION 402

#include "window.h"
#include "render.h"
#include "videooverlay.h"
#include "../../surface.h"
#include "../../canvas.h"
#include "generated/config.h"
#if CANVAS_X11_RENDER_USE_VDPAU
#include "vdpau/render.h"
#endif
#if CANVAS_X11_RENDER_USE_VAAPI
#include "vaapi/render.h"
#endif
#if CANVAS_X11_RENDER_USE_CAIRO
#	include "cairo/render.h"
#endif
#if CANVAS_X11_RENDER_USE_SKIA
#	include "skia/render.h"
#endif
#if CANVAS_X11_RENDER_USE_GL
#	include "gl/render.h"
#endif
#include <util/cfg/configregistrator.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/assert.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>


namespace canvas {
namespace x11 {

REGISTER_INIT_CONFIG( gui_window_x11 ) {
	root().addNode( "x11" )
#if CANVAS_X11_RENDER_USE_VDPAU
	.addValue( "use", "Render to instance", std::string("vdpau") );
#elif CANVAS_X11_RENDER_USE_VAAPI
	.addValue( "use", "Render to instance", std::string("vaapi") );
#elif CANVAS_X11_RENDER_USE_CAIRO
	.addValue( "use", "Render to instance", std::string("cairo") );
#elif CANVAS_X11_RENDER_USE_SKIA
	.addValue( "use", "Render to instance", std::string("skia") );
#elif CANVAS_X11_RENDER_USE_GL
	.addValue( "use", "Render to instance", std::string("gl") );
#else
#error You need a X11 render
#endif
}

namespace impl {
static int ctxErrorHandler( Display * /*dpy*/, XErrorEvent *ev ) {
	int msgSize = 1024;
	char msg[msgSize];
	XGetErrorText( ev->display, ev->error_code, msg, msgSize );
	LERROR( "x11::Window", "A X11 error ocurred: code=%d, msg:%s", ev->error_code, msg );
	DTV_ASSERT( false );
	return 0;
}
}

Window::Window( Display *display )
{
	_display = display;
	_window = None;
	_render = NULL;
	_xDamageEventBase = -1;
	_useVideoOverlay = false;
	_usePixmap = false;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	LINFO("x11::Window", "Initialize");

	XSetErrorHandler( &impl::ctxErrorHandler );

	//	Create render
	_render = createRender();
	if (!_render) {
		LERROR("x11::Window", "cannot create x11 render");
		return false;
	}

	//	Create window or use embedded
	_window = winID();
	if (!_window) {
		::Window parent = XDefaultRootWindow( _display );
		int scr = DefaultScreen( _display );

		XVisualInfo *vinfo = (XVisualInfo*) _render->visualInfo( _display );
		if (vinfo) {
			XSetWindowAttributes attrs;
			attrs.colormap = XCreateColormap( _display, parent, vinfo->visual, AllocNone );
			attrs.background_pixel = BlackPixel(_display, scr);
			attrs.border_pixel = WhitePixel(_display, scr);
			_window = XCreateWindow(
				_display, parent,
				0, 0, 100, 100, 0,
				vinfo->depth, InputOutput, vinfo->visual,
				CWBackPixel | CWColormap | CWBorderPixel,
				&attrs );
		} else {
			_window = XCreateSimpleWindow(
				_display, parent,
				0, 0, 100, 100, 0,
				WhitePixel(_display, scr), BlackPixel(_display, scr)
			);
		}

		if (!_window) {
			LERROR("x11::Window", "Failure in window creation");
			return false;
		}
	}

	//	Setup
	XStoreName( _display, _window, title().c_str() );

	// Set the name that x11 display in toolbars, menues, etc.
	size_t len = title().length() + 1; // Count '\0'
	char wmClassNames[len*2];
	strcpy(&wmClassNames[0], title().c_str());
	strcpy(&wmClassNames[len], title().c_str());
	Atom wm_class = XInternAtom(_display, "WM_CLASS", True);
	XChangeProperty(_display, _window, wm_class, XA_STRING, 8, PropModeReplace, (unsigned char*)wmClassNames, len*2);

	XSelectInput( _display, _window,
		ExposureMask
		|KeyPressMask
		|KeyReleaseMask
		|ButtonPressMask
		|ButtonReleaseMask
		|EnterWindowMask
		|StructureNotifyMask
		|SubstructureNotifyMask
	);

	//	Close window event must be informed
	Atom wm_delete = XInternAtom(_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(_display, _window, &wm_delete, 1);

	//	Map window
	XMapWindow( _display, _window );

	//	Init size
	initSize();

	//	Commit changes
	XSync(_display, False);

	//	Initialize render
	if (!_render->initialize()) {
		LERROR("x11::Window", "cannot initialize x11 render");
		return false;
	}

	//	Setup window
	_useVideoOverlay = _render->supportVideoOverlay();

	return true;
}

void Window::fin() {
	_render->finalize();
	DEL(_render);
	XDestroyWindow(_display,_window);

	XSetErrorHandler( NULL );
}

//	Aux
::Display *Window::dpy() const {
	return _display;
}

::Window Window::win() const {
	return _window;
}

Render *Window::createRender() {
	const std::string &use = util::cfg::getValue<std::string>("gui.window.x11.use");
	LINFO("canvas::Window::x11", "Use render: %s", use.c_str() );

#if CANVAS_X11_RENDER_USE_VDPAU
	if (use == "vdpau") {
		return new vdpau::Render( this );
	}
#endif
#if CANVAS_X11_RENDER_USE_CAIRO
	if (use == "cairo") {
		_usePixmap = true;
		return new cairo::Render( this );
	}
#endif
#if CANVAS_X11_RENDER_USE_SKIA
	if (use == "skia") {
		_usePixmap = true;
		return new skia::Render( this );
	}
#endif
#if CANVAS_X11_RENDER_USE_VAAPI
	if (use == "vaapi") {
		return new vaapi::Render( this );
	}
#endif
#if CANVAS_X11_RENDER_USE_GL
	if (use == "gl") {
		_usePixmap = true;
		return new gl::Render( this );
	}
#endif
	return NULL;
}

//	Embedded
bool Window::supportEmbedded() const {
	return true;
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	bool result = _render->initLayer( canvas );

	// Set application icon
	if (!icon().empty()){
		canvas::Surface *iconSurface = canvas->createSurfaceFromPath(icon().c_str());
		if (iconSurface) {
			util::DWORD *tmp = iconSurface->pixels();	//	TODO: Pass to BYTE *
			if (tmp) {
				//	Get image size
				canvas::Size size = iconSurface->getSize();
				int items = size.w * size.h + 2;

				//	Copy image size
				unsigned long buffer[items];
				buffer[0] = size.w;
				buffer[1] = size.h;

				//	Copy image data to unsigned long data
				unsigned long *ptr = buffer+2;
				for (int i=0; i<items-2; i++) {
					ptr[i] = tmp[i];
				}
				free( tmp );

				//	Replace property
				Atom net_wm_icon = XInternAtom(_display, "_NET_WM_ICON", True);
				Atom cardinal = XInternAtom(_display, "CARDINAL", True);
				XChangeProperty(
					_display,
					_window,
					net_wm_icon,
					cardinal,
					32,
					PropModeReplace,
					(const unsigned char *)buffer,
					items
				);
			} else {
				LWARN("x11::Window", "Cannot get icon pixels");
			}

			canvas->destroy(iconSurface);
		} else {
			LWARN("x11::Window", "Cannot create icon surface");
		}
	}
	return result;
}

void Window::finLayer( Canvas *canvas ) {
	_render->finLayer( canvas );
}

Surface *Window::lockLayer() {
	return _render->lockLayer();
}

void Window::renderLayerImpl( Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	Pixmap pixmap;
	GC gc;
	Size wSize = size();

	if (_usePixmap) {

		// Create and clear the pixmap
		XWindowAttributes xwa;
		XGetWindowAttributes( _display, _window, &xwa );
		pixmap = XCreatePixmap( _display, _window, wSize.w, wSize.h, xwa.depth );

		XGCValues v;
		v.graphics_exposures = False;
		v.subwindow_mode = IncludeInferiors;
		gc = XCreateGC( _display, pixmap, GCSubwindowMode | GCGraphicsExposures , &v );
		XFillRectangle( _display, pixmap, gc, 0, 0, wSize.w, wSize.h );
	}

	// Draw video(s) into pixmap
	if (_useVideoOverlay) {
		BOOST_FOREACH( canvas::VideoOverlay *vo, overlays() ) {
			dynamic_cast<VideoOverlay*>(vo)->draw(pixmap);
		}
	}

	// Draw canvas into pixmap
	_render->blit( surface, dirtyRegions, pixmap );

	if (_usePixmap) {
		// Copy pixmap into window
		XCopyArea( _display, pixmap, _window, gc, 0, 0, wSize.w, wSize.h,  0, 0 );
		XFreeGC( _display, gc );
		XFreePixmap( _display, pixmap );
	}

	XFlush( _display );
}

//	Overlays
bool Window::supportVideoOverlay() const {
	return _render->supportVideoOverlay();
}

canvas::VideoOverlay *Window::createOverlayInstance( int zIndex ) const {
	if (_useVideoOverlay) {
		return new VideoOverlay( const_cast<Window*>(this), zIndex );
	}
	return NULL;
}

//	Video methods
int Window::getFormat( char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines ) {
	return _render->getFormat( chroma, width, height, pitches, lines );
}

void Window::cleanup() {
	_render->cleanup();
}

const Rect &Window::getVideoBounds() const {
	return videoBounds();
}

void *Window::allocFrame( void **pixels ) {
	return _render->allocFrame( pixels );
}

void Window::freeFrame( void *frame ) {
	_render->freeFrame( frame );
}

void Window::renderFrame( void *frame ) {
	_render->renderFrame( frame );
}

//	Size methods
Size Window::screenSize() const {
	int screen = DefaultScreen(_display);
	return Size( DisplayWidth(_display, screen), DisplayHeight(_display, screen) );
}

bool Window::supportFullScreen() const {
	return true;
}

void Window::fullScreenImpl( bool enable ) {
	XEvent xev;

	{// Hack for gnome: remove
		const Size &s = screenSize();
		sizeHints(s,s);
	}

	LDEBUG( "x11::Window", "Set fullScreen: enable=%d", enable );
	Atom wm_state = XInternAtom( _display, "_NET_WM_STATE", False );
	Atom wm_fullscreen = XInternAtom( _display, "_NET_WM_STATE_FULLSCREEN", False );
	memset( &xev, 0, sizeof(xev) );
	xev.type = ClientMessage;
	xev.xclient.window = _window;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = enable ? 1 : 0;
	xev.xclient.data.l[1] = wm_fullscreen;
	xev.xclient.data.l[2] = 0;
	XSendEvent( _display, DefaultRootWindow(_display), False, SubstructureNotifyMask, &xev );
}

void Window::iconifyImpl( bool enable ) {
	LDEBUG( "x11::Window", "Iconify: enable=%d", enable );
	if (enable) {
		XIconifyWindow(_display, _window, DefaultScreen(_display));
	}
}

void Window::setSizeImpl( const Size &size ) {
	LDEBUG( "x11::Window", "Set: size=(%d,%d)", size.w, size.h );
	sizeHints( size, size );

	if (_render) {
		_render->restartRender();
	}
}

void Window::sizeHints( const Size &min, const Size &max ) {
	XSizeHints *xsh;
	xsh = XAllocSizeHints();

	xsh->flags = PMinSize | PMaxSize;
	xsh->min_height = min.h;
	xsh->min_width  = min.w;
	xsh->max_height = max.h;
	xsh->max_width  = max.w;

	XSetWMNormalHints(_display, _window, xsh);

	XFree(xsh);
	XSync(_display, False);
}

Size Window::canvasSize() {
	return getCanvasSize();
}

void Window::onEvent( XEvent ev ) {
	if (ev.type == MapNotify) {
		redraw();
	}
	else if (ev.type == EnterNotify) {
		if (ev.xcrossing.same_screen) {
			iconify(false);
		}
	}
	else if (ev.type == Expose) {
		redraw( Rect( ev.xexpose.x, ev.xexpose.y, ev.xexpose.width, ev.xexpose.height ) );
	}
	else if (_xDamageEventBase > -1 && ev.type == (_xDamageEventBase + XDamageNotify)) {
		XDamageNotifyEvent* dev = (XDamageNotifyEvent*) &ev;
		XDamageSubtract( _display, dev->damage, None, None );
		redraw( Rect( dev->area.x, dev->area.y, dev->area.width, dev->area.height ) );
	}
}

bool Window::initXCompositeExtension() {
	int event, error, major, minor;
	if (!XCompositeQueryExtension(_display, &event, &error)) {
		LWARN("x11::Window", "XComposite extension not available");
		return false;
	}
	if (!XCompositeQueryVersion(_display, &major, &minor)) {
		LWARN("x11::Window", "XComposite extension not available");
		return false;
	}
	int version = XCompositeVersion();
	if (version < XCOMPOSITE_MIN_VERSION) {
		LWARN("x11::Window", "XComposite version not suported");
		return false;
	}
	LDEBUG("x11::Window", "XComposite info: major=%d, minor=%d, event_base=%d error_base=%d version=%d", major, minor, event, error, version );
	return true;
}

bool Window::initXDamageExtension() {
	int error, major, minor;
	if (!XDamageQueryExtension(_display, &_xDamageEventBase, &error)) {
		LWARN("x11::Window", "XDamage extension not available");
		return false;
	}
	if (!XDamageQueryVersion(_display, &major, &minor)) {
		LWARN("x11::Window", "XDamage extension not available");
		return false;
	}
	if (major<1 || (major==1 && minor<1)) {
		LWARN("x11::Window", "XDamage version not suported");
		return false;
	}
	LDEBUG("x11::Window", "XDamage info: major=%d, minor=%d, event_base=%d error_bas=%d", major, minor, _xDamageEventBase, error );
	return true;
}

}
}
