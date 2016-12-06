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
 * button.cpp
 *
 *  Created on: May 19, 2011
 *      Author: gonzalo
 */

#include "button.h"
#include "widget.h"

#include "../view.h"
#include "../modules/wgt.h"
#include <luaz/lua.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/make_shared.hpp>

namespace wgt
{

Button::Button(int x, int y, int w, int h, const colorMap_t &colorMap)
	: Window(x, y, w, h, colorMap)
	, _label(boost::make_shared<Label>(0, 0, w, h, colorMap, "", 14, alignment::vCenter|alignment::hCenter))
	, _icon(ImagePtr())
	, _normalIconPath("")
	, _activeIconPath("")
	, _disableIconPath("")
	, _iconH(0)
	, _separation(0)
	, _callback(NULL)
	, _selectedCbk(NULL)
{
	addChild(_label);
	_originalX = x;
	_originalY = y;
}

Button::~Button() {}

void Button::draw(View* view) {
	if (_icon) {
		_icon->path(iconPath());
	}
	_label->setColor( "textColor", textColor());
	view->draw(this);
}

void Button::activate(bool b, bool /* fromTop */) {
	if (_enabled) {

		_label->setColor( "textColor", textColor() );
		if (_icon) {
			_icon->path(iconPath());
		}

		if (_selectedCbk && (b != _active) ) {
			_selectedCbk(b);
		}
		Widget::activate(b);
	}
}

void Button::label(const std::string& t)
{
	_label->text(t);
}

ImagePtr Button::icon()
{
	return _icon;
}

int Button::iconH()
{
	return _iconH;
}

int Button::separation()
{
	return _separation;
}

void Button::icon(const std::string& path, const std::string& activePath, const std::string disablePath, int imgw, int imgh, int iconH, int separation) {
	_disableIconPath = disablePath;
	icon(path, activePath, imgw, imgh, iconH, separation);	
}

void Button::icon(const std::string& path, const std::string& activePath, int imgw, int imgh, int iconH, int separation)
{
	_normalIconPath = path;
	_activeIconPath = activePath;
	_iconH = iconH;
	_separation = separation;

	if (_icon) {
		_icon->path(iconPath());
	} else {
		_icon = boost::make_shared<Image>((w()-imgw)/2, (iconH-imgh)/2, imgw, imgh, _colorMap, iconPath());
		_label->h(h() - iconH - separation);
		_label->y(iconH + separation);
		addChild(_icon);
	}
	_icon->dimmedPath(_normalIconPath);
}

const std::string& Button::iconPath() {
	if (!_enabled && !_disableIconPath.empty()) {
		return _disableIconPath;
	} else if (_active && !_activeIconPath.empty()) {
		return _activeIconPath;
	} else {
		return _normalIconPath;
	}

}

const Color Button::bgColor()
{
	if (!_enabled && (_colorMap["disabledColor"] != Color::None)) {
		return _colorMap["disabledColor"];
	}
	return (_active) ? _colorMap["selectedButtonColor"] : _colorMap["bgButtonColor"];
}

const Color Button::textColor()
{
	if (!_enabled) {
		return _colorMap["disabledTextColor"];
	} else if (_active) {
		return _colorMap["selectedTextColor"];
	} else {
		return _colorMap["textColor"];
	}
}

void Button::action( Action callback )
{
	_callback = callback;
}

void Button::x( int x ) {
	Widget::x(x);
	_icon->x(x);
	_label->x(x);
}

void Button::execute()
{
	if (_callback) {
		_callback();
	}
}

bool Button::onOk() {
	if (isEnabled()) {
		execute();
	}
	return true;
}

LabelPtr Button::label() {
	return _label;
}

void Button::selectedCallback( SelectedAction callback ) {
	_selectedCbk = callback;
}

void Button::restorePositions() {
	x( _originalX );
	y( _originalY );
	_icon->x( (w()- _icon->w() )/2 );
	_icon->y( 0 );
	_label->x( 0 );
	_label->y( y() + _iconH + _separation );
}

}
