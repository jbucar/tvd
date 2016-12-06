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
#include "../../rect.h"
#include <util/log.h>
#include <util/mcr.h>
#include <SkCanvas.h>
#include <SkBitmap.h>
#include <SkPaint.h>
#include <SkPath.h>
#include <SkImageDecoder.h>
#include <SkImageEncoder.h>
#include <SkStream.h>
#include <boost/math/special_functions/round.hpp>

namespace canvas {
namespace skia {

#define PAINTER SkPaint p; \
		{ \
			Color c = getColor(); \
			p.setARGB( ((c.r + c.g + c.b)>0 && c.alpha==0) ? 1 : c.alpha, c.r, c.g, c.b ); \
			p.setXfermodeMode( impl::getSkiaCompositionMode( getCompositionMode()) ); \
		}

namespace impl {
static SkXfermode::Mode getSkiaCompositionMode( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: return SkXfermode::kSrcOver_Mode;
		case composition::source: return SkXfermode::kSrc_Mode;
		case composition::clear: return SkXfermode::kClear_Mode;
		default: throw std::runtime_error("[canvas::skia::Surface] composition::mode not suported!!!");
	}
}

}

Surface::Surface( Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) ), _canvas(canvas)
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::skia::Surface] Cannot create surface!!! Invalid bounds");
	}
	_bitmap = new SkBitmap();
	_bitmap->setConfig( SkBitmap::kARGB_8888_Config, rect.w, rect.h );
	_bitmap->allocPixels();
	_bitmap->eraseARGB( 0, 0, 0, 0 );
	init();
}

Surface::Surface( Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) ), _canvas(canvas)
{
	_bitmap = new SkBitmap();
	if (!SkImageDecoder::DecodeFile( file.c_str(), _bitmap, SkBitmap::kARGB_8888_Config, SkImageDecoder::kDecodePixels_Mode )) {
		LERROR( "skia::Surface", "Cannot load image file: file=%s", file.c_str() );
		throw std::runtime_error("[canvas::skia::Surface] Cannot create surface from file!!!");
	}
	init();
}

Surface::Surface( Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) ), _canvas(canvas)
{
	_bitmap = new SkBitmap();
	_bitmap->setConfig( SkBitmap::kARGB_8888_Config, img->size.w, img->size.h );
	_bitmap->setPixels( img->data );
	init();
}

Surface::~Surface()
{
	DEL( _surface );
	DEL( _bitmap );
}

void Surface::init() {
	_alpha = 255;
	_surface = new SkCanvas( *_bitmap );
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

Size Surface::getSize() const {
	SkISize s = _surface->getDeviceSize();
	return Size(s.width(), s.height());
}

bool Surface::getClip( Rect &rect ) {
	SkRect r;
	bool result = _surface->getClipBounds( &r );
	rect.x = boost::math::iround(r.x())+1;
	rect.y = boost::math::iround(r.y())+1;
	rect.w = boost::math::iround(r.width())-2;
	rect.h = boost::math::iround(r.height())-2;
	return result;
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = alpha;
	return true;
}

util::BYTE Surface::getOpacity() const {
	return _alpha;
}

bool Surface::saveAsImage( const std::string &file ) {
	return SkImageEncoder::EncodeFile( file.c_str(), *_bitmap, SkImageEncoder::kPNG_Type, 100 );
}

void Surface::setClipImpl( const Rect &rect ) {
	SkRect r = SkRect::MakeXYWH( (SkScalar)rect.x, (SkScalar)rect.y, (SkScalar)rect.w, (SkScalar)rect.h );
	_surface->clipRect( r, SkRegion::kReplace_Op );
}

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	PAINTER
	int minX, minY, maxX, maxY;
	if (x1==x2 || y1==y2) {
		// Horizontal or vertical lines
		minX = std::min(x1, x2);
		maxX = std::max(x1, x2);
		minY = std::min(y1, y2);
		maxY = std::max(y1, y2);
		_surface->drawLine( (SkScalar)minX, (SkScalar)minY, (SkScalar)((y1==y2) ? maxX+1 : maxX), (SkScalar)((x1==x2) ? maxY+1 : maxY), p );
	} else {
		// Diagonal lines
		if (x1<x2) {
			minX = x1;
			minY = y1;
			maxX = x2;
			maxY = y2;
		} else {
			minX = x2;
			minY = y2;
			maxX = x1;
			maxY = y1;
		}
		_surface->drawLine( (SkScalar)minX, (SkScalar) ((minY<maxY) ? minY : minY+1), (SkScalar)(maxX+1), (SkScalar)((minY<maxY) ? maxY+1 : maxY), p );
	}
}

void Surface::drawRectImpl( const Rect &rect ) {
	PAINTER
	SkRect r = SkRect::MakeXYWH((SkScalar)rect.x, (SkScalar)rect.y, (SkScalar)(rect.w-1), (SkScalar)(rect.h-1));
	p.setStyle( SkPaint::kStroke_Style );
	_surface->drawRect( r, p );
}

