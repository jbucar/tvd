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
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QGlyphRun>
#include <boost/math/special_functions/round.hpp>
#include <boost/algorithm/string.hpp>

#define PAINTER QPainter p(_img);	\
		Color c = getColor();	\
		QColor qc(c.r, c.g, c.b, c.alpha);	\
		p.setPen(qc);	\
		p.setCompositionMode(impl::getQCompositionMode(getCompositionMode())); \
		p.setClipRect( _clip.x, _clip.y, _clip.w, _clip.h );

namespace canvas {
namespace qt {

namespace impl {

static QPainter::CompositionMode getQCompositionMode( composition::mode mode ) {
	switch (mode) {
		case composition::source_over: return QPainter::CompositionMode_SourceOver;
		case composition::source: return QPainter::CompositionMode_Source;
		case composition::clear: return QPainter::CompositionMode_Clear;
		default: throw std::runtime_error("[canvas::qt] composition::mode not suported!!!");
	}
}

}

Surface::Surface( Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) ), _canvas(canvas)
{
	if (rect.w<=0 || rect.h<=0 ) {
		throw std::runtime_error("[canvas::qt] Cannot create surface!!! Invalid bounds");
	}
	QImage *img = new QImage(rect.w, rect.h, QImage::Format_ARGB32_Premultiplied);
	img->fill(0);
	init( img );
}

Surface::Surface( Canvas *canvas, const std::string &file )
	: canvas::Surface( Point(0,0) ), _canvas(canvas)
{
	QImage tmp( file.c_str() );
	init( new QImage( tmp.convertToFormat(QImage::Format_ARGB32_Premultiplied) ) );
}

Surface::Surface( Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) ), _canvas(canvas)
{
	init( new QImage(img->data, img->size.w, img->size.h, QImage::Format_ARGB32_Premultiplied) );
}

void Surface::init( QImage *img ) {
	if (!img) {
		throw std::runtime_error( "[canvas::qt] Cannot create qt image!" );
	}

	if (img->isNull()) {
		throw std::runtime_error( "[canvas::qt] Image is null!" );
	}
	_img = img;
	_alpha = 1;
	_clip = Rect(0,0,_img->width(), _img->height());
}

Surface::~Surface()
{
	DEL(_img);
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

Size Surface::getSize() const {
	const QSize &qs = _img->size();
	Size s( qs.width(), qs.height() );
	return s;
}

bool Surface::getClip( Rect &rect ) {
	rect.x = _clip.x;
	rect.y = _clip.y;
	rect.w = _clip.w;
	rect.h = _clip.h;
	return true;
}

bool Surface::setOpacity( util::BYTE alpha ) {
	_alpha = alpha / 255.0;
	markDirty();
	return true;
}

util::BYTE Surface::getOpacity() const {
	return (util::BYTE) boost::math::iround(_alpha * 255.0);
}

double Surface::getAlpha() {
	return _alpha;
}

bool Surface::saveAsImage( const std::string &file ) {
	return _img->save( file.c_str(), "PNG" );
}

void Surface::setClipImpl( const Rect &rect ) {
	_clip = Rect(rect);
}

void Surface::drawLineImpl( int x1, int y1, int x2, int y2 ) {
	PAINTER
	if (x1==x2 && y1==y2) {
		p.drawPoint(x1, y1);
	} else {
		p.drawLine(x1, y1, x2, y2);
	}
}

void Surface::drawRectImpl( const Rect &rect ) {
	PAINTER;
	p.drawRect( rect.x, rect.y, rect.w-1, rect.h-1 );
}

void Surface::fillRectImpl( const Rect &rect ) {
	PAINTER;
	p.fillRect( rect.x, rect.y, rect.w, rect.h, qc );
}

void Surface::drawRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	PAINTER
	p.drawRoundedRect( rect.x, rect.y, rect.w-1, rect.h-1, arcW, arcH );
}

void Surface::fillRoundRectImpl( const Rect &rect, int arcW, int arcH ) {
	PAINTER
	QPainterPath path;
	path.addRoundedRect( rect.x, rect.y, rect.w-1, rect.h-1, arcW, arcH );
	p.fillPath( path, qc );
}

void Surface::drawPolygonImpl( const std::vector<Point> &vertices, bool closed ) {
	QPolygon pol;
	BOOST_FOREACH( Point p, vertices ) {
		pol.append(QPoint(p.x, p.y));
	}
	PAINTER
	if (closed) {
		p.drawPolygon( pol );
	} else {
		p.drawPolyline( pol );
	}
}

void Surface::fillPolygonImpl( const std::vector<Point> &vertices ) {
	QPolygonF pol;
	BOOST_FOREACH( Point p, vertices ) {
		pol.append(QPointF(p.x, p.y));
	}
	QPainterPath path;
	path.addPolygon(pol);
	PAINTER
	p.drawPolygon( pol );
	p.fillPath( path, qc );
}

