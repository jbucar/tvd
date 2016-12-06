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

#include "surface.h"
#include "canvas.h"
#include "font.h"
#include "glyphrun.h"
#include <util/assert.h>
#include <util/log.h>
#include <cairo/cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <stdexcept>
#ifdef _WIN32
#	define NOMINMAX
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <algorithm>
#include <stdlib.h>


#define CMP_CAIRO(c) (double(c)/255.0)
#define COLOR_TO_CAIRO(c) CMP_CAIRO(c.r), CMP_CAIRO(c.g), CMP_CAIRO(c.b), CMP_CAIRO(c.alpha)

namespace canvas {
namespace cairo {

namespace impl {

static inline void setup( cairo_t *cr ) {
	//	Set default line width
	cairo_set_line_width( cr, 1.0 );
	cairo_set_antialias( cr, CAIRO_ANTIALIAS_NONE );
}

static inline bool create( cairo_surface_t* &surface, cairo_t* &cr, int width, int height ) {
	surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height );
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo surface!");
		return false;
	}

	cr = cairo_create (surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo context!");
		return false;
	}

	setup( cr );

	return true;
}

static inline void destroy( cairo_surface_t *surface, cairo_t *cr ) {
	cairo_destroy(cr);
	cairo_surface_destroy (surface);
}

static inline void setColor( cairo_t *cr, const Color &color ) {
	//hack: if alpha is 0 we lose the rgb color of the pixel so
	//when alpha is 0 we change it to 1
	util::BYTE a = color.alpha;
	if ((color.r + color.g + color.b)>0 && a==0) {
		a = 1;
	}
	Color c = Color(color.r, color.g, color.b, a);
	cairo_set_source_rgba( cr, COLOR_TO_CAIRO(c) );
}

void gdk_cairo_set_source_pixbuf (
	cairo_t         *cr,
	const GdkPixbuf *pixbuf,
	double           pixbuf_x,
	double           pixbuf_y)
{
	gint width = gdk_pixbuf_get_width (pixbuf);
	gint height = gdk_pixbuf_get_height (pixbuf);
	guchar *gdk_pixels = gdk_pixbuf_get_pixels (pixbuf);
	int gdk_rowstride = gdk_pixbuf_get_rowstride (pixbuf);
	int n_channels = gdk_pixbuf_get_n_channels (pixbuf);
	int cairo_stride;
	guchar *cairo_pixels;
	cairo_format_t format;
	cairo_surface_t *surface;
	static const cairo_user_data_key_t key = {0};
	int j;

	if (n_channels == 3)
		format = CAIRO_FORMAT_RGB24;
	else
		format = CAIRO_FORMAT_ARGB32;

	cairo_stride = cairo_format_stride_for_width (format, width);
	cairo_pixels = (guchar *)g_malloc (height * cairo_stride);
	surface = cairo_image_surface_create_for_data ((unsigned char *)cairo_pixels,
		format,
		width, height, cairo_stride);
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LERROR("cairo::Surface", "Cannot create cairo from data!");
		throw std::runtime_error( "Cannot create cairo surface/context!" );		
	}

	cairo_surface_set_user_data (surface, &key,
		cairo_pixels, (cairo_destroy_func_t)g_free);

	for (j = height; j; j--)
	{
		guchar *p = gdk_pixels;
		guchar *q = cairo_pixels;

		if (n_channels == 3)
		{
			guchar *end = p + 3 * width;

			while (p < end)
			{
			#if G_BYTE_ORDER == G_LITTLE_ENDIAN
				q[0] = p[2];
				q[1] = p[1];
				q[2] = p[0];
			#else
				q[1] = p[0];
				q[2] = p[1];
				q[3] = p[2];
			#endif
				p += 3;
				q += 4;
			}
		}
		else
		{
			guchar *end = p + 4 * width;
			guint t1,t2,t3;

		#define MULT(d,c,a,t) { t = c * a + 0x7f; d = (guchar)(((t >> 8) + t) >> 8); }

			while (p < end)
			{
			#if G_BYTE_ORDER == G_LITTLE_ENDIAN
				MULT(q[0], p[2], p[3], t1);
				MULT(q[1], p[1], p[3], t2);
				MULT(q[2], p[0], p[3], t3);
				q[3] = p[3];
			#else
				q[0] = p[3];
				MULT(q[1], p[0], p[3], t1);
				MULT(q[2], p[1], p[3], t2);
				MULT(q[3], p[2], p[3], t3);
			#endif

				p += 4;
				q += 4;
			}

		#undef MULT
		}

		gdk_pixels += gdk_rowstride;
		cairo_pixels += cairo_stride;
	}

