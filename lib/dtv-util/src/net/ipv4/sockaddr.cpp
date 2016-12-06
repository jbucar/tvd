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
#include "../../string.h"
#include "../../assert.h"
#include <boost/lexical_cast.hpp>
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
namespace ipv4 {

SockAddr::SockAddr()
{
	_addr = INADDR_ANY;
	_port = 0;
}

SockAddr::SockAddr( const SockAddr &other )
{
	_addr = other._addr;
	_port = other._port;
}

SockAddr::SockAddr( const net::SockAddr &addr )
{
	struct sockaddr_in *saddr_in=(struct sockaddr_in *)addr.addr();
	DTV_ASSERT( saddr_in->sin_family == AF_INET );
	_addr = saddr_in->sin_addr.s_addr;
	_port = ntohs(saddr_in->sin_port);
}

SockAddr::SockAddr( unsigned short port )
{
	_port = port;
}

SockAddr::SockAddr( const std::string &ip, unsigned short port/*=0*/ )
{
	_addr = ::inet_addr(ip.c_str());
	_port = port;
}

SockAddr::~SockAddr()
{
}

//	Operators
bool SockAddr::operator==( const SockAddr &other ) const {
	return _addr == other._addr && _port == other._port;
}

//  Getters
net::SockAddr SockAddr::addr() const {
	struct sockaddr_in saddr_in;
	saddr_in.sin_family = AF_INET;
	saddr_in.sin_addr.s_addr = _addr;
	saddr_in.sin_port = htons(_port);
	return net::SockAddr( (struct sockaddr *)&saddr_in, sizeof(saddr_in) );
}

std::string SockAddr::asString() const {
	in_addr addr;
	addr.s_addr = _addr;
	std::string tmp = ::inet_ntoa(addr);
	if (_port) {
		tmp += ":";
		tmp += boost::lexical_cast<std::string>(_port);
	}
	return tmp;
}

}
}
}

