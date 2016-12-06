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

#include "config.h"
#include "ipv4/sockaddr.h"
#include "../assert.h"
#include "generated/config.h"
#if UTIL_BASE_USE_LINUX
#include "../linux/networkconfig.h"
#endif
#if UTIL_BASE_USE_WINDOWS || UTIL_BASE_USE_MINGW
#include "../win32/networkconfig.h"
#endif
#if UTIL_BASE_USE_ANDROID
#include "../android/networkconfig.h"
#endif

#include <boost/foreach.hpp>

namespace util {
namespace net {

Config *Config::create() {
#if UTIL_BASE_USE_LINUX
	return new NetworkConfig();
#endif

#if UTIL_BASE_USE_WINDOWS || UTIL_BASE_USE_MINGW
	return new NetworkConfig();
#endif

#if UTIL_BASE_USE_ANDROID
	return new NetworkConfig();
#endif

	DTV_ASSERT(false);
	return NULL;
}

bool isLoopback( const Adapter &adapter ) {
	BOOST_FOREACH( const AddressInfo &addr, adapter.addresses ) {
		if (addr.addr.asString() == "127.0.0.1") {
			return true;
		}
	}
	return false;
}

Config::Config()
{
}

Config::~Config()
{
}

bool Config::isConnected() {
	Adapters adapters;
	if (getAdapters(adapters)) {
		bool found=false;
		BOOST_FOREACH( const Adapter &a, adapters ) {
			if (!isLoopback(a)) {
				found=true;
				break;
			}
		}
		return found;
	}
	return false;
}

std::string Config::defaultGateway() {
	std::string tmp;
	util::net::Routes routes;
	if (getRoutes(routes) ) {
		ipv4::SockAddr ipv4Null("0.0.0.0",0);
		SockAddr addr=ipv4Null.addr();
		BOOST_FOREACH( const util::net::RouteInfo &r, routes ) {
			if (r.dst == addr) {
				tmp=r.gateway.asString();
				break;
			}
		}
	}
	return tmp;
}

}
}

