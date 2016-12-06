/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV implementation.

    DTV is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV.

    DTV es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "../../src/functions.h"
#include "functions.h"
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace util {

char **makeParams( const char *params[], size_t size ) {
	char **cmd;

	cmd = (char **)calloc( size+1, sizeof(char *) );

	size_t i;
	for (i=0; i<size; i++) {
#ifdef _WIN32
		cmd[i] = _strdup( params[i] );
#else
		cmd[i] = strdup( params[i] );
#endif
	}
	cmd[i] = NULL;
	
	return cmd;
}

void deleteParams( char **params ) {
	int i=0;
	if (params) {
		while (params[i]) {
			free( params[i] );
			i++;
		}
		free( params );
	}
}

}

TEST( UTF8, from_iso8859_15 ) {
	const char *iso885915 = "\xa4 \xa6 \xa8 \xb4 \xb8 \xbc \xbd \xbe";
	std::string tmp( iso885915 );
	util::toUTF8( "ISO-8859-15", tmp );
	ASSERT_TRUE( tmp == "\xe2\x82\xac \xc5\xa0 \xc5\xa1 \xc5\xbd \xc5\xbe \xc5\x92 \xc5\x93 \xc5\xb8" );
}

TEST( UTF8, from_iso8859_1 ) {
	const char *iso88591 = "\xa4 \xa6 \xa8 \xb4 \xb8 \xbc \xbd \xbe";
	std::string tmp( iso88591 );
	util::toUTF8( "ISO-8859-1", tmp );
	ASSERT_TRUE( tmp == "\xc2\xa4 \xc2\xa6 \xc2\xa8 \xc2\xb4 \xc2\xb8 \xc2\xbc \xc2\xbd \xc2\xbe" );
}

TEST( UTF8, from_iso8859_1_large ) {
	const char *iso88591 = "\xa4 \xa6 \xa8 \xb4 \xb8 \xbc \xbd \xbe";
	const char *utf = "\xc2\xa4 \xc2\xa6 \xc2\xa8 \xc2\xb4 \xc2\xb8 \xc2\xbc \xc2\xbd \xc2\xbe";
	std::string tmp, result;
	while (tmp.size() < 1050) {
		tmp += iso88591;
		result += utf;
	}
	util::toUTF8( "ISO-8859-1", tmp );
	ASSERT_TRUE( tmp == result );
}

TEST( base64, encode ) {
	util::BYTE data[] = "a-d+\0vmf\n+er*/g- 9('0\t";

	std::string result = util::base64encode( data, sizeof(data) );
	ASSERT_EQ( "YS1kKwB2bWYKK2VyKi9nLSA5KCcwCQA=", result );
}

TEST( base64, decode ) {
	std::string toDecode("YS1kKwB2bWYKK2VyKi9nLSA5KCcwCQA=");
	util::BYTE decodedData[24];

	size_t dataLength = util::base64decode( toDecode, decodedData, 24 );
	ASSERT_EQ( 23, dataLength );

	ASSERT_EQ( 0, memcmp("a-d+\0vmf\n+er*/g- 9('0\t", decodedData, dataLength) );
}
