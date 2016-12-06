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
#include "keyeventhandler.h"
#include "../connector.h"
#include <util/buffer.h>

namespace connector {

KeyEventHandler::KeyEventHandler( void )
{
}

KeyEventHandler::~KeyEventHandler( void )
{
}

#define KEY_BUFFER (MESSAGE_HEADER+sizeof(util::DWORD)+1)
void KeyEventHandler::send( Connector *conn, util::key::type key, bool isUp ) {
	util::BYTE buf[KEY_BUFFER];
	int pos=MESSAGE_HEADER;

	{	//	Copy Key
		//	Convert to DWORD (4 bytes)
		util::DWORD value = static_cast<util::DWORD>( key );
		int index=0;
		util::BYTE *ptr = (util::BYTE *)&value;

		//	Copy 4 bytes
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
	}

	//	Copy state
	buf[pos++] = isUp ? 1 : 0;

	MAKE_MSG(buf,messages::keyEvent,pos);
	util::Buffer msg( (char *)buf, pos, false );
	conn->send( &msg );
}

void KeyEventHandler::onKeyEvent( const KeyEventCallback &callback ) {
	_onKeyEvent = callback;
}

void KeyEventHandler::process( util::Buffer *msg ) {
	util::BYTE *payload = MSG_PAYLOAD(msg->bytes());
	util::DWORD value   = GET_BE_DWORD( payload );
	util::BYTE isUp     = GET_BYTE(payload+4);
	dispatch( boost::bind(_onKeyEvent,static_cast<util::key::type>( value ), (isUp == 1) ) );
}

messages::type KeyEventHandler::type() const {
	return messages::keyEvent;
}

}
