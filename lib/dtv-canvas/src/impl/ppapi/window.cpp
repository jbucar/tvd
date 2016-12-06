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
#include "../cairo/surface.h"
#include "../cairo/canvas.h"
#include <util/cfg/configregistrator.h>
#include <util/log.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <boost/thread.hpp>
#include <cairo/cairo.h>
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/utility/graphics/paint_manager.h>

namespace canvas {
namespace ppapi {

class PaintClient : public pp::PaintManager::Client {
public:
	PaintClient( Window *win ) : _win(win) {}
	virtual ~PaintClient() {}

	// PaintManager::Client implementation.
	virtual bool OnPaint(pp::Graphics2D& graphics_2d, const std::vector<pp::Rect> & /*paint_rects*/, const pp::Rect& paint_bounds) {
		bool result=false;
		LDEBUG( "ppapi", "On paint: size=(%d,%d,%d,%d)", paint_bounds.x(), paint_bounds.y(), paint_bounds.width(), paint_bounds.height() );
		_mutex.lock();
		if (_win) {
			pp::ImageData updated_image( _win->sys()->instance(), PP_IMAGEDATAFORMAT_BGRA_PREMUL, paint_bounds.size(), true );
			Rect r(paint_bounds.x(), paint_bounds.y(), paint_bounds.width(), paint_bounds.height());
			if (updated_image.data()) {
				_win->onPaint( updated_image.data(), r );
				graphics_2d.ReplaceContents(&updated_image);
				result=true;
			}
		}
		_mutex.unlock();
		return result;
	}

	void reset() {
		_mutex.lock();
		_win = NULL;
		_mutex.unlock();
	}

	void lock() {
		_mutex.lock();
	}

	void unlock() {
		_mutex.unlock();
	}

private:
	Window *_win;
	boost::mutex _mutex;
};


Window::Window( System *sys )
	: _sys(sys)
{
	_client = NULL;
	_paintMgr = NULL;
	_surface = NULL;

	{	//	Setup size
		pp::Size s = _sys->viewSize();
		util::cfg::setValue<int>("gui.window.size.width", s.width());
		util::cfg::setValue<int>("gui.window.size.height", s.height());
	}
}

Window::~Window()
{
	DTV_ASSERT(!_paintMgr);
}

//	Layer methods
bool Window::initLayer( canvas::Canvas *canvas ) {
	DTV_ASSERT(!_surface);
	LDEBUG( "ppapi", "Init layer" );
	const Size &size = canvas->size();
	_surface = new cairo::Surface( (cairo::Canvas *)canvas, Rect(0,0,size.w,size.h) );
	if (_surface) {
		_client = new PaintClient(this);
		_paintMgr = new pp::PaintManager(_sys->instance(), _client, false );
		_paintMgr->SetSize(_sys->viewSize());
		return true;
	}
	return false;
}

void Window::finLayer( canvas::Canvas * /*canvas*/ ) {
	LDEBUG( "ppapi", "Fin layer" );
	//	HACK: We cannot delete becouse onPaint is not multi-thread .....
	_client->reset();
	_client = NULL;
	_paintMgr = NULL;
	DEL(_surface);
}

canvas::Surface *Window::lockLayer() {
	DTV_ASSERT(_surface);
	//	TODO: Ver como hacer resize en window base
	// pp::Size s = _sys->viewSize();
	// resize( Size(s.width(), s.height()) );
	_client->lock();
	return _surface;
}

void Window::unlockLayer( Surface * /*surface*/ ) {
	_client->unlock();
}

void Window::renderLayerImpl( canvas::Surface * /*surface*/, const std::vector<Rect> &/*dirtyRegions*/ ) {
	pp::Size s = _sys->viewSize();
	_paintMgr->InvalidateRect(pp::Rect(0,0,s.width(),s.height()));
	_paintMgr->InvalidateRect(pp::Rect(0,0,s.width(),s.height()));	//	HACK: Pending paint are not painted the first time
}

void Window::onPaint( void *data, const Rect &rect ) {
	DTV_ASSERT(data);
	DTV_ASSERT(_surface);

	int stride = cairo_format_stride_for_width( CAIRO_FORMAT_ARGB32, rect.w );
	cairo_surface_t *surface = cairo_image_surface_create_for_data(
		(unsigned char *)data,
		CAIRO_FORMAT_ARGB32,
		rect.w, rect.h,
		stride
	);
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LERROR( "ppapi", "Cannot create surface for image data" );
		return;
	}

	cairo_t *cr = cairo_create (surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		LERROR( "ppapi", "Cannot create context for image data" );
		cairo_surface_destroy( surface );
		return;
	}
	cairo_surface_mark_dirty(surface);

	//	Copy content scaled to window size
	const Rect &scaledBds = targetWindow();
	Size canvSize = getCanvasSize();
	double fx = double(scaledBds.w) / double(canvSize.w);
	double fy = double(scaledBds.h) / double(canvSize.h);
	cairo_scale( cr, fx, fy );
	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_GOOD );

	cairo_surface_flush( _surface->getContent() );
	cairo_set_source_surface(cr, _surface->getContent(), scaledBds.x/fx, scaledBds.y/fy);

	cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
	cairo_paint(cr);

	LTRACE( "ppapi", "paint: rect=(%d,%d,%d,%d), target=(%d,%d,%d,%d)",
		rect.x, rect.y, rect.w, rect.h,
		scaledBds.x, scaledBds.y, scaledBds.w, scaledBds.h );

	cairo_surface_destroy( surface );
	cairo_destroy( cr );
}

//	Size methods
void Window::iconifyImpl( bool /*enable*/ ) {
}

bool Window::supportResize() const {
	return true;
}

void Window::setSizeImpl( const Size &size ) {
	LDEBUG( "ppapi", "Set size impl: size=(%d,%d)", size.w, size.h );
	if (_paintMgr) {
		pp::Size s(size.w,size.h);
		_paintMgr->SetSize(s);
	}
}

Size Window::screenSize() const {
	pp::Size s = _sys->viewSize();
	return Size(s.width(), s.height());
}

System *Window::sys() const {
	return _sys;
}

}
}