void Surface::fillRectImpl( const Rect &rect ) {
	PAINTER
	SkRect r = SkRect::MakeXYWH((SkScalar)rect.x, (SkScalar)rect.y, (SkScalar)rect.w, (SkScalar)rect.h);
	p.setStyle( SkPaint::kFill_Style );
	_surface->drawRect( r, p );
}

void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	PAINTER
	p.setStyle( SkPaint::kStroke_Style );
	SkRect r = SkRect::MakeXYWH((SkScalar)rect.x, (SkScalar)rect.y, (SkScalar)(rect.w-1), (SkScalar)(rect.h-1));
	_surface->drawRoundRect( r, (SkScalar)arcW, (SkScalar)arcH, p );
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	PAINTER
	p.setStyle( SkPaint::kFill_Style );
	SkRect r = SkRect::MakeXYWH((SkScalar)rect.x, (SkScalar)rect.y, (SkScalar)(rect.w-1), (SkScalar)(rect.h-1));
	_surface->drawRoundRect( r, (SkScalar)arcW, (SkScalar)arcH, p );
}

void Surface::drawPolygonImpl( const std::vector<Point> &vertices, bool closed ) {
	SkPath path;
	Point last = vertices[0];
	Point next = vertices[1];
	if ((next.x<last.x && next.y>last.y) || (next.x>last.x && next.y<last.y)) {
		path.moveTo( (SkScalar)last.x, (SkScalar)(last.y+1) );
	} else if (next.y!=last.y) {
		path.moveTo( (SkScalar)last.x, (SkScalar)last.y );
	} else {
		path.moveTo( (SkScalar)(last.x+1), (SkScalar)last.y );
	}
	for (unsigned i = 1; i < vertices.size(); ++i) {
		next = vertices[i];
		if (next.x==last.x) {
			path.lineTo( (SkScalar)next.x, (SkScalar)(next.y+1) );
		} else if ((next.x<last.x && next.y>last.y) || 
			   (next.x>last.x && next.y<last.y)) {
			path.lineTo( (SkScalar)(next.x+1), (SkScalar)next.y );
		} else {
			path.lineTo( (SkScalar)next.x, (SkScalar)next.y );
		}
		last = next;
	}
	if (closed) {
		path.close();
	}
	PAINTER
	p.setStyle( SkPaint::kStroke_Style );
	_surface->drawPath( path, p );
}

void Surface::fillPolygonImpl( const std::vector<Point> &vertices ) {
	SkPath path;
	path.moveTo( (SkScalar)vertices[0].x, (SkScalar)vertices[0].y );
	for (unsigned i = 1; i < vertices.size(); ++i) {
		path.lineTo( (SkScalar)vertices[i].x, (SkScalar)vertices[i].y );
	}
	path.close();
	PAINTER
	p.setStyle( SkPaint::kFill_Style );
	_surface->drawPath( path, p );
}

void Surface::drawEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	SkRect rect = SkRect::MakeXYWH( (SkScalar)(center.x-rw), (SkScalar)(center.y-rh), (SkScalar)(rw*2+1), (SkScalar)(rh*2+1) );
	PAINTER
	p.setStyle( SkPaint::kStroke_Style );
	_surface->drawOval( rect, p );
}

void Surface::fillEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	SkRect rect = SkRect::MakeXYWH( (SkScalar)(center.x-rw), (SkScalar)(center.y-rh), (SkScalar)(rw*2), (SkScalar)(rh*2) );
	PAINTER
	p.setStyle( SkPaint::kFill_Style );
	_surface->drawOval( rect, p );
}

