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

#include "label.h"

#include "../view.h"

namespace wgt{


Label::Label(int x, int y, int w, int h, const colorMap_t &colorMap, const std::string& text, int fontSize, int alignment, int margin)
	:Widget(x, y, w, h, colorMap){
	_text = text;
	_fontSize = fontSize;
	_alignment = alignment;
	_margin = margin;
	_mask = "";
	_emptyMask = "";
	_textLenght = 0;
}

Label::~Label()
{}

void Label::draw(View* view){
	view->draw(this);
}

//getters
const std::string& Label::text(){
	if (!_mask.empty()) {
		return maskedText();
	}
	return _text;
}
std::string Label::rawText(){
	return _text;
}

int Label::fontSize(){
	return _fontSize;
}

Alignment Label::alignment(){
	return _alignment;
}

int Label::margin(){
	return _margin;
}

//setters
void Label::text(const std::string& text){
	_text = text;
	parent()->needsRepaint(true);
}

void Label::alignment(Alignment align)
{
	_alignment = align;
}

void Label::fontSize(int s)
{
	_fontSize = s;
}

void Label::mask(const std::string& m)
{
	_mask = m;
}

void Label::emptyMask(const std::string& m)
{
	_emptyMask = m;
}

void Label::textLength(size_t length)
{
	_textLenght = length;
}

void Label::reset()
{
	_text = "";
}

const std::string& Label::maskedText()
{
	_masked = "";
	for (size_t i=0; i<_text.size(); ++i) {
		_masked += _mask;
	}

	for (size_t i=_text.size(); i<_textLenght; ++i) {
		_masked += _emptyMask;
	}

	return _masked;
}

}
