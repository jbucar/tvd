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

#include <string.h>
#include <gtest/gtest.h>
#include "util.h"
#include "../../src/functions.h"
extern "C" {
	#include "../../src/md5.h"
}

TEST( Md5, hashing ) {

	MD5_CTX ctx;
	unsigned char digest[16];
	unsigned char hex_digest[16*2+1];
	static const char *const test[7*2] = {
		"", "d41d8cd98f00b204e9800998ecf8427e",
		"a", "0cc175b9c0f1b6a831c399e269772661",
		"abc", "900150983cd24fb0d6963f7d28e17f72",
		"message digest", "f96b697d7cb7938d525a2f31aaf161d0",
		"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789","d174ab98d277d9f5a5611c2c9f419d9f",
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a"
	};

	for(int i=0; i<7; ++i) {
		MD5_Init(&ctx);
		MD5_Update(&ctx, test[i*2], strlen(test[i*2]));
		MD5_Final(digest, &ctx);
		for (int j = 0; j < 16; j++) {
			sprintf((char*) (hex_digest+2*j), "%02x", digest[j]);
		}

		ASSERT_STREQ((char*) test[i*2+1], (char*) hex_digest);
	}
}

TEST( Md5, file_checksum ) {
	std::string filename = util::getTestFile("md5file");
	std::string md5 = util::check_md5(filename);
	ASSERT_STREQ("1a99902011149b81787b38083282c112", md5.c_str());
	ASSERT_TRUE( util::check_md5(filename, "1a99902011149b81787b38083282c112") );
}