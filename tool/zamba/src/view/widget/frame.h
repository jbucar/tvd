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
 * frame.h
 *
 *  Created on: Jul 6, 2011
 *      Author: gonzalo
 */

#ifndef FRAME_H_
#define FRAME_H_

#include "window.h"
#include <canvas/point.h>
#include <canvas/size.h>

namespace wgt {

class Frame: public wgt::Window {
public:
	Frame(int outterx, int outtery, int outterw, int outterh, int innerx, int innery, int innerw, int innerh, const colorMap_t &colorMap);
	virtual ~Frame();

	void draw(View* view);

	const canvas::Point &outterPos() const;
	const canvas::Size &outterSize() const;
	const canvas::Point &innerPos() const;
	const canvas::Size &innerSize() const;

private:
	canvas::Point _outterPos;
	canvas::Size _outterSize;
	canvas::Point _innerPos;
	canvas::Size _innerSize;
};

DEF_SHARED(Frame);

}

#endif /* FRAME_H_ */
