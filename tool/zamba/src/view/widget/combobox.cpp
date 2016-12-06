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

#include "combobox.h"
#include "textarea.h"
#include "list.h"
#include "image.h"
#include "../view.h"
#include "../modules/wgt.h"
#include <util/mcr.h>
#include <util/log.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#define LABEL_W 150

namespace wgt{

ComboBox::ComboBox( int x
		   , int y
		   , int w
		   , int h
		   , const colorMap_t &colorMap
		   , const std::string& title
		   , int titleFontSize
		   , const std::vector<std::string>& items
		   , int itemsFontSize
		   , int itemsW)
	: Window(x, y, w, h, colorMap)
	, _open(false)
	, _selected(0)
	, _lastSelected(0)
	, _initialH(h)
	, _itemsW(itemsW)
	, _itemsFontSize(itemsFontSize)
	, _items(items)
	, _laction("")
	, _maxLines(0)
	, _parentNeedsRepaint(false)
	, _tempItem(LabelPtr(new Label(w - _itemsW + 3, 3, itemsW - 2*3 , h - 2*3, colorMap, "", itemsFontSize, alignment::vCenter|alignment::hLeft, 3)))
	, _tempItemVisible(false)
{
	_title = boost::make_shared<Label>(10, 0, LABEL_W, h, colorMap, title, titleFontSize, alignment::vCenter|alignment::hLeft);
	addChild(_title);
	
	_labelOpen.x = 6;
	_labelOpen.y = 6;
	_labelOpen.h = 6 + (h - 4*3);
	_labelOpen.w = _itemsW - 4*3;

	_labelClosed.x = 3;
	_labelClosed.y = 3;
	_labelClosed.h = h - 2*3;
	_labelClosed.w = _itemsW - 2*3;

	_combo = boost::make_shared<BorderedWindow>(w - _itemsW, 0, _itemsW, h, colorMap, 3);
	_combo->setColor( "bgColor", _colorMap["bgComboColor"] );
	addChild(_combo);
	
	_tempItem->setColor( "bgLabelColor", _colorMap["bgComboColor"] );
	_tempItem->setColor( "textColor", _colorMap["comboTextColor"] );
	_tempItem->setVisible(false);
	addChild(_tempItem);

	_arrowDown = boost::make_shared<Image>(w-20, (h-6) / 2, 11, 6, colorMap, "arrow_down.png");
	_arrowDownSelected = boost::make_shared<Image>(w-20, (h-6) / 2, 11, 6, colorMap, "arrow_down_naranja.png");
	_arrowDownSelected->setVisible(false);
	addChild(_arrowDown);
	addChild(_arrowDownSelected);

	_scrollbar = new Scrollbar();
	_scrollbar->scrollColor(_colorMap["selectedColor"]);
	_scrollbar->bgColor(_colorMap["bgComboColor"]);
	if (items.size() > 0) {
		createItems();
	}
}

ComboBox::~ComboBox()
{
	DEL(_scrollbar);
}

void ComboBox::createItems() {
	_selected = _lastSelected = 0;
	
	//Won't work if updating items while combo is open
	_labels.clear();
	BOOST_FOREACH(std::string item, _items) {
		LabelPtr label = LabelPtr(new Label(3, 3, _itemsW - 2*3 , h() - 2*3, _colorMap, item, _itemsFontSize, alignment::vCenter|alignment::hLeft, 3));
		_labels.push_back(label);
	}
	_combo->addChild(_labels[_selected]);
	_combo->fixOffsets();
	_scrollbar->init( x()+w()-23, y()+3, visibleItemCount(), _labels.size(), _initialH, 0 );
	_scrollbar->initFromTo(_selected, visibleItemCount(), _labels.size());
}

//getters
const std::string &ComboBox::title() {
	return _title->text();
}

const std::vector<std::string> &ComboBox::items() {
	return _items;
}

//setters
void ComboBox::title( const std::string& title ) {
	_title->text(title);
}

void ComboBox::items( const std::vector<std::string> &items ) {
	_items = items;
	if (items.size() > 0) {
		createItems();
	}
}

void ComboBox::close() {
	_combo->setColor( "borderColor", _colorMap["bgColor"] );
	_combo->removeChildren();
	_combo->h(_initialH);
	LabelPtr label = _labels[_selected];
	label->x(_labelClosed.x);
	label->y(_labelClosed.y);
	label->w(_labelClosed.w);
	label->h(_labelClosed.h);
	label->setColor( "textColor", _colorMap["comboTextColor"] );
	label->setColor( "bgLabelColor", _colorMap["bgComboColor"] );
	_combo->addChild(label);
	_combo->fixOffsets();
	_open = false;
	if ( _tempItemVisible ) {
		_tempItem->setVisible(true);
	}
	if (parent()) {
		_parentNeedsRepaint = true;
	}
	needsRepaint(true);
}

int ComboBox::visibleItemCount()
{
	if (_maxLines)
		return std::min(_maxLines, (int)_labels.size());
	else
		return _labels.size();
}

void ComboBox::open(){
	
	if ( _tempItemVisible ) {
		_tempItem->setVisible(false);
	}

	_combo->setColor( "borderColor", _colorMap["selectedColor"] );
	_combo->removeChildren();
	_combo->h(6 + _initialH * visibleItemCount());

	for (unsigned int i = _scrollbar->from(); i <= _scrollbar->to(); ++i) {
		LabelPtr label = _labels[i];
		label->x(_labelOpen.x);
		label->y(_labelOpen.y + (i-_scrollbar->from())*_initialH);
		label->w(_labelOpen.w);
		label->h(_labelOpen.h);
		if (i == _selected) {
			label->setColor( "bgLabelColor", _colorMap["selectedColor"] );
			label->setColor( "textColor", _colorMap["comboSelTextColor"] );
		} else {
			label->setColor( "bgLabelColor", _colorMap["bgComboColor"] );
			label->setColor( "textColor", _colorMap["comboTextColor"] );
		}
		_combo->addChild(label);
	}
	_scrollbar->init( x()+w()-23, y()+3, visibleItemCount(), _labels.size(), _initialH, 0 );
	_scrollbar->update( _selected, visibleItemCount(), _labels.size(), 3 );
	_combo->fixOffsets();
	_open = true;
	needsRepaint(true);
}

int ComboBox::getIndex() {
	return _lastSelected;
}

void ComboBox::setIndex( unsigned int sel ) {
	if (sel < _items.size()) {
		selected( sel, true );
		_lastSelected = sel;
	} else {
		LWARN("combobox", "Index %d isn't valid. The are %u items", sel, _items.size());
	}
}

int ComboBox::selected(){
	return _selected;
}

void ComboBox::selected(unsigned int selected, bool repaint){
	_selected = selected;
	_scrollbar->updateFromTo( _selected, visibleItemCount(), _labels.size() );
	_scrollbar->update( _selected, visibleItemCount(), _labels.size(), 3 );
	if (repaint) {
		_combo->removeChildren();
		for (unsigned int i = _scrollbar->from(); i <= _scrollbar->to(); ++i) {
			LabelPtr label = _labels[i];
			label->x(_labelOpen.x);
			label->y(_labelOpen.y + (i-_scrollbar->from())*_initialH);
			label->w(_labelOpen.w);
			label->h(_labelOpen.h);
			if (i == _selected) {
				label->setColor( "bgLabelColor", _colorMap["bgComboColor"] );
				label->setColor( "textColor", _colorMap["comboSelTextColor"] );
			} else {
				label->setColor( "bgLabelColor", _colorMap["bgComboColor"] );
				label->setColor( "textColor", _colorMap["comboTextColor"] );
			}
			_combo->addChild(label);
		}
	}
	if (_open) {
		open();
	} else {
		close();
	}
}

void ComboBox::action(Action f)
{
	_action = boost::optional<Action>(f);
}

void ComboBox::action(const std::string& f)
{
	_laction = f;
}

void ComboBox::activate(bool b, bool /*fromTop*/) {
	Window::activate(b);
	if (b) {
		_title->setColor( "textColor", _colorMap["selectedTextColor"] );
		_colorMap["bgColor"] = _colorMap["selectedColor"];
		_combo->setColor( "borderColor", _colorMap["selectedColor"] );
	} else {
		_title->setColor( "textColor", _colorMap["textColor"] );
		_colorMap["bgColor"] = _colorMap["borderColor"];
		_combo->setColor( "borderColor", _colorMap["borderColor"] );
	}
	_arrowDown->setVisible(!b);
	_arrowDownSelected->setVisible(b);
}

bool ComboBox::onExit()
{
	if (_open) {
		selected(_lastSelected);
		_lastSelected = _selected;
		close();
		return true;
	}
	return false;
}

bool ComboBox::onOk() {
	if (_open) {
		bool changed = _lastSelected != _selected;
		_tempItemVisible = false;
		close();
		if (changed) {
			_lastSelected = _selected;
			if (_action) {
				(*_action)();
			}
		}
		if (_laction.size()) {
			Wgt::call(_laction, (changed)? 1 : 0);
		}
	} else {
		parent()->updatePreviousActiveChild();
		if (_items.size() > 0) {
			open();
		}
	}
	return true;
}

bool ComboBox::onUpArrow() {
	if (_open) {
		if (_selected > 0) {
			selected(--_selected, true);
			needsRepaint(true);
		}
	}
	return _open;
}

bool ComboBox::onDownArrow() {
	if (_open) {
		if (_selected < _labels.size() - 1) {
			selected(++_selected, true);
			needsRepaint(true);
		}
	}
	return _open;
}

void ComboBox::maxLines(int n)
{
	_maxLines = n;
}

int ComboBox::maxLines()
{
	return _maxLines;
}

void ComboBox::draw( View *view ) {
	_arrowDown->setVisible(!(isOpen()||_active));
	_arrowDownSelected->setVisible(!isOpen() && _active);
	view->draw(this);
	if (_open && _maxLines && _maxLines < (int)_items.size()) {
		view->draw(_scrollbar);
	}
}

bool ComboBox::isOpen() {
	return _open;
}

void ComboBox::needsRepaint( bool flag ) {
	Widget::needsRepaint(flag);
	if (_parentNeedsRepaint) {
		parent()->rawNeedsRepaint(true);
		_parentNeedsRepaint = false;
	}
}

const BorderedWindowPtr &ComboBox::combo() {
	return _combo;
}

void ComboBox::setTempItem( const std::string &item ) {
	_tempItem->text(item);
	_tempItem->setVisible(true);
	_tempItemVisible = true;
}

void ComboBox::hideTempItem() {
	_tempItem->setVisible(false);
	_tempItemVisible = false;
}

void ComboBox::scrollbarArrows( const std::string &up, const std::string &down ) {
	_scrollbar->arrowUp(up);
	_scrollbar->arrowDown(down);
}

}
