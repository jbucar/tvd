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

#include "sockaddr.h"
#include "../../assert.h"
#include <string.h>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <netdb.h>
#endif

namespace util {
namespace net {
namespace ipv6 {

SockAddr::SockAddr()
{
	memset(_addr, 0, sizeof(_addr));
	_port = 0;
}

SockAddr::SockAddr( const std::string &ipv6, unsigned short port )
	: _port(port)
{
	struct in6_addr addr;
	::inet_pton( AF_INET6, ipv6.c_str(), &addr );
	memcpy(_addr,addr.s6_addr,16);
}

SockAddr::SockAddr( const SockAddr &other )
{
	memcpy(_addr,other._addr,16);
	_port = other._port;
}

SockAddr::SockAddr( const net::SockAddr &addr )
{
 	struct sockaddr *sa = addr.addr();
	struct sockaddr_in6 *saddr=(struct sockaddr_in6 *)sa;
	DTV_ASSERT( saddr->sin6_family == AF_INET6 );
	memcpy(_addr,saddr->sin6_addr.s6_addr,16);
	_port = ntohs(saddr->sin6_port);
}

SockAddr::~SockAddr()
{
}

bool SockAddr::operator==( const SockAddr &other ) const {
	return _port == other._port && !memcmp(_addr,other._addr,16);
}

//  Getters
std::string SockAddr::asString() const {
	char straddr[INET6_ADDRSTRLEN];
	struct in6_addr sAddr;
	memcpy(sAddr.s6_addr,_addr,16);
	::inet_ntop( AF_INET6, &sAddr, straddr, sizeof(straddr) );
	return std::string(straddr);
}

net::SockAddr SockAddr::addr() const {
	struct sockaddr_in6 saddr_in6;
	saddr_in6.sin6_family = AF_INET6;
	saddr_in6.sin6_port = htons(_port); /* Transport layer port # */
	saddr_in6.sin6_flowinfo = 0;        /* IPv6 flow information */
	memcpy(saddr_in6.sin6_addr.s6_addr,_addr,16);
	saddr_in6.sin6_scope_id = 0;        /* IPv6 scope-id */
	return net::SockAddr( (struct sockaddr *)&saddr_in6, sizeof(saddr_in6) );
}

}
}
}

