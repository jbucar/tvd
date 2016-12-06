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
 * frame.cpp
 *
 *  Created on: Jul 6, 2011
 *      Author: gonzalo
 */

#include "frame.h"

#include "../view.h"

namespace wgt
{

Frame::Frame(int outterx, int outtery, int outterw, int outterh, int innerx, int innery, int innerw, int innerh, const colorMap_t &colorMap)
	: Window(outterx, outtery, outterw, outterh, colorMap)
{
	_outterPos = canvas::Point( outterx, outtery );
	_outterSize = canvas::Size( outterw, outterh );
	_innerPos = canvas::Point( innerx, innery );
	_innerSize = canvas::Size( innerw, innerh );
}

Frame::~Frame()
{
}

void Frame::draw(View* view) {
	view->draw(this);
}

const canvas::Point &Frame::outterPos() const {
	return _outterPos;
}

const canvas::Size &Frame::outterSize() const {
	return _outterSize;
}

const canvas::Point &Frame::innerPos() const {
	return _innerPos;
}

const canvas::Size &Frame::innerSize() const {
	return _innerSize;
}

}