	cairo_set_source_surface (cr, surface, pixbuf_x, pixbuf_y);
	cairo_surface_destroy (surface);
}

}	//	namespace impl


Surface::Surface( Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) )
{
	_canvas = canvas;

	//	Create cairo surface for data
	const Size &size = img->size;
	int stride = cairo_format_stride_for_width( CAIRO_FORMAT_ARGB32, size.w );
	if (img->length < (size.h*stride)) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
	
	_surface = cairo_image_surface_create_for_data(
		img->data,
		CAIRO_FORMAT_ARGB32,
		size.w, size.h,
		stride );

	_cr = cairo_create (_surface);
	if (cairo_status(_cr) != CAIRO_STATUS_SUCCESS) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}

	impl::setup( _cr );

	init();
}

Surface::Surface( Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) )
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::cairo::Surface] Cannot create surface!!! Invalid bounds");
	}

	_canvas = canvas;
	if (!impl::create( _surface, _cr, rect.w, rect.h )) {
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
	init();
}

Surface::Surface( Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) )
{
	_canvas = canvas;

	GError *err=NULL;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file( file.c_str(), &err );
	if (!pixbuf) {
		LERROR( "cairo::Surface", "Cannot load image file: file=%s, err=%s\n",
			file.c_str(), err->message );
		throw std::runtime_error( "Cannot load image file!" );
	}
	
	int width = gdk_pixbuf_get_width(pixbuf);
	int height = gdk_pixbuf_get_height(pixbuf);
	if (!impl::create( _surface, _cr, width, height )) {
		LERROR( "cairo::Surface", "Cannot create cairo surface/context!" );
		throw std::runtime_error( "Cannot create cairo surface/context!" );
	}
	init();

	impl::gdk_cairo_set_source_pixbuf( _cr, pixbuf, 0, 0 );
	cairo_paint( _cr );

	g_object_unref(pixbuf);
}

void Surface::init() {
	_alpha = 1.0;
	setCompositionModeImpl( getCompositionMode() );
	cairo_rectangle(_cr, 0, 0, cairo_image_surface_get_width( _surface ), cairo_image_surface_get_height( _surface ));
	cairo_clip(_cr);
}

Surface::~Surface()
{
	impl::destroy( _surface, _cr );
}

cairo_surface_t *Surface::getContent() {
	return _surface;
}

cairo_t *Surface::getContext() {
	return _cr;
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

/****************************** Size functions ********************************/

Size Surface::getSize() const {
	Size size;
	size.w = cairo_image_surface_get_width( _surface );
	size.h = cairo_image_surface_get_height( _surface );
	return size;
}

/****************************** Clip functions ********************************/

bool Surface::getClip( Rect &rect ) {
	double x1, y1, x2, y2;

	cairo_clip_extents( _cr, &x1, &y1, &x2, &y2 );

	rect.x = (int) x1;
	rect.y = (int) y1;
	rect.w = (int) (x2 - x1);
	rect.h = (int) (y2 - y1);

	return true;
}

void Surface::setClipImpl( const Rect &rect ) {
	cairo_reset_clip( _cr );
	cairo_rectangle( _cr, rect.x, rect.y, rect.w, rect.h );
	cairo_clip( _cr );
}

void Surface::subClip( const Rect &clip, const Rect &rect ) {
	int x = std::max(rect.x, clip.x);
	int y = std::max(rect.y, clip.y);
	int x2 = std::min(rect.x + rect.w, clip.x + clip.w);
	int y2 = std::min(rect.y + rect.h, clip.y + clip.h);
	setClipImpl( Rect(x, y, x2-x, y2-y) );
}

/****************************** Line functions ********************************/

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_move_to (_cr, (x1<x2)? x1 : x1+1, (y1<y2)? y1 : y1+1 );
	cairo_line_to (_cr, (x1<x2)? x2+1 : x2, (y1<y2)? y2+1 : y2 );
	cairo_set_line_width (_cr, 1);
	cairo_stroke (_cr);

	cairo_restore(_cr);
}

