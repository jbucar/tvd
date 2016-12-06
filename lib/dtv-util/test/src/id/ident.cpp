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

#include "../../../src/id/pool.h"
#include <gtest/gtest.h>

TEST( Ident, constructor ) {
	util::id::Pool _pool("test");
}

TEST( Ident, alloc_one ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id = _pool.alloc();
	}
}

TEST( Ident, alloc_two ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id1 = _pool.alloc();
		util::id::Ident id2 = _pool.alloc();
		ASSERT_TRUE( id1 != id2 );
	}
}

TEST( Ident, alloc_max_reached_without_free ) {
	util::id::Pool _pool( "test", 0, 2 );

	{
		util::id::Ident id1 = _pool.alloc();
		util::id::Ident id2 = _pool.alloc();
		util::id::Ident id3 = _pool.alloc();

		try {
			util::id::Ident id4 = _pool.alloc();
			ASSERT_TRUE( false );
		} catch ( std::exception &/*e*/ ) {
		}
	}	
}

TEST( Ident, alloc_max_reached_with_free ) {
	util::id::Pool _pool( "test", 0, 2 );

	{
		util::id::Ident id1 = _pool.alloc();
		{
			util::id::Ident id2 = _pool.alloc();
		}
		util::id::Ident id3 = _pool.alloc();
		util::id::Ident id4 = _pool.alloc();
	}	
}

TEST( Ident, copy_contsruct ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id1 = _pool.alloc();
		util::id::Ident id2(id1);
		ASSERT_TRUE( id1 == id2 );
	}
}

TEST( Ident, opeator_copy ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id1 = _pool.alloc();
		util::id::Ident id2 = _pool.alloc();		
		ASSERT_TRUE( id1 != id2 );
		id1 = id2;
		ASSERT_TRUE( id1 == id2 );
	}
}

TEST( Ident, copy_empty_id ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id1 = _pool.alloc();
		util::id::Ident id2;
		ASSERT_TRUE( id1 != id2 );
		id1 = id2;
		ASSERT_TRUE( id1 == id2 );
	}
}

TEST( Ident, copy_from_empty_id ) {
	util::id::Pool _pool("test");

	{
		util::id::Ident id1;
		util::id::Ident id2 = _pool.alloc();
		ASSERT_TRUE( id1 != id2 );
		id1 = id2;
		ASSERT_TRUE( id1 == id2 );
	}
}

