/*******************************************************************************

  Copyright (C) 2010, 2011 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-connector implementation.

    DTV-connector is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-connector is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-connector.

    DTV-connector es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-connector se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include "networkfrontend.h"
#include "../../../demuxer/ts/demuxerimpl.h"
#include <util/buffer.h>
#include <util/cfg/configregistrator.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/net/ipv4/sockaddr.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>

namespace tuner {
namespace ts {

NetworkFrontend::NetworkFrontend( demuxer::ts::DemuxerImpl *demux )
	: URLFrontend(demux)
{
}

NetworkFrontend::~NetworkFrontend()
{
}

void NetworkFrontend::stopImpl() {
	//	Cancel socket ...
	_socket.shutdown();
	_socket.close();
}

bool NetworkFrontend::connect( const std::string &url ) {
	std::string host, port;

	bool result = parseUrl(url, &host, &port);
	if (result) {
		util::net::ipv4::SockAddr addr( host, boost::lexical_cast<unsigned short>(port) );
		_socket.create( util::net::type::stream );
		if (!_socket.connect(addr.addr())) {
			_socket.close();
			LERROR( "ts::NetworkFrontend", "Cannot connect to %s", url.c_str() );
			return false;
		}
	}

	return result;
}

//	Running in provider thread
void NetworkFrontend::reader( const std::string net ) {
	LDEBUG( "NetworkFrontend", "Network thread started: net=%s", net.c_str() );
	util::Buffer *buf=NULL;

	bool exit=false;
	while (!exit) {
		//  Get a buffer
		if (!buf) {
			buf = demux()->allocBuffer();
			DTV_ASSERT(buf);
		}

		//  Read from network
		util::SSIZE_T len = _socket.recv( buf->data(), buf->capacity() );
		if (len<0) {
			demux()->freeBuffer( buf );
			exit=true;
		}
		else if (len) {
			//	Enqueue buffer into demuxer
			buf->resize( len );
			demux()->pushData( buf );
			buf = NULL;
		}
	}
	LDEBUG( "ts::NetworkFrontend", "Network thread terminated: net=%s", net.c_str() );
}

}
}