/****************************** Rect functions ********************************/

void Surface::drawRectImpl( const Rect &rect ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	cairo_rectangle ( _cr, rect.x+1, rect.y+1, rect.w-1, rect.h-1 );
	cairo_stroke (_cr);

	cairo_restore(_cr);
}

void Surface::fillRectImpl( const Rect &rect ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	cairo_rectangle ( _cr, rect.x, rect.y, rect.w, rect.h );
	cairo_fill(_cr);

	cairo_restore(_cr);
}

/*************************** RoundRect functions ******************************/

void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	int    x = rect.x,
	y = rect.y,
	width = rect.w,
	height = rect.h;
	double dradius = (arcW + arcH) / 2,
	degrees = M_PI / 180.0;

	int radius = (int) ceil(dradius);
	if(radius >= (width / 2)){
		radius = width / 2 - 1;
	}
	if(radius >= (height/ 2)){
		radius = height / 2 - 1;
	}

	cairo_arc (_cr, x + width - radius, y + radius, radius, 270 * degrees, 0 * degrees);
	cairo_line_to(_cr, rect.x + rect.w, rect.y + rect.h - radius );
	cairo_arc (_cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_line_to(_cr, rect.x + radius, rect.y + rect.h );
	cairo_arc (_cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_line_to(_cr, rect.x + 0.5, rect.y + radius );
	cairo_arc (_cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_line_to(_cr, rect.x + rect.w - radius, rect.y + 0.5 );

	cairo_stroke (_cr);

	cairo_restore(_cr);
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	int    x = rect.x,
	y = rect.y,
	width = rect.w,
	height = rect.h;
	double dradius = (arcW + arcH) / 2,
	degrees = M_PI / 180.0;

	int radius = (int) ceil(dradius);
	if(radius >= (width / 2)){
		radius = width / 2 - 1;
	}
	if(radius >= (height/ 2)){
		radius = height / 2 - 1;
	}

	cairo_arc (_cr, x + width - radius, y + radius, radius, 270 * degrees, 0 * degrees);
	cairo_line_to(_cr, rect.x + rect.w, rect.y + rect.h - radius );
	cairo_arc (_cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_line_to(_cr, rect.x + radius, rect.y + rect.h );
	cairo_arc (_cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_line_to(_cr, rect.x + 0.5, rect.y + radius );
	cairo_arc (_cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_line_to(_cr, rect.x + rect.w - radius, rect.y + 0.5 );

	cairo_fill (_cr);

	cairo_restore(_cr);
}

/**************************** Polygon functions ******************************/

void Surface::drawRelLineTo( const Point &lp, const Point &np ) {
	int x = lp.x;
	int y = lp.y;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la derecha
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x+1;// Hacia arriba a la derecha
			y = np.y-lp.y-1;
		} else {
			x = np.x-lp.x;// Horizontal hacia la derecha
			y = np.y-lp.y;
		}
	} else if (lp.x > np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la izquierda
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x-1;	// Hacia arriba a la izquierda
			y = np.y-lp.y-1;
		} else {
			x = np.x-lp.x;	// Horizontal hacia la izquierda
			y = np.y-lp.y;
		}
	} else {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Vertical hacia abajo
			y = np.y-lp.y+1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x; 	// Vertical hacia arriba
			y = np.y-lp.y+1;
		}
	}
	cairo_rel_line_to(_cr, x, y );
}

void Surface::drawLineTo( const Point &lp, const Point &np ) {
	int x= lp.x;
	int y= lp.y;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Hacia abajo a la derecha
			y = np.y-lp.y;
		} else if (lp.y > np.y) {
			x = np.x-lp.x-1;// Hacia arriba a la derecha
			y = np.y-lp.y+1;
		} else {
			x = np.x-lp.x+1;// Horizontal hacia la derecha
			y = np.y-lp.y;
		}
	} else if (lp.x > np.x) {
		if (lp.y < np.y) {
			x = np.x-lp.x-1;// Hacia abajo a la izquierda
			y = np.y-lp.y+1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x;	// Hacia arriba a la izquierda
			y = np.y-lp.y;
		} else {
			x = np.x-lp.x+1;// Horizontal hacia la izquierda
			y = np.y-lp.y;
		}
	} else {
		if (lp.y < np.y) {
			x = np.x-lp.x;	// Vertical hacia abajo
			y = np.y-lp.y-1;
		} else if (lp.y > np.y) {
			x = np.x-lp.x; 	// Vertical hacia arriba
			y = np.y-lp.y-1;
		}
	}
	cairo_rel_line_to(_cr, x, y );
}

