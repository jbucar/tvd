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

#include "scrollarrow.h"
#include "../view.h"
#include <util/log.h>
#include <util/mcr.h>
#include <algorithm>

namespace wgt {

ScrollArrow::ScrollArrow( const std::string &arrowLeft, const std::string &arrowRight, int x, int y, int w, int h )
	: _arrowLeft(arrowLeft), _arrowRight(arrowRight), _x(x), _y(y), _w(w), _h(h), _canScrollRight(true), _canScrollLeft(true)
{
}

ScrollArrow::~ScrollArrow()
{
}

void ScrollArrow::x( int x ) {
	_x = x;
}

void ScrollArrow::y( int y ) {
	_y = y;
}

int ScrollArrow::x() {
	return _x;
}

int ScrollArrow::y() {
	return _y;
}

int ScrollArrow::w() {
	return _w;
}

int ScrollArrow::h() {
	return _h;
}

void ScrollArrow::canScrollRight( bool value ) {
	_canScrollRight = value;
}

void ScrollArrow::canScrollLeft( bool value ) {
	_canScrollLeft = value;
}

void ScrollArrow::arrowLeft( const std::string &filename ) {
	_arrowLeft = filename;
}

const std::string &ScrollArrow::arrowRight() const {
	return _canScrollRight ? _arrowRight : _arrowRight_deactived;
}

void ScrollArrow::deactiveImgs( const std::string &right_filename, const std::string &left_filename ) {
	_arrowRight_deactived = right_filename;
	_arrowLeft_deactived = left_filename;
}

void ScrollArrow::arrowRight( const std::string &filename ) {
	_arrowRight = filename;
}

const std::string &ScrollArrow::arrowLeft() const {
	return _canScrollLeft ? _arrowLeft : _arrowLeft_deactived;
}

Scroll::Scroll( int x, int y, int w, int h, int sep, const colorMap_t &colorMap, ButtonSetPtr wgt )
	: WidgetSet(x, y, w, h, colorMap), _separation(sep), _limit(0), _scrollee(wgt)
{
	addChild(wgt, false);

	int s = 10;
	_view = new ScrollArrow("h_arrow_left.png", "h_arrow_right.png", x + s, y, w - s * 2, h);
	_view->deactiveImgs("h_arrow_right_deactive.png", "h_arrow_left_deactive.png");
}

Scroll::~Scroll()
{
	DEL(_view);
}

const Color Scroll::bgColor() {
	return _enabled ? _colorMap["bgColor"] : _colorMap["disabledColor"];
}

unsigned int Scroll::limit() {
	return _limit;
}

void Scroll::init( int limit/*=-1*/ ) {
	_limit = (limit > -1) ? limit : _scrollee->getChildren().size();
	std::vector<WidgetPtr> ws = _scrollee->getChildren();
	_btns.clear();
	for(unsigned int i = 0; i < _limit; i++) {
		_btns.push_back(ws[i]);
	}
	adjust();
}

void Scroll::selectChild( size_t ix ) {
	_scrollee->selectChild(ix);
}

void Scroll::fixOffsets( Widget *w ) {
	WidgetSet::fixOffsets(w);
	_view->x(x() + 10);
	_view->y(y());
}

void Scroll::adjust() {
	int acu = 0;
	for(unsigned int i = 0; i < _limit; i++) {
		acu += _btns[i]->w();
		acu += _separation;
	}
	acu -= _separation;
	int offset = 30 + _scrollee->x() + (_scrollee->w() - acu) / 2;

	for(unsigned int i = 0; i < _limit; i++) {
		_btns[i]->x(offset);
		_btns[i]->needsRepaint(true);
		offset += _btns[i]->w() + _separation;
	}
	needsRepaint(true);
}

void Scroll::draw( View* view ) {
	view->draw(this);
}

ScrollArrow *Scroll::view() {
	return _view;
}

bool Scroll::onLeftArrow() {
	_scrollee->processKey(util::key::cursor_left);

	std::vector<WidgetPtr>::iterator it = std::find(_btns.begin(), _btns.end(), _scrollee->activeChild());

	if (it == _btns.end()) {
		WidgetPtr ptr = _scrollee->activeChild();
		_btns.insert(_btns.begin(), ptr);
		_btns.erase(_btns.end());
		adjust();
	}

	return true;
}

const std::vector<WidgetPtr>& Scroll::getChildren() {
	return _btns;
}

bool Scroll::onRightArrow() {
	_scrollee->processKey(util::key::cursor_right);

	std::vector<WidgetPtr>::iterator it = std::find(_btns.begin(), _btns.end(), _scrollee->activeChild());

	if (it == _btns.end()) {
		WidgetPtr ptr = _scrollee->activeChild();
		_btns.erase(_btns.begin());
		_btns.push_back(ptr);
		adjust();
	}

	return true;
}

}
