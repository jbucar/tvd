/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

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

#ifndef _UTIL_FUNCTIONS_H_
#define _UTIL_FUNCTIONS_H_

#include "types.h"
#include <set>
#include <string>

namespace util {

template<typename T, typename P>
inline void erase_if( T &container, const P &pred ) {
	typename T::iterator it=container.begin();
	while (it != container.end()) {
		if (pred( (*it) )) {
			it = container.erase( it );
		}
		else {
			it++;
		}
	}
}

bool compareSets(const std::set<int> *set1, const std::set<int> *set2);
DWORD crc_calc( DWORD crc, const BYTE *buffer, size_t length );
WORD crc16_calc( WORD crc, BYTE *buf, size_t len );
void toUTF8( const std::string &charset, std::string &str );

/**
 * Codifica un arreglo de datos binarios en base64
 * @param data Los datos binarios a codificar.
 * @param length La longitud en bytes de los datos.
 * @return La representación en base64 de los datos recibidos.
 */
std::string base64encode( BYTE *data, size_t length );

/**
 * Decodifica un string codificado en base64
 * @param encoded Los datos codificados en base64.
 * @param decoded[out] El arreglo donde se almacenarán los datos binarios decodificados.
 * @param buffLength La longitud de @b decoded (debe ser al menos: @bencoded.size() * 3 / 4).
 * @return La longitud de los datos binarios retornados en @b decoded.
 */
size_t base64decode( const std::string &encoded, BYTE *decoded, size_t buffLength );

/**
 * Retorna la firma md5 de un archivo
 * @param file El path del archivo a procesar.
 * @return El checksum md5.
 */
const std::string check_md5( const std::string &filePath );

/**
 * Verifica la firma md5 de un archivo
 * @param file El path del archivo a procesar.
 * @param md5 La firma md5 a comparar.
 * @return El checksum md5.
 */
bool check_md5( const std::string &filePath, const std::string &md5 );

}

#endif //_UTIL_FUNCTIONS_H_
