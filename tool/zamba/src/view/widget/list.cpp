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

/*
 * List.cpp
 *
 *  Created on: May 10, 2011
 *      Author: gonzalo
 */

#include "list.h"
#include "../view.h"
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <iostream>

namespace wgt {

List::List(int x, int y, int w, int h, int rowH, int rowSep, const colorMap_t &colorMap, int fontSize)
	: Widget(x, y, w, h, colorMap)
	, _current(-1)
	, _visible(0)
	, _updateScrollbar(true)
	, _autoscroll(false)
	, _rowHeight(rowH)
	, _rowSep(rowSep)
	, _fontSize(fontSize)
{
	_scrollbar = new Scrollbar();
	_scrollbar->scrollColor(getColor("scrollColor"));
	_scrollbar->bgColor(getColor("scrollBgColor"));
}

List::~List()
{
	DEL(_scrollbar);
}

const std::vector<Row> &List::contents() {
	return _contents;
}

int List::current() {
	return _current;
}

int List::visible() {
	return _visible;
}

int List::rowHeight() {
	return _rowHeight;
}

int List::rowSep() {
	return _rowSep;
}

int List::fontSize() {
	return _fontSize;
}

void List::setUpdateScrollbar( bool update ) {
	_updateScrollbar = update;
}

void List::contents( const std::vector<Row> &rows, const std::vector<std::string> &values ) {
	_contents.clear();
	_contents.assign(rows.begin(), rows.end());
	_values.clear();
	_values.assign(values.begin(), values.end());
	_scrollbar->initFromTo(_current, _visible, _contents.size());
	_scrollbar->init( x()+w(), y()+2+_rowHeight+_rowSep, _visible, _contents.size(), _rowHeight, _rowSep );
	needsRepaint(true);
}

void List::updateRow( size_t index, const Row& row, const std::string& value ) {
	if (index < _contents.size()) {
		_contents[index] = row;
		_values[index] = value;
	}
}

void List::addRow( const Row &row, const std::string &value ) {
	_contents.push_back(row);
	_values.push_back(value);

	_scrollbar->initFromTo( _current, _visible, _contents.size() );
	_scrollbar->init( x()+w(), y()+2+_rowHeight+_rowSep, _visible, _contents.size(), _rowHeight, _rowSep );
}

void List::selected( int n ) {
	_current = n;
}

int List::selected() {
	return _current;
}

void List::setVisibleRows( int n ) {
	_visible = n;
}

void List::addImage( const std::string &key, const std::string &value ) {
	_images[key] = value;
}

ColumnMapping &List::images_normal() {
	return _mappings_normal;
}

ColumnMapping &List::images_selected() {
	return _mappings_selected;
}

void List::cellWidths( const std::vector<int> &widths ) {
	_cellWidths.clear();
	_cellWidths.insert(_cellWidths.begin(), widths.begin(), widths.end());

	int listW = 0;
	BOOST_FOREACH(int width, _cellWidths) {
		listW += width + _rowSep;
	}
	w(listW);
}

const std::vector<std::string> &List::cellHeaders() {
	return _headers;
}

void List::cellHeaders( const std::vector<std::string>& headers ) {
	_headers = headers;
}

void List::cellAlignments( const std::vector<Alignment> &alignments ) {
	_alignments = alignments;
}

const std::vector<Alignment> &List::cellAlignments() {
	return _alignments;
}

const std::vector<int> &List::cellWidths() {
	return _cellWidths;
}

void List::updateScrollPosition() {
	if (_updateScrollbar) {
		int i = _autoscroll && _current < 0 ? _contents.size() - 1 : _current;
		_scrollbar->update(i, _visible, _contents.size(), 2);
	}
}

void List::autoscroll( bool autoscroll ) {
	_autoscroll = autoscroll;
}

void List::updateFromTo() {
	int i = _autoscroll && _current < 0 ? _contents.size() - 1 : _current;
	_scrollbar->updateFromTo(i, _visible, (int)_contents.size() );
}

void List::moveOneUp() {
	if (_contents.size()) {
		_current = (_current>0) ? _current-1 : _contents.size()-1;
		updateFromTo();
		updateScrollPosition();
	}
}

void List::moveOneDown() {
	if (_contents.size()) {
		_current++;
		if (_current>=(int)_contents.size()) {
			_current=0;
		}
		updateFromTo();
		updateScrollPosition();
	}
}

bool List::onUpArrow() {
	const int oldCurrent = _current;
	moveOneUp();
	if (oldCurrent != _current) {
		needsRepaint(true);
	}
	return true;
}

bool List::onDownArrow() {
	const int oldCurrent = _current;
	moveOneDown();
	if (oldCurrent != _current) {
		needsRepaint(true);
	}
	return true;
}

bool List::onPageUp() {
	if (_contents.size()) {
		const int oldCurrent = _current;
		_current-=_visible;
		if (_current<0) {
			_current = 0;
		}
		updateFromTo();
		updateScrollPosition();
		if (oldCurrent != _current) {
			needsRepaint(true);
		}
	}
	return true;
}

bool List::onPageDown() {
	if (_contents.size()) {
		const int oldCurrent = _current;
		_current+=_visible;
		if (_current>=(int)_contents.size()) {
			_current = _contents.size() - 1;
		}
		updateFromTo();
		updateScrollPosition();
		if (oldCurrent != _current) {
			needsRepaint(true);
		}
	}
	return true;
}

Scrollbar *List::scrollBar() {
	return _scrollbar;
}

void List::draw( View* view ) {
	updateFromTo();
	updateScrollPosition();
	view->draw(this);
}

void List::addColumnMapping( size_t column, const std::string &valueFrom, const std::string &valueToN, const std::string &valueToS ) {
	_mappings_normal[column][valueFrom]   = valueToN;
	_mappings_selected[column][valueFrom] = valueToS;
}

const std::string &List::value() {
	return _values[_current];
}

void List::fixOffsets(Widget* window) {
	Widget::fixOffsets(window);
	_scrollbar->init( x()+w(), y()+2+_rowHeight+_rowSep, _visible, _contents.size(), _rowHeight, _rowSep );
}

void List::scrollbarArrows( const std::string &up, const std::string &down ) {
	_scrollbar->arrowUp(up);
	_scrollbar->arrowDown(down);
}

int List::from() const {
	return _scrollbar->from();
}

int List::to() const {
	return _scrollbar->to();
}

} // end namespace
