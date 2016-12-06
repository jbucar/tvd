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

#include "progressbar.h"
#include "../view.h"
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>

namespace wgt {

ProgressBar::ProgressBar(int x, int y, int w, int h, int barPadding, const colorMap_t &colorMap)
	: Window(x, y, w, h, colorMap)
{
	//	Position and size
	int posY = boost::math::iround((float)y + ((float)h * 0.125f));
	int height = boost::math::iround((float)h * 0.75f);
	int labelWidth = boost::math::iround((float)w * 0.32f);
	int barWidth = boost::math::iround((float)w * 0.53f);
	int valueWidth = w - labelWidth - barWidth;

	colorMap_t tmp = _colorMap;
	tmp["bgColor"] = _colorMap["bgProgressBarColor"];

	_label = LabelPtr(new Label(x, posY, labelWidth, height, colorMap, "", 14, (int) (alignment::vCenter | alignment::hCenter), 0));
	_barWindow = WindowPtr(new Window(_label->x() + _label->w(), posY, barWidth, height, tmp));
	_bar = boost::make_shared <Bar> (_barWindow->x() + barPadding, posY + barPadding, _barWindow->w() - barPadding * 2, height - barPadding * 2, colorMap, 0, 0, 100);
	_valueView = LabelPtr(new Label(_barWindow->x() + _barWindow->w(), posY, valueWidth, height, colorMap, "0", 14, (alignment::vCenter | alignment::hCenter), 0));

	this->addChild(_label);
	this->addChild(_barWindow);
	this->addChild(_bar);
	this->addChild(_valueView);
}

ProgressBar::~ProgressBar()
{
	removeChildren();
}

void ProgressBar::show( int value ) {
	_bar->value(value);
}

void ProgressBar::setText( const std::string &value ) {
	_valueView->text(value);
}

void ProgressBar::setLabel( const std::string &value ) {
	_label->text(value);
}

void ProgressBar::setMinValue( int value ) {
	_bar->minValue(value);
}

void ProgressBar::setMaxValue( int value ) {
	_bar->maxValue(value);
}

void ProgressBar::setFontSize(int size) {
	_label->fontSize(size);
	_valueView->fontSize(size);
}

}
