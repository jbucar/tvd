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
#include "font.h"
#include "glyphrun.h"
#include "../../rect.h"
#include <util/log.h>

namespace canvas {
namespace dummy {

Surface::Surface( canvas::Canvas *canvas, const Rect &rect )
	: canvas::Surface( Point(rect.x,rect.y) ), _canvas(canvas), _size(rect.w, rect.h)
{
	LDEBUG("dummy::Surface", "Create: surface=%p", this);
}

Surface::Surface( canvas::Canvas *canvas, const std::string & /*file*/ )
	: canvas::Surface( Point(0,0) ), _canvas(canvas), _size(720,576)
{
	LDEBUG("dummy::Surface", "Create: surface=%p", this);
}

Surface::Surface( canvas::Canvas *canvas, ImageData *img )
	: canvas::Surface( Point(0,0) ), _canvas(canvas), _size(img->size)
{
	LDEBUG("dummy::Surface", "Create: surface=%p", this);
}

Surface::~Surface()
{
	LDEBUG("dummy::Surface", "Destroy: surface=%p", this);
}

canvas::Canvas *Surface::canvas() const {
	return _canvas;
}

Size Surface::getSize() const {
	return _size;
}

bool Surface::getClip( Rect & /*rect*/ ) {
	return true;
}

bool Surface::setOpacity( util::BYTE /*alpha*/ ) {
	return true;
}

util::BYTE Surface::getOpacity() const{
	return 1;
}

bool Surface::saveAsImage( const std::string & /*file*/ ) {
	return true;
}

void Surface::setClipImpl( const Rect & /*rect*/ ) {
}

void Surface::drawLineImpl( int /*x1*/, int /*y1*/, int /*x2*/, int /*y2*/ ) {
}

void Surface::drawRectImpl( const Rect & /*rect*/ ) {
}

void Surface::fillRectImpl( const Rect & /*rect*/ ) {
}

void Surface::drawRoundRectImpl( const Rect & /*rect*/, int /*arcW*/, int /*arcH*/ ) {
}

void Surface::fillRoundRectImpl( const Rect & /*rect*/, int /*arcW*/, int /*arcH*/ ) {
}

void Surface::drawPolygonImpl( const std::vector<Point>& /*vertices*/, bool /*closed*/ ) {
}

void Surface::fillPolygonImpl( const std::vector<Point>& /*vertices*/ ) {
}

void Surface::drawEllipseImpl( const Point & /*center*/, int /*rw*/, int /*rh*/, int /*angStart*/, int /*angStop*/ ) {
}

void Surface::fillEllipseImpl( const Point & /*center*/, int /*rw*/, int /*rh*/, int /*angStart*/, int /*angStop*/ ) {
}

void Surface::blitImpl( const Point & /*target*/, canvas::Surface * /*srcSurface*/, const Rect & /*source*/ ) {
}

void Surface::scaleImpl( const Rect & /*targetRect*/, canvas::Surface * /*srcSurface*/, const Rect & /*sourceRect*/, bool /*flipw=false*/, bool /*fliph=false*/ ) {
}

canvas::Surface *Surface::rotateImpl( int /*degrees*/ ) {
	return NULL;
}

void Surface::getPixelColorImpl( const Point & /*pos*/, Color &color ) {
	color = Color();
}

void Surface::setPixelColorImpl( const Point & /*pos*/, const Color & /*color*/ ) {
}

bool Surface::comparePixels( const Point &/*pos*/, canvas::Surface* /*image*/ ) {
	return true;
}

util::DWORD Surface::getPixel( const Point & /*pos*/ ) {
	return 0;
}

void Surface::resizeImpl( const Size &/*size*/, bool /*scaleContent=false*/ ) {
}

canvas::GlyphRun *Surface::createGlyphRun( const std::string &/*text*/, const Point &/*pos*/ ) {
	return new GlyphRun();
}

void Surface::renderText( canvas::GlyphRun * /*glyphRun*/, const Point &/*pos*/ ) {
}

}
}
