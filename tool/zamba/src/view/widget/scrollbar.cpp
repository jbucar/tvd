/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "scrollbar.h"

namespace wgt {

Scrollbar::Scrollbar()
	: _from(0), _to(0)
{
}

Scrollbar::~Scrollbar()
{
}

void Scrollbar::init( int x, int y, int visible, int count, int rowHeight, int rowSep ) {
	scroll.margin = 3;
	scroll.x = x;
	scroll.y = y;
	scroll.h = (rowHeight + rowSep)*visible - rowSep;
	scroll.w = 20;

	int visible_p  = (count>visible) ? 100 * visible / count : 100;
	handle.x = scroll.x + scroll.margin;
	handle.w = scroll.w - 2*scroll.margin;
	handle.h = (scroll.h - 2*scroll.margin)*visible_p/100;
}

void Scrollbar::update( int current, int visible, int count, int margins ) {
	if (current < 0) {
		current = 0;
	}
	int visible_xp = (count > visible)? 100*current / (count-1) : 100;
	handle.y = scroll.y + scroll.margin + (scroll.h - margins*scroll.margin - handle.h)*visible_xp/100;
}

unsigned int Scrollbar::from() {
	return _from;
}

unsigned int Scrollbar::to() {
	return _to;
}

void Scrollbar::updateFromTo( int current, int visible, int count ) {
	if (current >= 0) {
		if (_from + visible <= current) {
			const int d = std::max(0,current-_to);
			_from += d;
			_to = std::min(_to + d, count);
		} else if (_to - visible >= current) {
			const int d = std::max(0,_from-current);
			_from = std::max(0, _from - d);
			_to -= d;
		}
	}
}

void Scrollbar::initFromTo( int current, int visible, int count ) {
	_to   = std::min(count, visible) - 1;
	_from = std::max(0, _to - visible + 1);
	updateFromTo(current, visible, count);
}

void Scrollbar::arrowUp( const std::string &filename ) {
	_arrowUp = filename;
}

const std::string &Scrollbar::arrowUp() const {
	return _arrowUp;
}

void Scrollbar::arrowDown( const std::string &filename ) {
	_arrowDown = filename;
}

const std::string &Scrollbar::arrowDown() const {
	return _arrowDown;
}

const Color &Scrollbar::scrollColor() const {
	return _scrollColor;
}

void Scrollbar::scrollColor( const Color &color ) {
	_scrollColor = color;
}

const Color &Scrollbar::bgColor() const {
	return _bgColor;
}

void Scrollbar::bgColor( const Color &color ) {
	_bgColor = color;
}

}
