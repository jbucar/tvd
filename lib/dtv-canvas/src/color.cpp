/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "color.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace canvas {
	
namespace color {

typedef struct {
	const char *name;
	util::BYTE r;
	util::BYTE g;
	util::BYTE b;
	util::BYTE a;	
} ColorType;

#define DO_ENUM_COLOR( name, r, g, b, a ) { #name, r, g, b, a },
static const ColorType colors[] = {
	COLOR_LIST(DO_ENUM_COLOR)
	{ NULL, 0, 0, 0, 0 }
};
#undef DO_ENUM_COLOR

bool get( const char *textColor, Color &color ) {
	int i=0;
	while (colors[i].name && strcmp(colors[i].name,textColor) != 0) {
		i++;
	}
	if (i < LAST_COLOR) {
		color = Color( colors[i].r, colors[i].g, colors[i].b, colors[i].a );
		return true;
	}
	return false;
}

}

Color::Color( void )
{
	r = 0;
	g = 0;
	b = 0;
	alpha = 0;
}

Color::Color( util::BYTE paramR, util::BYTE paramG, util::BYTE paramB, util::BYTE paramAlpha )
{
	r = paramR;
	g = paramG;
	b = paramB;
	alpha = paramAlpha;
}

Color::Color( util::BYTE paramR, util::BYTE paramG, util::BYTE paramB )
{
	r = paramR;
	g = paramG;
	b = paramB;
	alpha = 255;
}

Color::~Color( void )
{
}

bool Color::operator==( const Color &color ) const {
	return r == color.r && g == color.g && b == color.b;
}

bool Color::operator!=( const Color &color ) const {
	return !((*this) == color);
}

bool Color::equals( const Color &color, int threshold ) const {
	bool result=true;
	if (abs(color.r - r) > threshold) {
		result=false;
	}
	if (abs(color.g - g) > threshold) {
		result=false;
	}
	if (abs(color.b - b) > threshold) {
		result=false;
	}
	if (abs(color.alpha - alpha) > threshold) {
		result=false;
	}
	return result;
}

}
