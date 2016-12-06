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

#include "../../src/buffer.h"
#include "../../src/types.h"
#include <gtest/gtest.h>

TEST( Buffer, default_constructor ) {
	util::Buffer buf;
	ASSERT_TRUE( buf.data() == NULL );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.asHexa().empty() );
}

TEST( Buffer, block_constructor ) {
	util::Buffer buf( 1024 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 1024 );
	ASSERT_TRUE( buf.blockSize() == 1024 );
	ASSERT_TRUE( buf.asHexa().empty() );
}

TEST( Buffer, copy_constructor ) {
	util::Buffer bufOriginal( 1024 );
	util::Buffer buf( bufOriginal );

	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 1024 );
	ASSERT_TRUE( buf.blockSize() == 1024 );
	ASSERT_TRUE( buf.asHexa().empty() );
}

TEST( Buffer, buffer_owner_constructor ) {
	const char name[] = "rako";
	size_t s = strlen(name);
	util::Buffer buf( name, s );

	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.data() != name );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.asHexa().empty() == false );
}

TEST( Buffer, buffer_no_owner_constructor ) {
	const char name[] = "rako";
	size_t s = strlen(name);
	util::Buffer buf( name, s, false );

	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.data() == name );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.asHexa().empty() == false );
}

TEST( Buffer, buffer_no_owner2_constructor ) {
	char tmp[200];
	size_t s=200;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.data() == tmp );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.asHexa().empty() == false );
}

TEST( Buffer, default_resize_mayor ) {
	util::Buffer buf;

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( 10 );
	ASSERT_TRUE( buf.length() == 10 );
	ASSERT_TRUE( buf.capacity() == 10 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, default_resize_equal ) {
	util::Buffer buf;

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( 0 );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() == NULL );
}

