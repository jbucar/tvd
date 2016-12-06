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

#pragma once

#include <util/types.h>

#define COLOR_LIST(fnc)	          \
	fnc ( white,       0xFF, 0xFF, 0xFF, 0xFF ) \
	fnc ( aqua,        0x00, 0xFF, 0xFF, 0xFF ) \
	fnc ( lime,        0x00, 0xFF, 0x00, 0xFF ) \
	fnc ( yellow,      0xFF, 0xFF, 0x00, 0xFF ) \
	fnc ( red,         0xFF, 0x00, 0x00, 0xFF ) \
	fnc ( fuchsia,     0xFF, 0x00, 0xFF, 0xFF ) \
	fnc ( purple,      0x80, 0x00, 0x80, 0xFF ) \
	fnc ( maroon,      0x80, 0x00, 0x00, 0xFF ) \
	fnc ( blue,        0x00, 0x00, 0xFF, 0xFF ) \
	fnc ( navy,        0x00, 0x00, 0x80, 0xFF ) \
	fnc ( teal,        0x00, 0x80, 0x80, 0xFF ) \
	fnc ( green,       0x00, 0x80, 0x00, 0xFF ) \
	fnc ( olive,       0x80, 0x80, 0x00, 0xFF ) \
	fnc ( silver,      0xC0, 0xC0, 0xC0, 0xFF ) \
	fnc ( gray,        0x80, 0x80, 0x80, 0xFF ) \
	fnc ( black,       0x00, 0x00, 0x00, 0xFF ) \
	fnc ( transparent, 0x00, 0x00, 0x00, 0x00 ) \
	fnc ( magenta,     0x00, 0x00, 0x00, 0x00 )

namespace canvas {

class Color;

namespace color {

#define DO_ENUM_COLOR( name, r, g, b, a ) name,
enum type {
	COLOR_LIST(DO_ENUM_COLOR)
	LAST_COLOR
};
#undef DO_ENUM_COLOR

bool get( const char *textColor, Color &color );
}	//	end color

class Color {
public:
	Color( void );
	Color( util::BYTE r, util::BYTE g, util::BYTE b, util::BYTE alpha );
	Color( util::BYTE r, util::BYTE g, util::BYTE b );
	~Color( void );

	bool operator==(const Color &color) const;
	bool operator!=(const Color &color) const;
	bool equals( const Color &color, int threshold ) const;

	util::BYTE r, g, b, alpha;
};
	
}
