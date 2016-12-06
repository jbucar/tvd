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

#include "../../../src/pool/circularpool.h"
#include <gtest/gtest.h>

TEST( CircularPool, basic_constructor ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 96, 4030 );
	ASSERT_TRUE( pool.id() == "id" );
}

TEST( CircularPool, basic_alloc ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );
	util::Buffer *buf = pool.alloc();
	ASSERT_TRUE( buf );
	pool.free( buf );
}

TEST( CircularPool, basic_push_get ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );

	util::Buffer *buf = pool.alloc();
	ASSERT_TRUE( buf );
	pool.put( buf );

	util::Buffer *buf1;
	util::DWORD mask;
	ASSERT_TRUE( pool.get( buf1, mask ) );
	ASSERT_TRUE( buf == buf1 );

	pool.free( buf1 );
}

TEST( CircularPool, basic_alloc_from_nodes ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );
	util::Buffer *bufs[15];
	int i=0;
	for (; i<10; i++) {
		bufs[i] = pool.alloc();
		ASSERT_TRUE( bufs[i] );
		pool.put( bufs[i] );
	}
	bufs[i] = pool.alloc();
	ASSERT_TRUE( bufs[i] );
}

TEST( CircularPool, basic_alloc_from_free_list ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );
	util::Buffer *bufs[15];
	for (int i=0; i<10; i++) {
		bufs[i] = pool.alloc();
		ASSERT_TRUE( bufs[i] );
	}

	for (int i=0; i<5; i++) {
		pool.put( bufs[i] );
	}

	for (int i=5; i<10; i++) {
		pool.free( bufs[i] );
	}

	util::Buffer *buf = pool.alloc();
	ASSERT_TRUE( buf );
	ASSERT_TRUE( buf == bufs[5] );
	pool.free(buf);
}

TEST( CircularPool, remove_all ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );
	util::Buffer *bufs[15];
	for (int i=0; i<10; i++) {
		bufs[i] = pool.alloc();
		ASSERT_TRUE( bufs[i] );
		pool.put( bufs[i] );
	}

	//	Free all nodes and put into free list
	pool.removeAll();

	//	Alloc and check that new buffer was allocated
	util::Buffer *buf = pool.alloc();

	bool result=false;
	for (int i=0; i<10; i++) {
		result |= (buf ==bufs[i]);
	}
	ASSERT_TRUE( result );

	pool.free( buf );
}

struct RemoveElement {
	RemoveElement( util::Buffer *buf ) : _buf(buf) {}
	bool operator()( util::Buffer *item ) const {
		return item == _buf;
	}

	util::Buffer *_buf;
};

TEST( CircularPool, remove_if ) {
	util::pool::CircularPool<util::Buffer *> pool( "id", 10, 4030 );
	util::Buffer *bufs[15];
	for (int i=0; i<10; i++) {
		bufs[i] = pool.alloc();
		ASSERT_TRUE( bufs[i] );
		pool.put( bufs[i] );
	}

	pool.remove_if( RemoveElement( bufs[9] ) );

	for (int i=0; i<9; i++) {
		util::Buffer *buf1;
		util::DWORD mask;
		ASSERT_TRUE( pool.get( buf1, mask ) );
		ASSERT_TRUE( bufs[i] == buf1 );
	}
}


//TODO: TEST( CircularPool, exit )
//TODO: TEST( CircularPool, notify_all )
