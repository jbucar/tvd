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
#include "buttoneventhandler.h"
#include "../connector.h"
#include <util/buffer.h>

namespace connector {

ButtonEventHandler::ButtonEventHandler( void )
{
}

ButtonEventHandler::~ButtonEventHandler( void )
{
}

#define BUTTON_BUFFER (MESSAGE_HEADER+sizeof(util::DWORD)*3+1)
void ButtonEventHandler::send( Connector *conn, unsigned int button, int x, int y, bool isPress ) {
	util::BYTE buf[BUTTON_BUFFER];
	int pos=MESSAGE_HEADER;

	{	//	Copy button
		//	Convert to DWORD (4 bytes)
		util::DWORD value = static_cast<util::DWORD>( button );
		int index=0;
		util::BYTE *ptr = (util::BYTE *)&value;

		//	Copy 4 bytes
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
	}

	{	//	Copy x
		//	Convert to DWORD (4 bytes)
		util::DWORD value = static_cast<util::DWORD>( x );
		int index=0;
		util::BYTE *ptr = (util::BYTE *)&value;

		//	Copy 4 bytes
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
	}

	{	//	Copy y
		//	Convert to DWORD (4 bytes)
		util::DWORD value = static_cast<util::DWORD>( y );
		int index=0;
		util::BYTE *ptr = (util::BYTE *)&value;

		//	Copy 4 bytes
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
	}

	//	Copy state	
	buf[pos++] = isPress ? 1 : 0;

	MAKE_MSG(buf,messages::buttonEvent,pos);
	util::Buffer msg( (char *)buf, pos, false);
	conn->send( &msg );
}

void ButtonEventHandler::onButtonEvent( const ButtonEventCallback &callback ) {
	_onButtonEvent = callback;
}

void ButtonEventHandler::process( util::Buffer *msg ) {
	util::BYTE *payload = MSG_PAYLOAD(msg->bytes());
	util::DWORD btn	    = GET_BE_DWORD( payload );	
	util::DWORD x	    = GET_BE_DWORD( payload+4 );
	util::DWORD y	    = GET_BE_DWORD( payload+8);	
	util::BYTE  isPress = GET_BYTE(payload+12);
	dispatch( boost::bind(_onButtonEvent, static_cast<unsigned int>( btn ), static_cast<int>( x ), static_cast<int>( y ), (isPress == 1) ) );
}

messages::type ButtonEventHandler::type() const {
	return messages::buttonEvent;
}

}
