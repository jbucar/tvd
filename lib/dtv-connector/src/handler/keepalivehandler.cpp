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
#include "keepalivehandler.h"
#include "../connector.h"
#include <util/buffer.h>
#include <util/types.h>

namespace connector {

KeepAliveHandler::KeepAliveHandler( void )
{
}

KeepAliveHandler::~KeepAliveHandler( void )
{
}

void KeepAliveHandler::send( Connector *conn ) {
	util::BYTE buf[10];
	int pos=MESSAGE_HEADER;

	buf[pos++] = 0xab;
	buf[pos++] = 0xba;
	buf[pos++] = 0xcd;
	buf[pos++] = 0xde;
	MAKE_MSG(buf,messages::keepAlive,pos);

	util::Buffer msg( (const char *)buf, pos, false );
	conn->send( &msg );
}

void KeepAliveHandler::onKeepAliveEvent( const KeepAliveCallback &callback ) {
	_onKeepAlive = callback;
}

void KeepAliveHandler::process( util::Buffer *msg ) {
	if (!_onKeepAlive.empty()) {
		KeepAliveData msgData;
		util::BYTE *payload  = MSG_PAYLOAD(msg->bytes());
		util::SSIZE_T offset = 0;

		msgData.memory = RDW(payload,offset);

		_onKeepAlive( &msgData );
	}
}

messages::type KeepAliveHandler::type() const {
	return messages::keepAlive;
}

}
