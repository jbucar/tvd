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

#include "utf8_iconv.h"
#include "../log.h"
#include <iconv.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 250

namespace util {
namespace tvd_iconv {

std::string toUTF8( const std::string &charset, const std::string &str ) {
	iconv_t ctx = iconv_open( "UTF8", charset.c_str() );
	if (!ctx) {
		LWARN( "iconv", "Cannot open iconv context for charset: charset=%s", charset.c_str() );
		return str;
	}

	std::string tmp;
	char out[MAX_SIZE];
	char *in_buf = (char *)str.c_str();
	size_t in_size = str.length()+1;
	while (in_size > 0) {
		char *out_buf = out;
		size_t out_size = MAX_SIZE;

		size_t ret = ::iconv( ctx, &in_buf, &in_size, &out_buf, &out_size );
		if (ret == size_t(-1)) {
			if (errno != E2BIG) {
				LWARN( "iconv", "Cannot convert input text: charset=%s, errno=%s", charset.c_str(), strerror(errno) );
				iconv_close( ctx );		
				return str;
			}
		}
		tmp += std::string(out,MAX_SIZE-out_size);
	}

	tmp.resize( tmp.size()-1 );
	iconv_close( ctx );

	return tmp;
}

}
}

