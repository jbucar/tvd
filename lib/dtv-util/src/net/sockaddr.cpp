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
#include "ipv4/sockaddr.h"
#include "ipv6/sockaddr.h"
#include "../assert.h"
#include <boost/static_assert.hpp>
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

SockAddr::SockAddr()
{
	BOOST_STATIC_ASSERT(sizeof(_sa) >= sizeof(struct sockaddr_storage));
	memset(_sa,0,sizeof(_sa));
	_len = 0;
}

SockAddr::SockAddr( const SockAddr &sa )
{
	BOOST_STATIC_ASSERT(sizeof(_sa) >= sizeof(struct sockaddr_storage));
	memcpy(_sa,sa._sa,sa._len);
	_len = sa._len;
}

SockAddr::SockAddr( struct sockaddr *sa, size_t len )
{
	BOOST_STATIC_ASSERT(sizeof(_sa) >= sizeof(struct sockaddr_storage));
	memcpy(_sa,sa,len);
	_len = len;
}

SockAddr::~SockAddr()
{
}

SockAddr &SockAddr::operator=( const SockAddr &sa ) {
	memcpy(_sa,sa._sa,sizeof(_sa));
	_len = sa._len;
	return *this;
}

bool SockAddr::operator==( const SockAddr &other ) const {
	bool ret=false;
	if (_len == other._len && addr()->sa_family == other.addr()->sa_family) {
		switch (addr()->sa_family) {
			case AF_INET: {
				ipv4::SockAddr addr1(*this);
				ipv4::SockAddr addr2(other);
				ret = addr1 == addr2;
				break;
			}
			case AF_INET6: {
				ipv6::SockAddr addr1(*this);
				ipv6::SockAddr addr2(other);
				ret = addr1 == addr2;
				break;
			}
			default: {
				break;
			}
		};
	}
	return ret;
}

struct sockaddr *SockAddr::addr() const {
	struct sockaddr *tmp = (struct sockaddr *)_sa;
	return tmp;
}

size_t SockAddr::len() const {
	return _len;
}

//	Methods
std::string SockAddr::asString() const {
	std::string tmp;
	switch (addr()->sa_family) {
		case AF_INET: {
			ipv4::SockAddr addr(*this);
			tmp = addr.asString();
			break;
		}
		case AF_INET6: {
			ipv6::SockAddr addr(*this);
			tmp = addr.asString();
			break;
		}
		default: {
			tmp = "Unknown";
			break;
		}
	};

	return tmp;
}

}
}

