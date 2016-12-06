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

#include "changechannelbydigit.h"

#include <sstream>
#include <iomanip>

#define X 720-72-100
#define Y 57
#define W 100
#define H 50
#define BORDER_W 4

#define LABEL_X 8
#define FONT_SIZE 30


namespace wgt{
	
ChangeChannelByDigit::ChangeChannelByDigit( const colorMap_t &colorMap )
		      :BorderedWindow(X, Y, W, H, colorMap, BORDER_W){
	_visible = false;
	_keysCount = 0;
	_valueL = LabelPtr(new Label(LABEL_X + BORDER_W, 0, W, H, colorMap, "____", FONT_SIZE, alignment::vTop|alignment::hLeft));
	
	addChild(_valueL);

	fixOffsets();
}

ChangeChannelByDigit::~ChangeChannelByDigit(){
}

bool ChangeChannelByDigit::onDigitPressed(int key)
{
	std::stringstream ssKey;
	ssKey << key;

	std::string value = _valueL->text();
	value[_keysCount] = ssKey.str()[0];
	++_keysCount;

	_valueL->text(value);

	needsRepaint(true);
	return true;
}

void ChangeChannelByDigit::reset(){
	_keysCount = 0;
	_valueL->text("____");
	
}

const std::string& ChangeChannelByDigit::getValueStr(){
	return _valueL->text();
}

}

