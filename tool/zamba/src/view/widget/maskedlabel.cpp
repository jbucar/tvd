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

#include "maskedlabel.h"

#include "../view.h"

#include <sstream>

namespace wgt {

MaskedLabel::MaskedLabel(
	  int x
	, int y
	, int w
	, int h
	, const colorMap_t &colorMap
	, std::string text
	, int fontSize
	, int alignment
	, int margin)
	: Label(x, y, w, h, colorMap, text, fontSize, alignment, margin)
	, _keysCount(0)
{
	_mask = "__.__";
	_masked = _mask;
}

MaskedLabel::~MaskedLabel()
{
}

void MaskedLabel::draw(View* view){
	view->draw(this);
}


bool MaskedLabel::onDigitPressed(int key)
{
	std::stringstream ssKey;
	ssKey << key;

	std::string value = _text;
	value += ssKey.str()[0];
	++_keysCount;
	text(value);

	needsRepaint(true);
	return true;
}

void MaskedLabel::reset()
{
	Label::reset();
	_keysCount = 0;
	_masked = _mask;
	
}

const std::string& MaskedLabel::maskedText()
{
	std::string masked = _masked;
	size_t text_size = _keysCount;
	if (text_size == 1) {
		masked[4] = _text[0];
	} 
	if (text_size == 2) {
		masked[3] = _text[0];
		masked[4] = _text[1];
	} 
	if (text_size == 3) {
		masked[1] = _text[0];
		masked[3] = _text[1];
		masked[4] = _text[2];
	} 
	if (text_size == 4) {
		masked[0] = _text[0];
		masked[1] = _text[1];
		masked[3] = _text[2];
		masked[4] = _text[3];
	} 
	_masked="";
	for(size_t i = 0; i<masked.size(); ++i){
		_masked += masked[i];
	}
	return _masked;
}

std::string MaskedLabel::rawText(){
	std::string raw = _masked;
	std::replace( raw.begin(), raw.end(), '_', '0' );
	return raw;
}
}
