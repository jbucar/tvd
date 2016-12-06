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

#include "slidebar.h"

#include "window.h"
#include "image.h"
#include "../view.h"
#include "../modules/wgt.h"
#include "../modules/luawgt.h"

#include <boost/make_shared.hpp>

#include <sstream>

namespace wgt{

SlideBar::SlideBar(int x, int y, int w, int h
		, LabelPtr title
		, int minValue
		, int maxValue
		, const colorMap_t &colorMap
		, int bar_w
		, int step
		)
	: Window(x, y, w, h, colorMap)
	, _laction("")
{
	// Margins
	int bar_margin = 3;
	int value_margin = 22;
	int arrow_margin = 6;

	//widths
	int value_label_width = 30;
	int arrow_width = 6;

	_minValue = minValue;
	_maxValue = maxValue;
	_maxW = bar_w - bar_margin * 2;
	_value = _minValue;
	_step = step;
	_title = title;
	addChild(_title);
	
	colorMap_t barColor, barBg;
	barColor = _colorMap;
	barBg = _colorMap;

	barBg["bgColor"] = _colorMap["bgComboColor"];
	barBg["bgLabelColor"] = _colorMap["bgComboColor"];
	barBg["textColor"] = _colorMap["slidebarTextColor"];
	barColor["bgColor"] = _colorMap["bgBarColor"];

	_valueL = LabelPtr(new Label(_title->w() + value_margin, 3, value_label_width, 20, barBg, "", 15, alignment::vCenter|alignment::hCenter, 3));
	addChild(_valueL);
	
	ImagePtr arrow_left = boost::make_shared<Image>(_valueL->x() + _valueL->w() + arrow_margin, 8, arrow_width, 11, colorMap, "arrow_left.png");
	addChild(arrow_left);

	WindowPtr barBackground = boost::make_shared<Window>(arrow_left->x() + arrow_left->w() + arrow_margin, 3, bar_w, 20, barBg);
	addChild(barBackground);
	
	_valueW = boost::make_shared<Window>(bar_margin, 3, _maxW, 14, barColor);
	barBackground->addChild(_valueW);

	ImagePtr arrow_right = boost::make_shared<Image>(barBackground->x() + barBackground->w() + arrow_margin , 8, arrow_width, 11, colorMap, "arrow_right.png");
	addChild(arrow_right);
}

SlideBar::~SlideBar(){
}

void SlideBar::setValue(int value) {
	_value = value;
	int p = (100*(value-_minValue))/(_maxValue-_minValue);
	int w = _maxW * p / 100;
	_valueW->w(w);

	std::string valueStr;
	std::stringstream out;
	out << value;
	valueStr = out.str();
	_valueL->text(valueStr);
	if (_action) {
		(*_action)();
	}
	if (_laction.size()) {
		//View::instance()->call(_laction);
		Wgt::call(_laction);
	}
}

int SlideBar::value(){
	return _value;
}

void SlideBar::action(const std::string& f)
{
	_laction = f;
}

void SlideBar::action(Action f) {
	_action = boost::optional<Action>(f);
}

void SlideBar::activate(bool b, bool /* fromTop */) {
	Window::activate(b);
	if (b) {
		_title->setColor( "textColor", _colorMap["selectedTextColor"] );
		_colorMap["bgColor"] = _colorMap["selectedColor"];
	} else {
		_title->setColor( "textColor", _colorMap["textColor"] );
		_colorMap["bgColor"] = _colorMap["borderColor"];
	}
}

bool SlideBar::onRightArrow(){
	parent()->updatePreviousActiveChild();
	setValue(std::min(_value+_step, _maxValue));
	needsRepaint(true);
	return true;
}

bool SlideBar::onLeftArrow(){
	parent()->updatePreviousActiveChild();
	setValue(std::max(_value-_step, _minValue));
	needsRepaint(true);
	return true;
}

bool SlideBar::onOk(){
	return true;
}

}
