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

#include "textarea.h"

#include "../view.h"

namespace wgt {

TextArea::TextArea(int x, int y, int w, int h, const colorMap_t &colorMap, const std::string& text, int fontSize, int lines, int margin, int alignment, int borderWidth)
	: BorderedWindow(x, y, w, h, colorMap, borderWidth)
	, _lines     (lines)
	, _fontSize  (fontSize)
	, _text      (text)
	, _margin    (margin)
	, _alignment (alignment)
	, _leading   (0)
	, _fitToText(false)
{
}

TextArea::~TextArea(){
}

void TextArea::draw(View* view){
	view->draw(this);
}

void TextArea::text(const std::string& text){
	_text = text;
}

void TextArea::append(const std::string& text){
	_text += text;
}

void TextArea::clearText(){
	_text = "";
}

//setters
void TextArea::lines(int lines){
	_lines = lines;
}

void TextArea::fontSize(int fontSize){
	_fontSize = fontSize;
}

void TextArea::margin(int margin){
	_margin = margin;
}

void TextArea::alignment(int alignment){
	_alignment = alignment;
}

//getters
const std::string& TextArea::text(){
	return _text;
}

int TextArea::lines(){
	return _lines;
}

int TextArea::fontSize(){
	return _fontSize;
}

int TextArea::margin(){
 	return _margin;
}

int TextArea::alignment(){
 	return _alignment;
}

int TextArea::leading()
{
	return _leading;
}

bool TextArea::fitToText()
{
	return _fitToText;
}

void TextArea::leading(int leading)
{
	_leading = leading;
}

void TextArea::fitToText(bool flag)
{
	_fitToText = flag;
}

const Color TextArea::bgColor() {
	if (isActive()) {
		return _colorMap["selectedColor"];
	} else {
		return _colorMap["bgColor"];
	}
}

const Color TextArea::textColor() {
	if (isActive()) {
		return _colorMap["selectedTextColor"];
	} else {
		return _colorMap["textColor"];
	}
}


}

