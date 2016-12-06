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

#include "window.h"

#include "../view.h"
#include "../color.h"

#include <boost/foreach.hpp>

#include <vector>

namespace wgt{

Window::Window(int x, int y, int w, int h, const colorMap_t &colorMap)
	: Widget(x, y, w, h, colorMap)
	, _activeChild(0)
{}

Window::~Window(){}

void Window::draw(View* view){
	view->draw(this);
}

const std::vector<WidgetPtr>& Window::getChildren(){
	return _children;
}

void Window::addChild(WidgetPtr widget, bool canHandleKey)
{
	widget->parent(this);
	_children.push_back(widget);
	if (canHandleKey) {
		_keyHandlingChildren.push_back(widget);
	}
}

/*
void Window::removeChild(Widget* w)
{
	std::vector<Widget*>::iterator ix = std::find(_children.begin(), _children.end(), w);
	_children.erase(ix);
}
*/

const Color Window::bgColor() {
	return _enabled ? _colorMap["bgColor"] : _colorMap["disabledColor"];
}

void Window::removeChildren()
{
	_activeChild = 0;
	_children.clear();
	_keyHandlingChildren.clear();
}

void Window::selectPreviousChild(bool circular)
{
	if (_keyHandlingChildren.size()) {
		if (circular) {
			_activeChild = (_activeChild==0)? _keyHandlingChildren.size()-1 : _activeChild-1;
		} else {
			_activeChild = std::max(0, (int) _activeChild-1);
		}
	}
}

void Window::selectNextChild(bool circular)
{
	if (_keyHandlingChildren.size()) {
		if (circular) {
			_activeChild = ( _activeChild == _keyHandlingChildren.size()-1)? 0 : _activeChild+1;
		} else {
			_activeChild = std::min((int)_keyHandlingChildren.size()-1, (int) _activeChild+1);
		}
	}
}

void Window::selectChild(size_t ix)
{
	if (_keyHandlingChildren.size()) {
		activeChild()->activate(false);
	}
	if (ix < _keyHandlingChildren.size()) {
		_activeChild = ix;
		activeChild()->activate(true);
	}
}

WidgetPtr Window::activeChild()
{
	return _keyHandlingChildren[_activeChild];
}

size_t Window::getActiveIndex() {
	return _activeChild;
}

void Window::deactivateChild(){
	if (_keyHandlingChildren.size() > _activeChild) {
		activeChild()->activate(false);
	}
}

bool Window::processKey(util::key::type key)
{
	if (_keyHandlingChildren.size() && activeChild()->processKey(key)) {
		return true;
	}
	return Widget::processKey(key);
}

void Window::fixOffsets(Widget* window)
{
	if (window) {
		_x += window->x();
		_y += window->y();
	}

	BOOST_FOREACH(WidgetPtr widget, _children) {
		widget->fixOffsets(this);
	}
}

}
