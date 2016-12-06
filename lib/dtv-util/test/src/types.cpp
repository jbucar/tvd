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
#include "../../src/buffer.h"
#include "../../src/types.h"
#include <gtest/gtest.h>
#include <iostream>

namespace types {

util::BYTE values[] = { 0xAA, 0x55, 0xAA, 0x55, 0x55, 0xAA, 0x55, 0xAA, 0x00, 0x00, 0x00, 0x00 };

}

TEST( Types, check_size ) {
	ASSERT_TRUE( sizeof(util::SSIZE_T) == sizeof(size_t) );
	ASSERT_TRUE( sizeof(util::BYTE) == 1 );
	ASSERT_TRUE( sizeof(util::WORD) == 2 );
	ASSERT_TRUE( sizeof(util::DWORD) == 4 );
	ASSERT_TRUE( sizeof(util::QWORD) == 8 );
}

TEST( Types, basic_byte_get ) {
	util::BYTE result = GET_BYTE(types::values);
	ASSERT_TRUE( result == 0xAA );
}

TEST( Types, basic_word_get ) {
	util::WORD result = GET_WORD(types::values);
	ASSERT_TRUE( result == 0xAA55 );
}

TEST( Types, basic_dword_get ) {
	util::DWORD result = GET_DWORD(types::values);
	ASSERT_TRUE( result == 0xAA55AA55L );
}

TEST( Types, basic_qword_get ) {
	util::QWORD result = GET_QWORD(types::values);
	ASSERT_TRUE( result == 0xAA55AA5555AA55AALL );
}

TEST( Types, basic_byte_put ) {
	util::Buffer b(1);
	b.resize(1);
	PUT_BYTE(0x01, b.bytes());
	ASSERT_EQ( "01", b.asHexa() );
}


TEST( Types, basic_word_put ) {
	util::Buffer w(1);
	w.resize(2);
	PUT_WORD(0x0102, w.bytes());
	ASSERT_EQ( "02:01", w.asHexa() );
}

TEST( Types, basic_be_word_put ) {
	util::Buffer w(1);
	w.resize(2);
	PUT_BE_WORD(0x0102, w.bytes());
	ASSERT_EQ( "01:02", w.asHexa() );
}


TEST( Types, basic_dword_put ) {
	util::Buffer dw(1);
	dw.resize(4);
	PUT_DWORD(0x01020304L, dw.bytes());
	ASSERT_EQ( "04:03:02:01", dw.asHexa() );

}

TEST( Types, basic_be_dword_put ) {
	util::Buffer dw(1);
	dw.resize(4);
	PUT_BE_DWORD(0x01020304L, dw.bytes());
	ASSERT_EQ( "01:02:03:04", dw.asHexa() );
}


TEST( Types, basic_qword_put ) {
	util::Buffer dw(1);
	dw.resize(8);
	PUT_QWORD(0x0102030405060708LL, dw.bytes());
	ASSERT_EQ( "08:07:06:05:04:03:02:01", dw.asHexa() );
}

TEST( Types, basic_be_qword_put ) {
	util::Buffer dw(1);
	dw.resize(8);
	PUT_BE_QWORD(0x0102030405060708LL, dw.bytes());
	ASSERT_EQ( "01:02:03:04:05:06:07:08", dw.asHexa() );
}
