/******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "net.h"
#include "../lua.h"
#include <util/net/config.h>
#include <util/net/ipv4/sockaddr.h>
#include <util/log.h>
#include <boost/foreach.hpp>

namespace luaz {
namespace net {

namespace impl {
	static util::net::Config *net = NULL;
}

static int isConnected( lua_State *L ) {
	lua_pushboolean( L, impl::net->isConnected() );
	return 1;
}

static int adapters( lua_State *L ) {
	util::net::Adapters adapters;
	impl::net->getAdapters( adapters );
	std::vector<std::string> names;
	BOOST_FOREACH( const util::net::Adapter &a, adapters ) {
		names.push_back( a.name );
	}
	return lua::pushList( L, names );
}

static int defaultGW( lua_State *L ) {
	std::string gw = impl::net->defaultGateway();
	lua_pushstring( L, gw.c_str() );
	return 1;
}

static int address( lua_State *L ) {
	const int device = luaL_checkint(L, 1);
	util::net::Adapters adapters;
	impl::net->getAdapters( adapters );
	lua_pushstring( L, adapters[device].addresses[0].addr.asString().c_str() );
	return 1;
}

static int netmask( lua_State *L ) {
	const int device = luaL_checkint(L, 1);
	util::net::Adapters adapters;
	impl::net->getAdapters( adapters );
	lua_pushstring( L, adapters[device].addresses[0].netmask.asString().c_str() );
	return 1;
}

static int nameservers( lua_State *L ) {
	util::net::dns::Config dns;
	impl::net->getDNS( dns );
	return lua::pushList( L, dns.servers );
}

static int domain( lua_State *L ) {
	util::net::dns::Config dns;
	impl::net->getDNS( dns );
	lua_pushstring( L, dns.domain.c_str() );
	return 1;
}

static const struct luaL_Reg net_methods[] = {
	{ "isConnected", isConnected  },
	{ "adapters",    adapters     },
	{ "address",     address      },
	{ "netmask",     netmask      },
	{ "defaultGW",   defaultGW    },
	{ "nameservers", nameservers  },
	{ "domain",      domain       },
	{ NULL,          NULL         }
};

void initialize( zapper::Zapper * /*mgr*/ ) {
	impl::net = util::net::Config::create();
}

void finalize( void ) {
	LINFO("net::Module", "Stop");
	delete impl::net;
}

void start( lua_State *L ) {
	LINFO("net::Module", "Start");
	luaL_register( L, "net", net_methods );
}

void stop() {}

}
}

