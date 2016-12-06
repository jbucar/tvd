/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "text.h"
#include <util/functions.h>

namespace tuner {

namespace impl {

static const char *charsetTable[] = {
	"ISO-8859-15",  //  00
	"ISO-8859-5",   //  01
	"ISO-8859-6",   //  02
	"ISO-8859-7",   //  03
	"ISO-8859-8",   //  04
	"ISO-8859-9",   //  05
	"ISO-8859-10",  //  06
	"ISO-8859-11",  //  07
	"ISO-8859-15",  //  08 (reserved)
	"ISO-8859-13",  //  09
	"ISO-8859-14",  //  0A
	"ISO-8859-15",  //  0B
	"ISO-8859-15",  //  0C
	"ISO-8859-15",  //  0D
	"ISO-8859-15",  //  0E
	"ISO-8859-15",  //  0F
	"ISO-8859",     //  10
	"ISO-10646",    //  11
	"KSX1001",      //  12
	"GB-2312",      //  13
	"ISO-10646",    //  14
	"ISO-10646",    //  15
	"ISO-8859-15",  //  16 (reserved)
	"ISO-8859-15",  //  17 (reserved)
	"ISO-8859-15",  //  18 (reserved)
	"ISO-8859-15",  //  19 (reserved)
	"ISO-8859-15",  //  1A (reserved)
	"ISO-8859-15",  //  1B (reserved)
	"ISO-8859-15",  //  1C (reserved)
	"ISO-8859-15",  //  1D (reserved)
	"ISO-8859-15",  //  1E (reserved)
	"ISO-8859-15",  //  1F (todo)
};

} // namespace impl

int parseLanguage( Language &lang, util::BYTE *data ) {
	char ptr[4];
	for (int i=0; i<3; i++) {
		ptr[i] = data[i];
	}
	ptr[3] = '\0';
	lang = ptr;
	return 3;
}

int parseText( std::string &var, util::BYTE *ptr, int len ) {
	if (len > 0) {
		util::BYTE table = 0;
		util::BYTE off = 0;
		if (ptr[0] < 0x20) {
			table = ptr[0];
			off=1;
		}

		var.assign( (char *)(ptr+off), len-off );
		util::toUTF8( impl::charsetTable[table], var );
	}
	else {
		var.clear();
	}
	return len;
}

int parseText( std::string &var, util::BYTE *ptr ) {
	util::BYTE len = GET_BYTE(ptr);
	return parseText( var, ptr+1, len )+1;
}

std::string charsetName( util::BYTE table ) {
	return impl::charsetTable[ table ];
}

}