void Surface::blitImpl( const Point &target, canvas::Surface *srcSurface, const Rect &source ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);
	SkRect srcRect = SkRect::MakeXYWH( (SkScalar)source.x, (SkScalar)source.y, (SkScalar)source.w, (SkScalar)source.h );
	SkRect destRect = SkRect::MakeXYWH( (SkScalar)target.x, (SkScalar)target.y, (SkScalar)source.w, (SkScalar)source.h );
	PAINTER
	p.setAlpha( src->getOpacity() );
	_surface->drawBitmapRectToRect( *src->bitmap(), &srcRect, destRect, &p );
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	Surface *src=dynamic_cast<Surface *>(srcSurface);
	SkRect srcRect = SkRect::MakeXYWH( (SkScalar)sourceRect.x, (SkScalar)sourceRect.y, (SkScalar)sourceRect.w, (SkScalar)sourceRect.h );
	SkRect dstRect = SkRect::MakeXYWH( (SkScalar)targetRect.x, (SkScalar)targetRect.y, (SkScalar)targetRect.w, (SkScalar)targetRect.h );
	PAINTER
	p.setAlpha( src->getOpacity() );
	_surface->scale( flipw ? -1 : 1, fliph ? -1 : 1 );
	if (flipw && fliph) {
		_surface->translate( (SkScalar)(targetRect.w * -1), (SkScalar)(targetRect.h * -1) );
	} else if (flipw) {
		_surface->translate( (SkScalar)(targetRect.w * -1), 0 );
	} else if (fliph) {
		_surface->translate( 0,(SkScalar)(targetRect.h * -1) );
	}
	_surface->drawBitmapRectToRect( *src->bitmap(), &srcRect, dstRect, &p );
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
	canvas::Rect rect = getBounds();
	canvas::Size size = getSize();
	if (degrees==90 || degrees==270) {
		rect.w = size.h;
		rect.h = size.w;
	}
	Surface *newSurface = dynamic_cast<Surface *>( createSimilar( rect ) );
	SkCanvas *newCanvas = newSurface->content();
	newCanvas->rotate( (SkScalar)degrees );

	int tw = 0;
	int th = 0;
	if (degrees==90 || degrees==180) {
		th = -size.h;
	}
	if (degrees==270 || degrees==180) {
		tw = -size.w;
	}
	newCanvas->translate( (SkScalar)tw, (SkScalar)th );

	SkPaint p;
	p.setXfermodeMode( SkXfermode::kSrc_Mode );
	newCanvas->drawBitmap( *_bitmap, 0, 0, &p );

	return newSurface;
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	// ARGB
	util::DWORD pixel = getPixel( pos );
	color.alpha = (util::BYTE) ((pixel & 0xFF000000) >> 24);
	color.r = (util::BYTE) boost::math::iround( (double)((pixel & 0x00FF0000) >> 16) * 255 / (color.alpha ? color.alpha : 1) );
	color.g = (util::BYTE) boost::math::iround( (double)((pixel & 0x0000FF00) >> 8) * 255 / (color.alpha ? color.alpha : 1) );
	color.b = (util::BYTE) boost::math::iround( (double)(pixel & 0x000000FF) * 255 / (color.alpha ? color.alpha : 1) );
}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	_bitmap->lockPixels();
	util::DWORD *pixel = _bitmap->getAddr32( pos.x, pos.y );
	util::DWORD c = 0x00000000;
	util::BYTE a = color.alpha ? color.alpha : (util::BYTE) 1;
	c |= (a << 24);
	c |= (boost::math::iround( (double)color.r * a / 255) << 16 );
	c |= (boost::math::iround( (double)color.g * a / 255) << 8 );
	c |= boost::math::iround( (double)color.b * a / 255 );
	*pixel = c;
	_bitmap->unlockPixels();
}

/****************************** Text functions ********************************/
canvas::GlyphRun *Surface::createGlyphRun( const std::string &text, const Point &/*pos*/ ) {
	return new GlyphRun(text.length());
}

void Surface::renderText( canvas::GlyphRun *glyphRun, const Point &/*pos*/ ) {
	GlyphRun *gr = dynamic_cast<GlyphRun*>(glyphRun);
	const Font *f = dynamic_cast<const Font*>(font());

	PAINTER
	p.setAntiAlias(true);
	p.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
	p.setTypeface(f->skFont());
	p.setTextSize((SkScalar)f->size());

	_surface->drawPosText(gr->glyphs(), gr->cant() << 1, gr->positions(), p);
}

util::DWORD Surface::getPixel( const Point &pos ) {
	_bitmap->lockPixels();
	util::DWORD *pixel = _bitmap->getAddr32( pos.x, pos.y );
	_bitmap->unlockPixels();
	return *pixel;
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	SkBitmap *newBitmap = new SkBitmap();
	newBitmap->setConfig( SkBitmap::kARGB_8888_Config, size.w, size.h );
	newBitmap->allocPixels();
	newBitmap->eraseARGB( 0, 0, 0, 0 );
	SkCanvas *newSurface = new SkCanvas( *newBitmap );

	if (scaleContent) {
		SkPaint p;
		p.setXfermodeMode( SkXfermode::kSrc_Mode );
		const Size oldSize = getSize();
		SkRect srcRect = SkRect::MakeXYWH( 0, 0, (SkScalar)oldSize.w, (SkScalar)oldSize.h );
		SkRect dstRect = SkRect::MakeXYWH( 0, 0, (SkScalar)size.w, (SkScalar)size.h );
		newSurface->drawBitmapRectToRect( *_bitmap, &srcRect, dstRect, &p );
	}
	DEL( _surface );
	DEL( _bitmap );
	_bitmap = newBitmap;
	_surface = newSurface;
}

SkBitmap *Surface::bitmap() {
	return _bitmap;
}

SkCanvas *Surface::content() {
	return _surface;
}

util::DWORD *Surface::pixels() {
	Size s = getSize();
	int size = s.w * s.h * sizeof(util::DWORD);
	void *src = _bitmap->getPixels();
	util::DWORD *tmp = (util::DWORD*) malloc( size );
	memcpy( tmp, src, size );
	return tmp;
}

}
}