TEST( Buffer, default_resize_zero ) {
	util::Buffer buf;
	const char name[] = "rako";
	size_t s = strlen(name);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );

	buf.copy( name, s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.resize( 0 );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, default_resize_minor ) {
	util::Buffer buf;
	const char name[] = "rako";
	size_t s = strlen(name);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );

	buf.copy( name, s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.resize( 2 );
	ASSERT_TRUE( buf.length() == 2 );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, block_resize_mayor1 ) {
	util::Buffer buf(100);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	buf.resize( 10 );
	ASSERT_TRUE( buf.length() == 10 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, block_resize_mayor2 ) {
	util::Buffer buf(100);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	buf.resize( 101 );
	ASSERT_TRUE( buf.length() == 101 );
	ASSERT_TRUE( buf.capacity() == 200 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, block_resize_equal ) {
	util::Buffer buf(100);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	buf.resize( 0 );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, block_resize_minor ) {
	util::Buffer buf(100);
	const char name[] = "rako";
	size_t s = strlen(name);

	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );

	buf.copy( name, s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.resize( 2 );
	ASSERT_TRUE( buf.length() == 2 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, no_owner_resize_mayor1 ) {
	char tmp[100];
	size_t s = 100;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( 10 );
	ASSERT_TRUE( buf.length() == 10 );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, no_owner_resize_mayor2 ) {
	char tmp[100];
	size_t s = 100;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( 101 );
	ASSERT_TRUE( buf.length() == 101 );
	ASSERT_TRUE( buf.capacity() == 101 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, no_owner_resize_mayor3 ) {
	char *tmp = NULL;
	size_t s = 0;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( 101 );
	ASSERT_TRUE( buf.length() == 101 );
	ASSERT_TRUE( buf.capacity() == 101 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, no_owner_resize_equal ) {
	char tmp[100];
	size_t s = 100;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	buf.resize( s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

TEST( Buffer, no_owner_resize_minor ) {
	char tmp[100];
	size_t s = 100;
	util::Buffer buf( tmp, s, false );

	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );

	buf.resize( 2 );
	ASSERT_TRUE( buf.length() == 2 );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
}

//	void copy( char *buf, int len );
//		a) owner=true,  bufLen=len,   blockSize=0
//		b) owner=true,  bufLen>len,   blockSize=0
//		c) owner=true,  bufLen<len,   blockSize=0
//		d) owner=true,  bufLen<len,   blockSize=100
//		e) owner=false, bufLen=100, blockSize=0
TEST( Buffer, copy_a ) {
	char name1[] = "rako";
	char name2[] = "1234";
	size_t s = strlen(name1);
	util::Buffer buf( name1, s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.copy( name2, s );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name2,s) == 0 );
}

TEST( Buffer, copy_b ) {
	char name1[] = "rako22";
	size_t s1 = strlen(name1);

	char name2[] = "1234";
	size_t s2 = strlen(name1);

	util::Buffer buf( name1, s1 );
	ASSERT_TRUE( buf.length() == s1 );
	ASSERT_TRUE( buf.capacity() == s1 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.copy( name2, s2 );
	ASSERT_TRUE( buf.length() == s2 );
	ASSERT_TRUE( buf.capacity() == s2 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name2,s2) == 0 );
}

TEST( Buffer, copy_c ) {
	char name1[] = "ra";
	size_t s1 = strlen(name1);

	char name2[] = "1234";
	size_t s2 = strlen(name1);

	util::Buffer buf( name1, s1 );
	ASSERT_TRUE( buf.length() == s1 );
	ASSERT_TRUE( buf.capacity() == s1 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.copy( name2, s2 );
	ASSERT_TRUE( buf.length() == s2 );
	ASSERT_TRUE( buf.capacity() == s2 );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name2,s2) == 0 );
}

TEST( Buffer, copy_d ) {
	char name1[] = "ra";
	size_t s1 = strlen(name1);
	util::Buffer buf( 100 );
	buf.copy( name1, s1 );
	ASSERT_TRUE( buf.length() == s1 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );

	char name2[] = "1234";
	size_t s2 = strlen(name1);
	buf.copy( name2, s2 );
	ASSERT_TRUE( buf.length() == s2 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name2,s2) == 0 );
}

//	void copy( int pos, char *buf, int len );
//		a) owner=true,  pos=len,   blockSize=0
//		b) owner=true,  pos>len,   blockSize=0
//		c) owner=true,  pos<len,   blockSize=0
//		d) owner=true,  pos<len,   blockSize=100
//		e) owner=false, pos=100,   blockSize=0
//		f) pos<0
TEST( Buffer, copy_with_pos_a ) {
	std::string name1 = "rako";
	util::Buffer buf( name1.c_str(), name1.length() );
	ASSERT_TRUE( buf.length() == name1.length() );
	ASSERT_TRUE( buf.capacity() == name1.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	std::string name2 = "1234";
	std::string res = name1 + name2;
	buf.append( name2.c_str(), name2.length() );
	ASSERT_TRUE( buf.length() == res.length() );
	ASSERT_TRUE( buf.capacity() == res.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),res.c_str(),res.length()) == 0 );
}

TEST( Buffer, copy_with_pos_b ) {
	std::string name1("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	std::string name2( "rako" );
	std::string name3( "otrodddd" );

	util::Buffer buf( name1.c_str(), name1.length() );
	ASSERT_TRUE( buf.length() == name1.length() );
	ASSERT_TRUE( buf.capacity() == name1.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );

	buf.copy( name2.c_str(), name2.length() );
	ASSERT_TRUE( buf.length() == name2.length() );
	ASSERT_TRUE( buf.capacity() == name1.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name2.c_str(),name2.length()) == 0 );

	buf.copy( buf.length()+5, name3.c_str(), name3.length() );
	size_t s = (name2.length()+name3.length()+5);
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == name1.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),"rakoaaaaaotrodddd",buf.length()) == 0 );
}

TEST( Buffer, copy_with_pos_c ) {
	std::string name1( "123456789" );
	std::string name2( "1234" );
	std::string res( "121234789" );

	util::Buffer buf( name1.c_str(), name1.length() );
	ASSERT_TRUE( buf.length() == name1.length() );
	ASSERT_TRUE( buf.capacity() == name1.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name1.c_str(),name1.length()) == 0 );

	buf.copy( 2, name2.c_str(), name2.length() );
	ASSERT_TRUE( buf.length() == res.length() );
	ASSERT_TRUE( buf.capacity() == res.length() );
	ASSERT_TRUE( buf.blockSize() == 0 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),res.c_str(),buf.length()) == 0 );
}

