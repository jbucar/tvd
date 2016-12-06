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
#include <gtk/gtk.h>
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#else
#include <gdk/gdkwin32.h>
#endif

namespace canvas {
namespace gtk {

namespace impl {
	System *getSystem();
}

VideoOverlay::VideoOverlay( Window *win, int zIndex )
	: canvas::VideoOverlay( zIndex )
{
	_main = win;
	_window = NULL;
}

VideoOverlay::~VideoOverlay( void )
{
	DTV_ASSERT(!_window);
}

bool VideoOverlay::create() {
	DTV_ASSERT(_main);

	LDEBUG("gtk::VideoOverlay", "Create"); 

	//	Create top level window
	_window = gtk_drawing_area_new();

	gtk_widget_set_events( GTK_WIDGET(_window),  GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK );

	{	//	Setup background color to black
		GdkColor color;
		gdk_color_parse ("black", &color);
		gtk_widget_modify_bg (_window, GTK_STATE_NORMAL, &color);
	}

	const Rect &rect = windowBounds();
	gtk_widget_set_size_request( _window, rect.w, rect.h );

	gtk_fixed_put( GTK_FIXED(_main->layout()), _window, rect.x, rect.y );
	gtk_widget_realize(_window);
	gdk_window_set_composited( gtk_widget_get_window(_window), TRUE );

	return true;
}

void VideoOverlay::destroy() {
	LDEBUG("gtk::VideoOverlay", "Destroy");
	DTV_ASSERT(_window);

	gtk_widget_destroy(_window);
	_window = NULL;
}

void VideoOverlay::show() {
	DTV_ASSERT(_window);
	gtk_widget_show(_window);
}

void VideoOverlay::hide() {
	DTV_ASSERT(_window);
	gtk_widget_hide(_window);
}

bool VideoOverlay::moveImpl( const Point &point ) {
	DTV_ASSERT(_window);
	gtk_fixed_move( GTK_FIXED(_main->layout()), _window, point.x, point.y );
	return true;
}

bool VideoOverlay::resizeImpl( const Size &size ) {
	DTV_ASSERT(_window);
	gtk_widget_set_size_request(_window, size.w, size.h);
	return true;
}

bool VideoOverlay::getDescription( VideoDescription &desc ) {
	DTV_ASSERT(_window);
#ifdef _WIN32
	desc = GDK_WINDOW_HWND(gtk_widget_get_window(_window));
#else
	desc.display = GDK_WINDOW_XDISPLAY(gtk_widget_get_window(_window));
	desc.screenID = GDK_SCREEN_XNUMBER(gtk_widget_get_screen(GTK_WIDGET(_window)));
	desc.winID = GDK_WINDOW_XID(gtk_widget_get_window(_window));
#endif
	return true;
}

canvas::Window *VideoOverlay::win() const {
	return _main;
}

void VideoOverlay::draw( const Rect &rect ) {
	GdkWindow *winCanvas = gtk_widget_get_window(_main->widget());
	GdkWindow *winVideo = gtk_widget_get_window(_window);

	cairo_t *cr = gdk_cairo_create( winCanvas );

#if GTK_MAJOR_VERSION >= 3
	gint wx, wy;
	gtk_widget_translate_coordinates(_window, gtk_widget_get_toplevel(_window), 0, 0, &wx, &wy);
	gdk_cairo_set_source_window(
		cr,
		winVideo,
		wx,
		wy
	);
#else
	gdk_cairo_set_source_pixmap(
		cr,
		winVideo,
		_window->allocation.x,
		_window->allocation.y
	);	
#endif

	cairo_rectangle( cr, rect.x, rect.y, rect.w, rect.h );
	cairo_clip( cr );
	cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE );
	cairo_paint(cr);

	cairo_destroy (cr);
}

}
}

