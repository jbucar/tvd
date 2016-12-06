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

#include <gtest/gtest.h>
#include "../../../src/net/config.h"
#include "../../../src/net/ipv4/sockaddr.h"
#include <boost/foreach.hpp>

#define PRINT_INFO

TEST( NetConfig, constructor ) {
	util::net::Config *net = util::net::Config::create();
	ASSERT_TRUE(net);
	delete net;
}

TEST( NetConfig, is_connected ) {
	util::net::Config *net = util::net::Config::create();
	ASSERT_TRUE( net->isConnected() );
	delete net;
}

TEST( NetConfig, nameservers ) {
	util::net::Config *net = util::net::Config::create();
	util::net::dns::Config dns;
	ASSERT_TRUE( net->getDNS(dns) );
	ASSERT_TRUE( !dns.domain.empty() );
	ASSERT_TRUE( !dns.servers.empty() );

#ifdef PRINT_INFO
	printf( "Default domain name: %s\n", dns.domain.c_str() );
	printf( "DNS Servers:\n" );
	BOOST_FOREACH( const std::string &srv, dns.servers ) {
		printf( "\tDNS: %s\n", srv.c_str() );
	}
#endif

	delete net;
}

TEST( NetConfig, isLoopback_ipv4 ) {
	util::net::AddressInfo netAddr;
	util::net::ipv4::SockAddr ipv4( "127.0.0.1" );
	netAddr.addr = ipv4.addr();
	util::net::Adapter adapter;
	adapter.name = "lo0";
	ASSERT_FALSE( util::net::isLoopback(adapter) );
	adapter.addresses.push_back( netAddr );
	ASSERT_TRUE( util::net::isLoopback(adapter) );
}

TEST( NetConfig, DISABLED_isLoopback_ipv6 ) {
}

TEST( NetConfig, adapters ) {
	util::net::Config *net = util::net::Config::create();
	util::net::Adapters adapters;
	ASSERT_TRUE( net->getAdapters(adapters) );
	ASSERT_TRUE( !adapters.empty() );
	ASSERT_TRUE( !adapters[0].name.empty() );

#ifdef PRINT_INFO
	printf( "Adapters:\n" );
	BOOST_FOREACH( const util::net::Adapter &a, adapters ) {
		printf( "\tAdapter: name=%s addresses=%zu\n",
			a.name.c_str(), a.addresses.size() );
		BOOST_FOREACH( const util::net::AddressInfo &addr, a.addresses ) {
			printf( "\t\tAddr: addr=%s, netmask=%s\n",
				addr.addr.asString().c_str(), addr.netmask.asString().c_str() );
		}
	}
#endif

	delete net;
}

TEST( NetConfig, routes ) {
	util::net::Config *net = util::net::Config::create();
	util::net::Routes routes;
	ASSERT_TRUE( net->getRoutes(routes) );
	ASSERT_TRUE( !routes.empty() );
	ASSERT_TRUE( !routes[0].adapter.empty() );

#ifdef PRINT_INFO
	printf( "Routes:\n" );
	BOOST_FOREACH( const util::net::RouteInfo &r, routes ) {
		printf( "\tRoute: dst=%s, gateway=%s, adapter=%s, src=%s\n",
			r.dst.asString().c_str(),
			r.gateway.asString().c_str(),
			r.adapter.c_str(),
			r.src.asString().c_str()
		);
	}
#endif

	delete net;
}

TEST( NetConfig, default_gw ) {
	util::net::Config *net = util::net::Config::create();
	std::string gw = net->defaultGateway();
	ASSERT_TRUE( !gw.empty() );
	delete net;
}
