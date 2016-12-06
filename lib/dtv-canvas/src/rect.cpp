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

#include "rect.h"
#include "point.h"
#include "size.h"
#include <util/assert.h>
#include <algorithm>

namespace canvas {

Rect::Rect( void )
{
	x = 0;
	y = 0;
	w = 0;
	h = 0;
}

Rect::Rect( int paramX, int paramY, int paramW, int paramH )
{
	x = paramX;
	y = paramY;
	w = paramW;
	h = paramH;
}

Rect::Rect( const Point &point, const Size &size )
{
	x = point.x;
	y = point.y;
	w = size.w;
	h = size.h;
}

Rect::Rect( int px, int py, const Size &size )
{
	x = px;
	y = py;
	w = size.w;
	h = size.h;
}

Rect::Rect( const Point &point, int pw, int ph )
{
	x = point.x;
	y = point.y;
	w = pw;
	h = ph;
}

Rect::~Rect( void )
{
}

bool Rect::operator==(const Rect& rect) const {
	return x == rect.x && y == rect.y && w == rect.w && h == rect.h;
}

bool Rect::operator!=(const Rect& rect) const {
	return !((*this)==rect);
}

Rect &Rect::operator=( const Rect &rect ) {
	x = rect.x;
	y = rect.y;
	w = rect.w;
	h = rect.h;
	return *this;
}

Rect &Rect::operator=( const Point &point ) {
	x = point.x;
	y = point.y;
	return *this;
}

Rect &Rect::operator=( const Size &size ) {
	w = size.w;
	h = size.h;
	return *this;
}

bool Rect::isEmpty() const {
	return !w || !h;
}

bool Rect::includes( const Rect &rect ) const {
	bool check;

	check = x <= rect.x;
	check &= rect.x + rect.w <= x+w;
	check &= y <= rect.y;
	check &= rect.y + rect.h <= y+h;

	return check;
}

bool Rect::intersects( const Rect& rect ) const {
	bool check;
	
	check = rect.x > x + w - 1;
	check |= x > rect.x + rect.w - 1;
	check |= rect.y > y + h - 1;
	check |= y > rect.y + rect.h - 1;

	return !check;
}

bool Rect::intersection( const Rect &rect, Rect &intersection ) const {
	intersection.x = std::max( x, rect.x );
	intersection.y = std::max( y, rect.y );
	intersection.w = std::max(std::min( x+w, rect.x+rect.w ) - intersection.x, 0);
	intersection.h = std::max(std::min( y+h, rect.y+rect.h ) - intersection.y, 0);
	return intersection.w>0 && intersection.h>0;
}

bool Rect::abuts( const Rect &rect, float percentage/*=0*/ ) const {
	bool check_top    = false;
	bool check_bottom = false;
	bool check_left   = false;
	bool check_right  = false;
	float height_s = 0.0;
	float width_s = 0.0;

	check_top    =   y == rect.y+rect.h;
	check_bottom = y+h == rect.y;
	width_s = (w > rect.w) ? (float)rect.w / (float)w : (float)w / (float)rect.w;

	check_left  = x+w == rect.x;
	check_right =   x == rect.x+rect.w;
	height_s = (h > rect.h) ? (float)rect.h / (float)h : (float)h / (float)rect.h;

	return ((check_top || check_bottom) && width_s >= percentage) || ((check_left || check_right) && height_s >= percentage) ;
}

Rect Rect::encompass( const Rect &rect) const {
	int min_x = std::min( x, rect.x );
	int min_y = std::min( y, rect.y );
	int max_x = std::max( x+w, rect.x+rect.w );
	int max_y = std::max( y+h, rect.y+rect.h );
	return Rect( min_x, min_y, max_x-min_x, max_y-min_y );
}

int Rect::area() const {
	return w*h;
}

bool Rect::aside( const Rect& rect ) const {
	bool check = false;
	
	check |= (x==rect.x && x+w<rect.x+rect.w && y==rect.y && y+h==rect.y+rect.h);
	check |= (x>rect.x && x+w==rect.x+rect.w && y==rect.y && y+h==rect.y+rect.h);
	check |= (x==rect.x && x+w==rect.x+rect.w && y==rect.y && y+h<rect.y+rect.h);
	check |= (x==rect.x && x+w==rect.x+rect.w && y>rect.y && y+h==rect.y+rect.h);

	return check;
}

Rect Rect::cut( const Rect& rect ) const {
	DTV_ASSERT(rect.aside(*this));
	int newx, newy, neww, newh;

	newx = (rect.x+rect.w==x+w) ? x : rect.x+rect.w;
	newy = (rect.y+rect.h==y+h) ? y : rect.y+rect.h;
	neww = (rect.w==w) ? w : w-rect.w;
	newh = (rect.h==h) ? h : h-rect.h;

	return Rect(newx, newy, neww, newh);
}

std::vector<Rect> Rect::divide( const Rect& rect ) const {
	DTV_ASSERT(goesThrough(rect));
	std::vector<Rect> parts;

	if (x<rect.x && x+w>rect.x+rect.w) { // Divide verticaly
		parts.push_back(Rect(x,y,rect.x-x,h));
		parts.push_back(Rect(rect.x+rect.w,y,x+w-rect.x-rect.w,h));
	} else { // Divide horizontaly
		parts.push_back(Rect(x,y,w,rect.y-y));
		parts.push_back(Rect(x,rect.y+rect.h,w,y+h-rect.y-rect.h));
	}

	return parts;
}

bool Rect::goesInto( const Rect& rect ) const {
	bool check = false;
	check |= (x>=rect.x && x+w<=rect.x+rect.w && (y>=rect.y || y+h<=rect.y+rect.h)); // Verticaly
	check |= (y>=rect.y && y+h<=rect.y+rect.h && (x>=rect.x || x+w<=rect.x+rect.w)); // Horizontaly
	return check;
}

bool Rect::goesThrough( const Rect& rect ) const {
	bool check = false;
	check |= (x>rect.x && x+w<rect.x+rect.w && y<rect.y && y+h>rect.y+rect.h); // Verticaly
	check |= (x<rect.x && x+w>rect.x+rect.w && y>rect.y && y+h<rect.y+rect.h); // Horizontaly
	return check;
}

}
