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
#include "../../../src/net/sockaddr.h"
#include "../../../src/net/ipv4/sockaddr.h"
#include "../../../src/net/ipv6/sockaddr.h"
#include <gtest/gtest.h>

TEST( SockAddr, basic_constructor ) {
	util::net::SockAddr addr;
}

TEST( SockAddr, copy_constructor_v4 ) {
	util::net::ipv4::SockAddr v4("127.0.0.2");
	util::net::SockAddr addr1(v4.addr());
	ASSERT_TRUE( addr1.asString() == v4.asString() );
	util::net::SockAddr addr2(addr1);
	ASSERT_TRUE( addr1.asString() == addr2.asString() );
}

TEST( SockAddr, copy_constructor_v6 ) {
	util::net::ipv6::SockAddr v6("::",0);
	util::net::SockAddr addr1(v6.addr());
	ASSERT_EQ( addr1.asString(), v6.asString() );
	ASSERT_EQ( addr1, v6.addr() );

	util::net::SockAddr addr2(addr1);
	ASSERT_EQ( addr1, addr2 );

	std::string tmp1 = addr1.asString();
	std::string tmp2 = addr2.asString();

	ASSERT_EQ( addr1.asString(), addr2.asString() );
}

