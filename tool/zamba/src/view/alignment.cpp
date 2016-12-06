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
 * alignment.cpp
 *
 *  Created on: Jun 2, 2011
 *      Author: gonzalo
 */

#include "alignment.h"
#include "modules/canvas.h"

namespace wgt {

int alignx(Alignment alignment, int x, int widgetw, int containerw, int margin)
{
	if(alignment == alignment::None) {
		return x;
	}
	if (alignment & alignment::hLeft) {
		x = x + margin;
	} else if (alignment & alignment::hCenter) {
		x = x + (containerw - widgetw)/2;
	} else if (alignment & alignment::hRight) {
		x = x + containerw - widgetw - margin;
	}
	return x;
}

int aligny(Alignment alignment, int y, int widgeth, int containerh, int margin)
{
	if(alignment == alignment::None) {
		return y;
	}

	y += margin + canvas::fontAscent();

	if (alignment & alignment::vCenter || alignment & alignment::vBottom) {
		int tmp = containerh - margin * 2;
		if (alignment & alignment::vCenter) {
			y += (tmp - widgeth) / 2;
		} else if (alignment & alignment::vBottom) {
			y += tmp - widgeth;
		}
	}

	return y;
}

std::pair<int,int> align(Alignment alignment, int x, int y, int widgetw, int widgeth, int containerw, int containerh, int margin)
{
	x = alignx(alignment, x, widgetw, containerw, margin);
	y = aligny(alignment, y, widgeth, containerh, margin);
	return std::make_pair(x,y);
}

}
