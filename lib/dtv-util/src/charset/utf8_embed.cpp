/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "utf8_embed.h"
#include "../log.h"

namespace util {
namespace embed {

//	changing the encoding from ISO-8859-15 to UTF-8.
std::string toUTF8( const std::string &charset, const std::string &str ) {
	std::string d;
	const unsigned char *s = (const unsigned char *)str.c_str();

	if (charset == "ISO-8859-15") {
		while (*s) {
			if (*s < 128) {
				d += (char) *(s++);
			}
			else if (*s < 192) {
				switch (*s) {
					case 164: d += "\xE2\x82\xAC"; s++; break;
					case 166: d += "\xC5\xA0"; s++; break;
					case 168: d += "\xC5\xA1"; s++; break;
					case 180: d += "\xC5\xBD"; s++; break;
					case 184: d += "\xC5\xBE"; s++; break;
					case 188: d += "\xC5\x92"; s++; break;
					case 189: d += "\xC5\x93"; s++; break;
					case 190: d += "\xC5\xB8"; s++; break;
					default:  d += "\xC2"; d += (char) *(s++); break;
				}
			}
			else {
				d += "\xC3";
				d += (char) (*(s++) - 64);
			}
		}
	}
	else if (charset == "ISO-8859-1") {
		while (*s) {
			if (*s<128) {
				d += (char) *s++;
			} else {
				d += (char)(0xc2+(*s>0xbf)), d+= (char) ((*s++&0x3f)+0x80);
			}
		}
	}
	else {
		LWARN( "text", "Cannot convert using charset table: charset=%s", charset.c_str() );
		d = str;
	}

	return d;
}

}
}
