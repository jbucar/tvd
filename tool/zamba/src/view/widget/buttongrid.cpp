/*****************************************************************************

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

#include "buttongrid.h"
#include "../view.h"
#include <util/keydefs.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/make_shared.hpp>
#include <boost/concept_check.hpp>
#include <boost/foreach.hpp>

namespace wgt {

ButtonGrid::ButtonGrid(int x, int y, int w, int h, unsigned int rowsVisible, unsigned int rowH, unsigned int rowSeparation, unsigned int rows, const colorMap_t &colorMap)
	: Window(x,y,w,h,colorMap)
{
	_row = 0;
	_rows = rows;
	_rowH = rowH;
	_rowSeparation = rowSeparation;
	_colorMap = colorMap;
	_hElements = 0;
	_visible = rowsVisible;
	
	for( unsigned int i=0; i < rows; i++) {
		addRow( 0, (i * ( _rowSeparation + _rowH ) ), w, _rowH );
	}

	_scrollbar = new Scrollbar();
	_scrollbar->scrollColor(_colorMap["scrollColor"]);
	_scrollbar->bgColor(_colorMap["scrollBgColor"]);
	_scrollbar->initFromTo( _row, _visible, _hElements );
	_scrollbar->init( x+w, y+5, _visible, _hElements, _rowH, _rowSeparation );
}

ButtonGrid::~ButtonGrid()
{
	DEL(_scrollbar);
}

void ButtonGrid::draw( View *view ) {
	updateFromTo();
	updateScrollPosition();
	view->draw(this);
}

void ButtonGrid::addRow( int x, int y, int w, int h ) {
	ButtonSetPtr bt = boost::make_shared<ButtonSet>(x, y, w, h, _colorMap);
	LTRACE("ButtonGrid", "pos x: %d y: %d w: %d h: %d helments: %d", x, y, w, h, _hElements);
	bt->circular(true);
	addChild(bt, false);
	_buttons.push_back( bt );
	_hElements++;
}

void ButtonGrid::appendRow( unsigned int offset /*= 0*/ ) {
	addRow( 0, offset + _rows * ( _rowSeparation + _rowH ), w(), _rowH );
	_rows++;
	(_buttons[ _hElements -1 ])->fixOffsets(this);

	_scrollbar->initFromTo( _row, _visible, _hElements );
	_scrollbar->init( x()+w(), y()+5, _visible, _hElements, _rowH, _rowSeparation );
}

void ButtonGrid::updateScrollPosition() {
	_scrollbar->update( _row, _visible, _hElements, 2 );
}

void ButtonGrid::updateFromTo() {
	_scrollbar->updateFromTo( _row, _visible, _hElements );
}

void ButtonGrid::selectRow(unsigned int row)
{
	if (row < _hElements) {
		_buttons[_row]->setCurrent(-1);
		_row = row;
	} else {
		LWARN("ButtonGrid", "Selected row: %d is invalid", row);
	}
	needsRepaint( true );
}

bool ButtonGrid::onLeftArrow() {
	LTRACE("ButtonGrid", "On left arrow. Current row = %d", _row );
	ButtonSetPtr bt = _buttons[_row];
	return bt->processKey(util::key::cursor_left);
}

bool ButtonGrid::onUpArrow() {
	ButtonSetPtr currentBt = _buttons[_row];
	
	unsigned int currentPos = currentBt->currentPos();

	do {
		_row = ( _row == 0 ) ? _hElements - 1 : _row - 1;
	} while ( _buttons[_row]->getChildren().size() <= currentPos );
	
	updateFromTo();
	updateScrollPosition();

	LTRACE("ButtonGrid", "On up arrow. Current row = %d and current position = %d", _row, currentPos );
	currentBt->setCurrent( -1 );
	ButtonSetPtr nextBt = _buttons[_row];
	return nextBt->setCurrent( currentPos );
}

bool ButtonGrid::onRightArrow() {
	LDEBUG("ButtonGrid", "On right arrow. Current row = %d", _row );
	ButtonSetPtr bt = _buttons[_row];
	return bt->processKey(util::key::cursor_right);
}

bool ButtonGrid::onDownArrow() {
	ButtonSetPtr currentBt = _buttons[_row];
	unsigned int currentPos = currentBt->currentPos();

	do {
		_row = (  _row == ( _hElements - 1 ) ) ? 0 : _row + 1;
	} while ( _buttons[_row]->getChildren().size() <= currentPos );

	updateFromTo();
	updateScrollPosition();
	
	LTRACE("ButtonGrid", "On down arrow. Current row = %d and current position = %d", _row, currentPos );
	currentBt->setCurrent( -1 );
	ButtonSetPtr nextBt = _buttons[_row];
	return nextBt->setCurrent( currentPos );
}

bool ButtonGrid::selectButton( int pos ) {
	ButtonSetPtr currentBt = _buttons[_row];
	if (pos < 0) {
		currentBt->deactivateChild();
		return true;
	} else {
		return currentBt->setCurrent( pos );
	}
}

bool ButtonGrid::onOk() {
	LTRACE("ButtonGrid", "On ok");
	ButtonSetPtr bt = _buttons[_row];
	return bt->processKey(util::key::ok);
}

void ButtonGrid::addButton( ButtonPtr b, unsigned int row ) {
	LTRACE("ButtonGrid", "Add button in row = %d", row);
	if( row <= _buttons.size() -1 ) {
		_buttons[row]->addButton(b);
	} else {
		LWARN("ButtonGrid", "Trying to add button in row: %d but it only has %d rows", row + 1, _buttons.size());
	}
}

const std::string &ButtonGrid::currentHelp() {
	ButtonSetPtr bt = _buttons[_row];
	return bt->help();
}

void ButtonGrid::fixOffsets( Widget *window ) {
	if (window) {
		_x += window->x();
		_y += window->y();
		for(unsigned int i=0; i < _hElements; i++) {
			_buttons[i]->fixOffsets(this);
		}
		_scrollbar->init( x()+w(), y()+5, _visible, _hElements, _rowH, _rowSeparation );
		_scrollbar->initFromTo( _row, _visible, _hElements );
	} else {
		for(unsigned int i=0; i < _hElements; i++) {
			_buttons[i]->needsRepaint(true);
			BOOST_FOREACH(WidgetPtr widget, _buttons[i]->getChildren()) {
				LTRACE("ButtonGrid", "Fix offset. Bt x: %d y: %d", _buttons[i]->x(), _buttons[i]->y());
				widget->fixOffsets(_buttons[i].get());
				LTRACE("ButtonGrid", "Fix offset. button x: %d y: %d", widget->x(), widget->y());
			}
		}
	}
}

int ButtonGrid::activeRow() {
	return _row;
}

int ButtonGrid::activeColumn() {
	return _buttons[_row]->getActiveIndex();
}

void ButtonGrid::scrollbarArrows( const std::string &up, const std::string &down ) {
	_scrollbar->arrowUp(up);
	_scrollbar->arrowDown(down);
}

}