void Surface::drawPolygonImpl( const std::vector<Point>& vertices, bool closed ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_width (_cr, 1);

	const Point& fp = vertices[0];
	Point lp = fp;	// Last point drawn
	Point np = vertices[1]; // Next point to draw
	int mx, my;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else {
			mx = lp.x;
			my = lp.y+1;
		}
	} else if(lp.x > np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else {
			mx = lp.x;
			my = lp.y+1;
		}
	} else {
		mx = lp.x+1;
		my = lp.y+1;
	}
	cairo_move_to( _cr, mx, my );
	for (size_t i=1; i<vertices.size()-1; i++) {
		np = vertices[i];
		drawLineTo(lp, np);
		lp = np;
	}
	np = vertices[vertices.size()-1];
	if (np!=fp) {
		drawLineTo(lp, np);
	}

	if (closed || (!closed && np == fp)) {
		cairo_close_path( _cr );
	}

	cairo_stroke (_cr);

	cairo_restore(_cr);
}

void Surface::fillPolygonImpl(const std::vector<Point>& vertices ) {
	cairo_save(_cr);

	impl::setColor( _cr, getColor() );
	cairo_set_line_cap  (_cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_width (_cr, 1);

	Point fp = vertices[0];
	Point lp = fp; // Last point drawn
	Point np = vertices[1]; // Next point to draw
	int mx;
	int my;
	if (lp.x < np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else if (lp.y > np.y) {
			mx = lp.x;
			my = lp.y+1;
		} else {
			mx = lp.x+1;
			my = lp.y;
		}
	} else if(lp.x > np.x) {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else if (lp.y > np.y) {
			mx = lp.x+1;
			my = lp.y+1;
		} else {
			mx = lp.x+1;
			my = lp.y+1;
		}
	} else {
		if (lp.y < np.y) {
			mx = lp.x+1;
			my = lp.y;
		} else {
			mx = lp.x;
			my = lp.y;
		}
	}

	cairo_move_to( _cr, mx, my );
	for (size_t i=1; i<vertices.size()-1; i++) {
		np = vertices[i];
		drawRelLineTo(lp, np);
		lp = np;
	}
	np = vertices[vertices.size()-1];
	if (np!=fp) {
		drawRelLineTo(lp, np);
	}
	cairo_close_path( _cr );
	cairo_fill (_cr);

	cairo_restore(_cr);
}

/**************************** Ellipse functions ******************************/

void Surface::drawEllipseImpl( const canvas::Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	impl::setColor( _cr, getColor() );

	cairo_save(_cr);

	cairo_translate(_cr,center.x+0.5,center.y+0.5);
	cairo_scale(_cr,rw  , rh );
	cairo_arc(_cr,0.0, 0.0, 1.0, 0.0, 2 * M_PI);

	cairo_restore(_cr);
	cairo_stroke(_cr);
}


void Surface::fillEllipseImpl( const canvas::Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	impl::setColor( _cr, getColor() );

	cairo_save(_cr);

	cairo_translate(_cr,center.x+0.5,center.y+0.5);
	cairo_scale(_cr,rw  , rh );
	cairo_arc(_cr,0.0, 0.0, 1.0, 0.0, 2 * M_PI);
	cairo_fill_preserve(_cr);

	cairo_restore(_cr);
	cairo_stroke(_cr);
}

bool Surface::hackDejaVuFont() const {
	return false;
}

/****************************** Text functions ********************************/
canvas::GlyphRun *Surface::createGlyphRun( const std::string &text, const Point &/*pos*/ ) {
	return new GlyphRun(text.length());
}

void Surface::renderText( canvas::GlyphRun *glyphRun, const Point &/*pos*/ ) {
	GlyphRun *gr = dynamic_cast<GlyphRun*>(glyphRun);
	const Font *f = dynamic_cast<const Font*>(font());

	cairo_save(_cr);
	cairo_matrix_t ctm;
	cairo_matrix_init_identity(&ctm);
	cairo_set_matrix(_cr, &ctm);

	cairo_set_font_face( _cr, f->cairoFont() );
	cairo_set_font_size( _cr, f->size() );
	impl::setColor( _cr, getColor() );
	cairo_show_glyphs( _cr, gr->glyphs(), gr->cant() );
	cairo_new_path( _cr );

	cairo_restore(_cr);
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = (float) (alpha / 255.0);
	markDirty();
	return true;
}

util::BYTE Surface::getOpacity() const{
	return (util::BYTE) (_alpha * 255);
}

float Surface::alpha() const{
	return _alpha;
}

void Surface::setCompositionModeImpl( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: _op = CAIRO_OPERATOR_OVER; break;
		case composition::source: _op = CAIRO_OPERATOR_SOURCE; break;
		case composition::clear: _op = CAIRO_OPERATOR_CLEAR; break;
		default: return;
	}
	cairo_set_operator( _cr, (cairo_operator_t)_op );
}

