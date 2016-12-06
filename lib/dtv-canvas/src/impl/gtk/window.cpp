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
#include "system.h"
#include "videooverlay.h"
#include "../cairo/canvas.h"
#include "../cairo/surface.h"
#include "../../inputmanager.h"
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <util/cfg/cfg.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions.hpp>
#include <gtk/gtk.h>

namespace canvas {
namespace gtk {

namespace impl {
	System *getSystem();
}

util::key::type getKeyCode( guint symbol );

#if GTK_MAJOR_VERSION >= 3
static gboolean on_window_draw( GtkWidget *da, cairo_t * /*cr*/, gpointer user_data ) {
	int width = gtk_widget_get_allocated_width(da);
	int height = gtk_widget_get_allocated_height(da);
	Rect rect( 0, 0, width, height );

	Window *win = (Window*) user_data;
	DTV_ASSERT(win);
	win->draw( da, rect );
	return TRUE;
}
#else
static gboolean on_window_expose_event( GtkWidget *da, GdkEventExpose *event, gpointer user_data ) {
	Rect rect( event->area.x, event->area.y, event->area.width, event->area.height );
	Window *win = (Window*) user_data;
	DTV_ASSERT(win);
	win->draw( da, rect );
	return TRUE;
}
#endif

static gboolean key_event_cb( GtkWidget * /*widget*/, GdkEventKey *kevent, gpointer /*user_data*/ ) {
	impl::getSystem()->input()->dispatchKey( getKeyCode(kevent->keyval), kevent->type == GDK_KEY_RELEASE );
	return TRUE;
}

static gboolean button_event_cb( GtkWidget * /*widget*/, GdkEventButton *kevent, gpointer /*user_data*/ ) {
	if (kevent->button <= 5) {
		input::ButtonEvent evt;
		evt.button = kevent->button;
		evt.x = static_cast<int>(kevent->x);
		evt.y = static_cast<int>(kevent->y);
		evt.isPress = (kevent->type == GDK_BUTTON_PRESS);
		input::dispatchButtonEvent( impl::getSystem(), &evt );
	}
	return TRUE;
}

static gint on_delete_event( GtkWidget * /*widget*/, GdkEvent  * /*event*/, gpointer /*data*/ ) {
    /* If you return FALSE in the "delete_event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */
	impl::getSystem()->onWindowDestroyed();
	return (TRUE);
}

static gboolean on_state_event( GtkWidget * /*widget*/, GdkEventWindowState *event, gpointer user_data ) {
	Window *win = (Window *)user_data;
	DTV_ASSERT(win);
	if (event->changed_mask & GDK_WINDOW_STATE_ICONIFIED) {
		if (!(event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)) {
			win->iconify(false);
		}
		else {
			win->iconify(true);
		}
	}
	return FALSE;
}

static gboolean on_configure_event( GtkWindow * /*window*/, GdkEvent * /*event*/, gpointer user_data ) {
	Window *win = (Window *)user_data;
	DTV_ASSERT(win);
	win->redraw();
	return FALSE;
}

Window::Window()
{
	_surface = NULL;
	_window = NULL;
	_layout = NULL;
}

Window::~Window()
{
}

//	Initialization
bool Window::init() {
	LDEBUG("gtk::Window", "Initialize");

	//      Create main window
	_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	//      Create main window layout
	_layout = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(_window), _layout);

	DTV_ASSERT(_window);
	g_object_ref( _window );

	//	Setup main window
	gtk_widget_set_events( GTK_WIDGET(_window),  GDK_STRUCTURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_FOCUS_CHANGE );
	//gtk_widget_set_events( GTK_WIDGET(_window), GDK_ALL_EVENTS_MASK );
	gtk_window_set_title( GTK_WINDOW(_window), title().c_str() );
	if (!icon().empty()) {
		gtk_window_set_default_icon_from_file( icon().c_str(), NULL );
	}
	gtk_window_set_position(GTK_WINDOW(_window), GTK_WIN_POS_CENTER);
	gtk_widget_set_app_paintable(_window, TRUE);
	gtk_widget_set_double_buffered(_window, TRUE);

	{	//	Setup background color to black
		GdkColor color;
		gdk_color_parse ("black", &color);
		gtk_widget_modify_bg (_window, GTK_STATE_NORMAL, &color);
	}

	gtk_widget_realize(_window);

	//	Setup size
	initSize();

	//	Setup window signals
#if GTK_MAJOR_VERSION >= 3
	g_signal_connect( G_OBJECT(_window), "draw", G_CALLBACK(on_window_draw), this);
#else
	g_signal_connect( G_OBJECT(_window), "expose_event", G_CALLBACK(on_window_expose_event), this);
#endif
	g_signal_connect( GTK_WIDGET(_window), "delete_event", G_CALLBACK (on_delete_event), this);
	g_signal_connect( GTK_WIDGET(_window), "configure-event", G_CALLBACK(on_configure_event), this );
	g_signal_connect( GTK_WIDGET(_window), "window-state-event", G_CALLBACK(on_state_event), this );