TEST( Buffer, copy_with_pos_d ) {
	std::string name1( "123456789" );
	std::string name2( "1234" );
	std::string res( "121234789" );
	util::Buffer buf( 100 );

	buf.copy( name1.c_str(), name1.length() );
	ASSERT_TRUE( buf.length() == name1.length() );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),name1.c_str(),name1.length()) == 0 );

	buf.copy( 2, name2.c_str(), name2.length() );
	ASSERT_TRUE( buf.length() == res.length() );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( memcmp(buf.data(),res.c_str(),buf.length()) == 0 );
}

//	void assign( char *buf, int len );
TEST( Buffer, assign_owner ) {
	const char name[] = "rako";
	size_t s = strlen(name);
	util::Buffer buf( name, s, false );

	ASSERT_TRUE( buf.data() == name );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );

	char name2[] = "rako23";
	size_t s2 = strlen(name2);
	buf.swap( name2, s2 );
	ASSERT_TRUE( buf.data() == name2 );
	ASSERT_TRUE( buf.length() == s2 );
	ASSERT_TRUE( buf.capacity() == s2 );
	ASSERT_TRUE( buf.blockSize() == 0 );

	char name3[] = "sdf";
	size_t s3 = strlen(name3);
	buf.swap( name3, s3 );
	ASSERT_TRUE( buf.data() == name3 );
	ASSERT_TRUE( buf.length() == s3 );
	ASSERT_TRUE( buf.capacity() == s3 );
	ASSERT_TRUE( buf.blockSize() == 0 );
}

TEST( Buffer, assign_no_owner_buf_null ) {
	// owner=false && buf==NULL
	util::Buffer buf;
	ASSERT_TRUE( buf.data() == NULL );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 0 );
	ASSERT_TRUE( buf.blockSize() == 0 );

	char name[] = "rako";
	size_t s = strlen(name);
	buf.swap( name, s );
	ASSERT_TRUE( buf.data() == name );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );
}

TEST( Buffer, assign_no_owner_buf_no_null ) {
	// owner=false && buf!=NULL
	char name[] = "rako";
	size_t s = strlen(name);
	util::Buffer buf( name, s );
	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );

	char name2[] = "rako355";
	size_t s2 = strlen(name2);
	buf.swap( name2, s2 );
	ASSERT_TRUE( buf.data() == name2 );
	ASSERT_TRUE( buf.length() == s2 );
	ASSERT_TRUE( buf.capacity() == s2 );
	ASSERT_TRUE( buf.blockSize() == 0 );
}

//	void swap( Buffer &other );
TEST( Buffer, swap ) {
	const char name[] = "rako";
	size_t s = strlen(name);
	util::Buffer buf( name, s, false );

	ASSERT_TRUE( buf.data() == name );
	ASSERT_TRUE( buf.length() == s );
	ASSERT_TRUE( buf.capacity() == s );
	ASSERT_TRUE( buf.blockSize() == 0 );

	util::Buffer other( 100 );
	ASSERT_TRUE( other.data() != NULL );
	ASSERT_TRUE( other.length() == 0 );
	ASSERT_TRUE( other.capacity() == 100 );
	ASSERT_TRUE( other.blockSize() == 100 );

	buf.swap( other );

	ASSERT_TRUE( buf.data() != NULL );
	ASSERT_TRUE( buf.length() == 0 );
	ASSERT_TRUE( buf.capacity() == 100 );
	ASSERT_TRUE( buf.blockSize() == 100 );

	ASSERT_TRUE( other.data() == name );
	ASSERT_TRUE( other.length() == s );
	ASSERT_TRUE( other.capacity() == s );
	ASSERT_TRUE( other.blockSize() == 0 );
}
