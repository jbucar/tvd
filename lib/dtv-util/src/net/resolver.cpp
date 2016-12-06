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

#include "../log.h"
#include "sockaddr.h"
#include <vector>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#else
#	include <arpa/inet.h>
#	include <netdb.h>
#endif
#include <string.h>

namespace util {
namespace net {

bool resolve(
	std::vector<util::net::SockAddr> &addresses,
	const std::string &host,
	const std::string &port/*=""*/,
	family::type family/*=family::any*/,
	type::type socket_type/*=type::any*/
)
{
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = getFamily(family);
	hints.ai_socktype = socket_type;
	int error = ::getaddrinfo( host.c_str(), port.empty() ? NULL : port.c_str(), &hints, &result );
	if (error != 0) {
		LERROR( "resolver", "getaddrinfo failed: error=%s", gai_strerror(error) );
		return false;
	} else {
		for (struct addrinfo *res = result; res != NULL; res = res->ai_next) {
			addresses.push_back( SockAddr( res->ai_addr, res->ai_addrlen ) );
		}
	}
	::freeaddrinfo( result );

	return true;
}

}	//	net
}	//	util