void Surface::drawEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	PAINTER
	p.drawEllipse(center.x-rw, center.y-rh, rw*2, rh*2);
}

void Surface::fillEllipseImpl( const Point &center, int rw, int rh, int /*angStart*/, int /*angStop*/ ) {
	QPainterPath path;
	path.addEllipse(center.x - rw, center.y - rh, rw*2, rh*2);
	PAINTER
	p.drawEllipse(center.x - rw, center.y - rh, rw*2, rh*2);
	p.fillPath( path, qc );
}

void Surface::blitImpl( const Point &target, canvas::Surface *srcSurface, const Rect &source ) {
	if (source.w>0 && source.h>0) {
		Surface *sur = dynamic_cast<Surface *>( srcSurface );
		QPoint dstPoint( target.x, target.y );
		QRect srcRect( source.x, source.y, source.w, source.h );

		PAINTER;
		p.setOpacity( sur->getAlpha() );
		p.drawImage( dstPoint, *sur->getImage(), srcRect );
	}
}

void Surface::scaleImpl( const Rect &targetRect, canvas::Surface *srcSurface, const Rect &sourceRect, bool flipw /*=false*/, bool fliph /*=false*/ ) {
	if (sourceRect.w>0 && sourceRect.h>0) {
		Surface *src = dynamic_cast<Surface *>( srcSurface );

		QRect srcRect(sourceRect.x, sourceRect.y, sourceRect.w, sourceRect.h);
		QImage tmp(sourceRect.w, sourceRect.h, QImage::Format_ARGB32_Premultiplied);
		{
			QPainter p(&tmp);
			QPoint dest(0, 0);
			p.drawImage(dest, *(src->getImage()), srcRect);
		}
		QRect dstRect(targetRect.x, targetRect.y, targetRect.w, targetRect.h);
		PAINTER;
		p.setOpacity( src->getAlpha() );
		p.drawImage( dstRect, tmp.mirrored(flipw, fliph) );
	}
}

canvas::Surface *Surface::rotateImpl( int degrees ) {
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
	QImage *img = newSurface->getImage();
	QPainter p(img);
	p.rotate(degrees);
	p.translate(tw,th);
	p.drawImage( 0, 0 , *getImage() );
	p.end();

	return newSurface;
}

void Surface::setColorImpl( Color &color ) {
	if (!color.alpha) {
		color.alpha = 1;
	}
}

void Surface::getPixelColorImpl( const Point &pos, Color &color ) {
	QRgb rgb = _img->pixel( pos.x, pos.y );
	color.alpha = (util::BYTE) qAlpha(rgb);
	color.r = (util::BYTE) boost::math::iround(qRed(rgb) * 255 / (color.alpha ? color.alpha : 1) );
	color.g = (util::BYTE) boost::math::iround(qGreen(rgb) * 255 / (color.alpha ? color.alpha : 1) );
	color.b = (util::BYTE) boost::math::iround(qBlue(rgb) * 255 / (color.alpha ? color.alpha : 1) );
}

void Surface::setPixelColorImpl( const Point &pos, const Color &color ) {
	util::BYTE a = color.alpha ? color.alpha : (util::BYTE) 1;
	QRgb rgb = qRgba( 
		boost::math::iround(color.r * a / 255 ),
		boost::math::iround(color.g * a / 255 ),
		boost::math::iround(color.b * a / 255 ),
		a
	);
	QPoint p( pos.x, pos.y );
	_img->setPixel( p, rgb );
}

canvas::GlyphRun *Surface::createGlyphRun( const std::string &text, const Point &origin ) {
	return new GlyphRun(text.length(), origin);
}

void Surface::renderText( canvas::GlyphRun *glyphRun, const Point &pos ) {
	PAINTER
	GlyphRun *gr = dynamic_cast<GlyphRun*>(glyphRun);
	QGlyphRun qtGR;
	qtGR.setRawFont(*dynamic_cast<const Font*>(font())->qtFont());
	qtGR.setRawData(gr->indexArray(), gr->positionArray(), gr->cant());
	QPointF start(pos.x, pos.y);
	p.drawGlyphRun( start, qtGR );
}

util::DWORD Surface::getPixel( const Point &pos ) {
	return _img->pixel( pos.x, pos.y );
}

QImage *Surface::getImage() {
	return _img;
}

void Surface::resizeImpl( const Size &size, bool scaleContent/*=false*/ ) {
	if (scaleContent) {
		QImage tmp = _img->scaled(size.w, size.h);
		DEL(_img);
		_img = new QImage(tmp);
	} else {
		DEL(_img);
		_img = new QImage(size.w, size.h, QImage::Format_ARGB32_Premultiplied);
		_img->fill(0);
	}
}

}
}
