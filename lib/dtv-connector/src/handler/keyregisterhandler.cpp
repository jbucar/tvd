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
#include "keyregisterhandler.h"
#include "../connector.h"
#include <util/buffer.h>
#include <util/assert.h>
#include <boost/foreach.hpp>

#define RELEASE_KEYS 0
#define RESERVE_KEYS 1

namespace connector {

KeyRegisterHandler::KeyRegisterHandler( void )
{
}

KeyRegisterHandler::~KeyRegisterHandler( void )
{
}

#define MAX_KEYS 200
#define KEY_BUF_SIZE ((200*4)+4)
void KeyRegisterHandler::send( Connector *conn, const std::set< util::key::type > &keys ) {
	util::BYTE buf[KEY_BUF_SIZE];
	int pos=MESSAGE_HEADER;
	int index;
	util::BYTE *ptr;
	bool writeList;
	util::DWORD len;

	DTV_ASSERT( keys.size() < MAX_KEYS );

	if (keys.size() == ::util::key::LAST-1) {
		len = util::DWORD(-1);
		writeList=false;
	}
	else {
		len = keys.size();
		writeList=true;
	}

	{	//	Write len
		index=0;
		ptr = (util::BYTE *)&len;
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
		buf[pos++] = ptr[index++];
	}

	if (writeList) {
		//	Make payload
		BOOST_FOREACH( const util::key::type &elem, keys ) {
			util::DWORD value = (util::DWORD) elem;
			index=0;
			ptr = (util::BYTE *)&value;

			buf[pos++] = ptr[index++];
			buf[pos++] = ptr[index++];
			buf[pos++] = ptr[index++];
			buf[pos++] = ptr[index++];
		}
	}

	MAKE_MSG(buf,messages::keyRegister,pos);
	util::Buffer msg( (char *)buf, pos, false );
	conn->send( &msg );
}

void KeyRegisterHandler::onKeyEvent( const KeyRegisterCallback &callback ) {
	_onRegister = callback;
}

void KeyRegisterHandler::process( util::Buffer *msg ) {
	if (!_onRegister.empty()) {
		KeyRegisterDataPtr msgData( new KeyRegisterData() );
		util::BYTE *data = msg->bytes();
		size_t msgSize   = MSG_PAYLOAD_SIZE(data);
		util::BYTE *payload = MSG_PAYLOAD(data);
		size_t offset = 0;
		util::BYTE *ptr = payload+offset;

		//	Read len
		util::DWORD len = GET_BE_DWORD( ptr );
		ptr += sizeof(util::DWORD);
		offset += sizeof(util::DWORD);

		if (len == util::DWORD(-1)) {
#define SAVE_KEY( c, n, u ) msgData->insert( static_cast<util::key::type>(util::key::c) );
			UTIL_KEY_LIST(SAVE_KEY);
#undef SAVE_KEY
		}
		else {
			//	Key event
			while (offset < msgSize) {
				// Fixes Unaligned userspace access
				util::DWORD ins = GET_BE_DWORD( ptr );
				ptr += sizeof(util::DWORD);
				offset+=sizeof(util::DWORD);

				msgData->insert( (util::key::type) ins );
			}
		}

		dispatch( boost::bind(_onRegister,msgData) );
	}
}

messages::type KeyRegisterHandler::type() const {
	return messages::keyRegister;
}

}