void Surface::blitImpl( const Point &targetPoint, canvas::Surface *srcSurface, const Rect &source ) {
	cairo_save(_cr);

	Rect oldClip;
	getClip(oldClip);
	Surface *src=dynamic_cast<Surface *>(srcSurface);
	cairo_set_source_surface ( _cr, src->getContent(), targetPoint.x-source.x, targetPoint.y-source.y );
	subClip( oldClip, Rect(targetPoint.x, targetPoint.y, source.w, source.h) );
	cairo_paint_with_alpha( _cr, src->alpha() );
	setClipImpl( oldClip );

	cairo_restore(_cr);
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	cairo_save(_cr);

	Rect oldClip;
	getClip(oldClip);
	Surface *src = NULL;
	Rect r(Point(0,0),srcSurface->getSize());
	if (r != sourceRect) {
		canvas::Surface *tmp = canvas()->createSurface(Size(sourceRect.w, sourceRect.h));
		tmp->blit(Point(0,0), srcSurface, sourceRect);
		src = dynamic_cast<Surface *>(tmp);
	} else {
		src = dynamic_cast<Surface *>(srcSurface);
	}

	cairo_t *cr = NULL;
	cairo_surface_t *content = NULL;

	double fx = ((double)targetRect.w / (double)sourceRect.w) * (flipw ? -1.0 : 1.0);
	double fy = ((double)targetRect.h / (double)sourceRect.h) * (fliph ? -1.0 : 1.0);
	impl::create( content, cr, targetRect.w, targetRect.h );
	cairo_scale( cr, fx, fy );
	if (fx<0 && fy<0) {
		cairo_translate( cr, targetRect.w/fx, targetRect.h/fy );
	} else if (fx < 0) {
		cairo_translate( cr, targetRect.w/fx, 0 );
	} else if (fy < 0) {
		cairo_translate( cr, 0, targetRect.h/fy );
	}
	cairo_set_source_surface( cr, src->getContent(), 0, 0 );
	cairo_pattern_set_filter( cairo_get_source(cr), CAIRO_FILTER_GOOD );
	cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint( cr );

	cairo_set_source_surface( _cr, content, targetRect.x, targetRect.y );
	subClip( oldClip, targetRect );
	cairo_paint_with_alpha( _cr, src->alpha() );

	setClipImpl( oldClip );

	impl::destroy( content, cr );
	if (r != sourceRect) {
		canvas::Surface *tmp = dynamic_cast<canvas::Surface *>(src);
		canvas()->destroy( tmp );
	}

	cairo_restore(_cr);
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	cairo_save(_cr);

	int tw=0;
	int th=0;
	canvas::Rect rect;
	canvas::Size size = getSize();
	if (degrees==90 || degrees==270) {
		canvas::Point pos = getLocation();
		rect = canvas::Rect(pos.x,pos.y,size.h,size.w);
		if (degrees==90) {
			th = -size.h;
		} else {
			tw = -size.w;
		}
	} else {
		rect = getBounds();
		if (degrees==180) {
			tw = -size.w;
			th = -size.h;
		}
	}
	Surface *newSurface = dynamic_cast<Surface *>( createSimilar( rect ) );
	double radians = ((double)degrees)*boost::math::constants::pi<double>()/180.0;

	cairo_t *ctx = newSurface->getContext();
	cairo_rotate( ctx, radians );
	cairo_translate( ctx, tw, th);
	cairo_set_source_surface( ctx, getContent(), 0, 0 );
	cairo_paint( ctx );

	cairo_restore(_cr);

	return newSurface;
}