	//	Input events
	g_signal_connect( GTK_WIDGET(_window), "key_press_event", G_CALLBACK(key_event_cb), this );
	g_signal_connect( GTK_WIDGET(_window), "key_release_event", G_CALLBACK(key_event_cb), this );
	g_signal_connect( GTK_WIDGET(_window), "button_press_event", G_CALLBACK(button_event_cb), this );
	g_signal_connect( GTK_WIDGET(_window), "button_release_event", G_CALLBACK(button_event_cb), this );


	//	Show window
	gtk_widget_show_all(_window);

	return true;
}

void Window::fin() {
	//	Remove window
	gtk_widget_destroy( _window );
	g_object_unref( _window );
}

//	Size methods
Size Window::screenSize() const {
	GdkScreen *screen = gtk_widget_get_screen( _window );
	return Size( gdk_screen_get_width(screen), gdk_screen_get_height(screen) );
}

void Window::setSizeImpl( const Size &s ) {
	GdkGeometry hints;
	hints.min_width = s.w;
	hints.min_height = s.h;
	hints.max_width = s.w;
	hints.max_height = s.h;

	GdkWindowHints mask = (GdkWindowHints) (GDK_HINT_MAX_SIZE | GDK_HINT_MIN_SIZE );
	gtk_window_set_geometry_hints( GTK_WINDOW (_window), _window, &hints, mask );
}

bool Window::supportFullScreen() const {
	return true;
}

void Window::fullScreenImpl( bool enable ) {
	DTV_ASSERT(_window);

	if (enable) {
		// Hack for gnome: remove geometry hints
		gdk_window_set_geometry_hints( gtk_widget_get_window(_window), NULL, (GdkWindowHints)0 );

		gtk_window_fullscreen( GTK_WINDOW(_window) );
	} else {
		gtk_window_unfullscreen( GTK_WINDOW(_window) );
	}
}

void Window::iconifyImpl( bool enable ) {
	if (enable) {
		gtk_window_iconify( GTK_WINDOW(_window) );
	} else {
		gtk_window_deiconify( GTK_WINDOW(_window) );
	}
}

//	Overlays
bool Window::supportVideoOverlay() const {
	GdkDisplay *display = gtk_widget_get_display( _window );
	return gdk_display_supports_composite( display ) != FALSE;
}

canvas::VideoOverlay *Window::createOverlayInstance( int zIndex ) const {
	return new VideoOverlay( (Window *)this, zIndex );
}

//	Layer methods
bool Window::initLayer( Canvas *canvas ) {
	const Size &s = canvas->size();
	_surface = new cairo::Surface( (cairo::Canvas *)canvas, Rect(0,0,s.w,s.h) );
	return true;
}

void Window::finLayer( Canvas * /*canvas*/ ) {
	DEL(_surface);
}

Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	return _surface;
}

void Window::renderLayerImpl( Surface *surface, const std::vector<Rect> &dirtyRegions ) {
	DTV_ASSERT( surface == _surface );
	UNUSED(surface);

	cairo_surface_t *s = _surface->getContent();
	const Size &canvasSize = getCanvasSize();
	const Rect &scaledBounds = targetWindow();

	BOOST_FOREACH( const Rect &r, dirtyRegions ) {
		//      Mark changed region
		cairo_surface_mark_dirty_rectangle( s, r.x, r.y, r.w, r.h );
		//      Mark exposed area
		gtk_widget_queue_draw_area(
			_window,
			boost::math::iround(float(r.x * scaledBounds.w) / float(r.w)),
			boost::math::iround(float(r.y * scaledBounds.h) / float(r.h)),

			boost::math::iround(float(r.w) * (float(scaledBounds.w) / float(canvasSize.w))),
			boost::math::iround(float(r.h) * (float(scaledBounds.h) / float(canvasSize.h))) );
	}

	cairo_surface_mark_dirty_rectangle( s, 0, 0, canvasSize.w, canvasSize.h );
	gtk_widget_queue_draw_area( _window, scaledBounds.x, scaledBounds.y, scaledBounds.w, scaledBounds.h );
}

void Window::draw( GtkWidget *da, const Rect &r ) {
	DTV_ASSERT(da);
	if (da == _window && _surface) {
		BOOST_FOREACH( canvas::VideoOverlay *vo, overlays()) {
			dynamic_cast<VideoOverlay*>(vo)->draw( r );
		}

		GdkWindow *win=gtk_widget_get_window(da);
		cairo_t *cr = gdk_cairo_create(win);

		cairo_rectangle( cr, r.x, r.y, r.w, r.h );
		cairo_clip( cr );

		const Rect &scaledBounds = targetWindow();
		const Size &canvasSize = getCanvasSize();

		double fx = double(scaledBounds.w) / double(canvasSize.w);
		double fy = double(scaledBounds.h) / double(canvasSize.h);
		cairo_scale( cr, fx, fy );
		cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_GOOD );

		cairo_set_source_surface(cr, _surface->getContent(), scaledBounds.x/fx, scaledBounds.y/fy);
		cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
		cairo_paint(cr);

		cairo_destroy(cr);
	}
}

//	Implementation
GtkWidget *Window::widget() const {
	return _window;
}

GtkWidget *Window::layout() const {
	return _layout;
}

}
}
