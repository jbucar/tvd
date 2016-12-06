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
 * dialog.cpp
 *
 *  Created on: May 24, 2011
 *      Author: gonzalo
 */

#include "dialog.h"

#include "../view.h"
#include "../luacallback.h"
#include "../modules/wgt.h"
#include <util/mcr.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <iostream>

namespace wgt
{

Dialog::Dialog( int x, int y, int w, int h, const colorMap_t &colorMap, int borderWidth, int margin )
	: BorderedWindow(x,y,w,h, colorMap, borderWidth)
	, _margin(margin)
	, _titleHeight(40)
	, _title(LabelPtr(new Label(borderWidth, borderWidth, w-2*borderWidth, _titleHeight, colorMap, "", 16, alignment::vCenter|alignment::hCenter, 0)))
	, _text(TextAreaPtr(new TextArea(borderWidth + _margin, _titleHeight, w-2*(borderWidth+_margin), h -_titleHeight - 2*(borderWidth +_margin), colorMap, "", 14, 1, 0, alignment::vCenter|alignment::hLeft)))
	, _icon(ImagePtr())
{
	addChild(_title, false);
	addChild(_text, false);
	setVisible(false);

	Color color = _title->getColor("titleColor");
	titleColor(color);
}

Dialog::~Dialog()
{
}

void Dialog::title(const std::string& t)
{
	_title->text(t);
}

TextAreaPtr Dialog::text()
{
	return _text;
}

void Dialog::iconPath(const std::string& path, int imgw, int imgh, const std::string& pathDisabled)
{
	if (_icon) {
		_icon->path(path);
		if (!pathDisabled.empty()) {
			_icon->dimmedPath(pathDisabled);
		}
	} else {
		_icon = boost::make_shared<Image>(borderWidth()+_margin, _titleHeight + borderWidth()+_margin, imgw, imgh, _colorMap, path);
		_text->x(_text->x() + imgw + _margin);
		_text->w(_text->w() - imgw - _margin);
		addChild(_icon, false);
		_icon->fixOffsets(this);
	}
}

bool Dialog::onExit()
{
	close();
	return true;
}

void Dialog::titleColor(const Color& color)
{
	_title->setColor( "textColor", color );
}

void Dialog::textColor(const Color& color)
{
	_title->setColor( "textColor", color );
}

//----------------------------------------------------------------------------------//
YesNoDialog::YesNoDialog( int x, int y, int w, int h, const colorMap_t &colorMap, int borderWidth, int bw, int bh, int margin )
	: Dialog(x,y, w,h, colorMap, borderWidth, margin)
{
	colorMap_t buttonColor = _colorMap;
	buttonColor["bgColor"] = _colorMap["bgButtonColor"];
	buttonColor["selectedColor"] = _colorMap["selectedButtonColor"];
	buttonColor["textColor"] = _colorMap["buttonTextColor"];
	buttonColor["selectedTextColor"] = _colorMap["buttonTextColor"];

	_yes = boost::make_shared<Button>(0,0, bw, bh, buttonColor);
	_yes->label("SI");
	_yes->label()->fontSize(20);
	_yes->action(boost::bind(&YesNoDialog::close, this));

	_no = boost::make_shared<Button>(bw + 25,0, bw, bh, buttonColor);
	_no->label("NO");
	_no->label()->fontSize(20);
	_no->action(boost::bind(&YesNoDialog::close, this));

	int buttonset_w = _no->x() + _no->w();
	int buttonset_h = _no->h();
	_buttonSet = boost::make_shared<ButtonSet>((w - 2*bw-10)/2, h - buttonset_h - borderWidth - _margin, buttonset_w, buttonset_h, colorMap);
	_buttonSet->addButton(_yes);
	_buttonSet->addButton(_no);
	addChild(_buttonSet);

	_text->h(_buttonSet->y() - _text->y());

	fixOffsets();
}

void YesNoDialog::buttonColor(const Color& color, const Color& activeColor)
{
	_yes->setColor("bgColor", color);
	_yes->setColor("selectedColor", activeColor);
	_no->setColor("bgColor", color);
	_no->setColor("selectedColor", activeColor);
}

void YesNoDialog::buttonTextColor(const Color& color, const Color& activeColor)
{
	_yes->setColor("textColor", color);
	_yes->setColor("selectedTextColor", activeColor);
	_no->setColor("textColor", color);
	_no->setColor("selectedTextColor", activeColor);
}

void YesNoDialog::closeFirst(Button::Action f)
{
	close();
	f();
}

void YesNoDialog::yesAction(Button::Action fYes)
{
	_yes->action(boost::bind(&YesNoDialog::closeFirst, this, fYes));
}

void YesNoDialog::yesLabel( const std::string &label, int fontSize /*=-1*/ ) {
	_yes->label(label);
	if (fontSize > 0) {
		_yes->label()->fontSize(fontSize);
	}
}

void YesNoDialog::noLabel( const std::string &label, int fontSize /*=-1*/ ) {
	_no->label(label);
	if (fontSize > 0) {
		_no->label()->fontSize(fontSize);
	}
}

void YesNoDialog::noAction(Button::Action fNo)
{
	_no->action(boost::bind(&YesNoDialog::closeFirst, this, fNo));
}

bool YesNoDialog::onExit()
{
	Dialog::onExit();
	_no->execute(); // cancel
	return true;
}

void YesNoDialog::open()
{
	_buttonSet->selectChild(1); // button 'NO'
	Dialog::open();
}

//----------------------------------------------------------------------------------//

} // end namespace
