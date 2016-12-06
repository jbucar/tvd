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

#include "videooverlay.h"
#include "window.h"
#include "../../color.h"
#include "../../surface.h"
#include <util/assert.h>
#include <util/log.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

namespace canvas {
namespace x11 {

namespace impl {
	System *getSystem();
}

VideoOverlay::VideoOverlay( Window *win, int zIndex )
	: canvas::VideoOverlay( zIndex )
{
	_main = win;
}

VideoOverlay::~VideoOverlay( void )
{
}

bool VideoOverlay::create() {
	DTV_ASSERT(_main);

	LDEBUG("x11::VideoOverlay", "Create"); 

	const Rect &rect = windowBounds();
	_display = _main->dpy();
	int screen = DefaultScreen(_display);
	_video = XCreateSimpleWindow( 
		_display,
		_main->win(),
		rect.x, rect.y, rect.w, rect.h,
		0,
		WhitePixel(_display, screen),
		BlackPixel(_display, screen));

	XSelectInput( _display, _video, 
		ExposureMask
		|KeyPressMask
		|KeyReleaseMask
		|ButtonPressMask
		|ButtonReleaseMask
		|EnterWindowMask
		|StructureNotifyMask
		|SubstructureNotifyMask
	);

	XCompositeRedirectWindow( _display, _video, CompositeRedirectManual );
	_damage = XDamageCreate( _display, _video, XDamageReportRawRectangles );

	XMapWindow( _display, _video );
	XSync( _display, False );

	return createImpl( rect );
}

bool VideoOverlay::createImpl( const Rect & /*rect*/ ) {
	return true;
}

void VideoOverlay::destroy() {
	LDEBUG("x11::VideoOverlay", "Destroy");

	XDamageDestroy( _display, _damage );
	XSync( _display, True );

	destroyImpl();

	XDestroyWindow( _main->dpy(), _video );
}

void VideoOverlay::destroyImpl() {
}

void VideoOverlay::show() {
	XMapWindow( _display, _video );
}

void VideoOverlay::hide() {
	XUnmapWindow( _display, _video );
}

bool VideoOverlay::moveImpl( const Point &point ) {
	LDEBUG( "x11::VideoOverlay", "move(%d,%d)", point.x, point.y );
	XMoveWindow( _display, _video, point.x, point.y );
	return true;
}

bool VideoOverlay::resizeImpl( const Size &size ) {
	LDEBUG( "x11::VideoOverlay", "resize(%d,%d)", size.w, size.h );
	XResizeWindow( _display, _video, size.w, size.h );
	return true;
}

bool VideoOverlay::moveResizeImpl( const Rect &rect ) {
	LDEBUG( "x11::VideoOverlay", "moveResize(%d,%d,%d,%d)", rect.x, rect.y, rect.w, rect.h );
	XMoveResizeWindow( _display, _video, rect.x, rect.y, rect.w, rect.h );
	return true;
}

void VideoOverlay::draw( Pixmap dest ) {
	XWindowAttributes xwa;
	XGetWindowAttributes( dpy(), video(), &xwa );
	XGCValues v;
	v.graphics_exposures = False;
	v.subwindow_mode = IncludeInferiors;
	GC gc = XCreateGC( dpy(), video(), GCSubwindowMode | GCGraphicsExposures, &v );
	XCopyArea( dpy(), video(), dest, gc, 0, 0, xwa.width, xwa.height,  xwa.x, xwa.y );
	XFreeGC( dpy(), gc );
}

bool VideoOverlay::getDescription( VideoDescription &desc ) {
#ifdef _WIN32
	desc = _video;
#else
	desc.display = _display;
	desc.screenID = DefaultScreen(_display);
	desc.winID = _video;
#endif
	return true;
}

canvas::Window *VideoOverlay::win() const {
	return _main;
}

Display *VideoOverlay::dpy() const {
	return _display;
}

::Window VideoOverlay::video() const {
	return _video;
}

}
}