/*********************** Pixel manipulation functions *************************/
util::DWORD Surface::getPixel( const Point &pos ) {
	DTV_ASSERT( cairo_image_surface_get_format(_surface) == CAIRO_FORMAT_ARGB32 );
	unsigned char *data = cairo_image_surface_get_data(_surface);
	int stride = cairo_image_surface_get_stride(_surface);
	util::DWORD *pixelptr;
	pixelptr = (util::DWORD *)(data + pos.y * stride); // row

	return pixelptr[pos.x]; // pixel
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	util::DWORD pixel = getPixel( pos );

	color.alpha = (unsigned char)((pixel & 0xFF000000) >> 24);
	color.r = (unsigned char)((pixel & 0x00FF0000) >> 16);
	color.g = (unsigned char)((pixel & 0x0000FF00) >> 8);
	color.b = (unsigned char)(pixel & 0x000000FF);

	if (color.alpha != 255) {
		if (color.alpha != 0) {
			double factor = 255.0/double(color.alpha);
			color.r = (unsigned char) boost::math::iround(color.r*factor);
			color.g = (unsigned char) boost::math::iround(color.g*factor);
			color.b = (unsigned char) boost::math::iround(color.b*factor);
		}
	}

}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	cairo_save(_cr);

	cairo_rectangle ( _cr, pos.x, pos.y, 1, 1);
	cairo_set_operator( _cr, CAIRO_OPERATOR_SOURCE );
	impl::setColor( _cr, color );
	cairo_fill ( _cr );
	cairo_set_operator( _cr, (cairo_operator_t)_op );

	cairo_restore(_cr);
}

bool Surface::saveAsImage( const std::string &file ) {
	return cairo_surface_write_to_png( _surface, file.c_str() ) == CAIRO_STATUS_SUCCESS;
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	cairo_t *cr = NULL;
	cairo_surface_t *content = NULL;
	impl::create( content, cr, size.w, size.h );
	cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
	cairo_rectangle(cr, 0, 0, size.w, size.h);
	cairo_clip(cr);

	if (scaleContent) {
		cairo_save(cr);

		Size actualSize = getSize();
		double fx = (double)size.w / (double)actualSize.w;
		double fy = (double)size.h / (double)actualSize.h;
		cairo_scale( cr, fx, fy );
		cairo_set_source_surface( cr, _surface, 0, 0 );
		cairo_set_operator( cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint( cr );
		setCompositionModeImpl(getCompositionMode());

		cairo_restore(cr);
	}
	impl::destroy( _surface, _cr );
	_surface = content;
	_cr = cr;

}

util::DWORD *Surface::pixels() {
	cairo_surface_flush(_surface);
	int w = cairo_image_surface_get_width( _surface );
	int h = cairo_image_surface_get_height( _surface );
	size_t size = w*h*sizeof(util::DWORD);
	unsigned char *data = cairo_image_surface_get_data(_surface);
	util::DWORD *pixels = (util::DWORD*) malloc( size );

	memcpy( pixels, data, size );
	return pixels;
}

}
}
