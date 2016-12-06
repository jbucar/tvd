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

#include "playvideohandler.h"
#include "../connector.h"
#include <util/buffer.h>

namespace connector {

PlayVideoHandler::PlayVideoHandler()
{
}

PlayVideoHandler::~PlayVideoHandler()
{
}

#define WRITE(val)	\
	memcpy((ptr+offset), &val, sizeof(val) ); \
	offset+=sizeof(val);

void PlayVideoHandler::send( Connector *conn, size_t id, const std::string &url ) {
	char *buf = (char *)malloc(MESSAGE_HEADER+sizeof(id)+url.length()+sizeof(size_t));
	char *ptr = (char *)MSG_PAYLOAD(buf);
	int offset = 0;

	WRITE(id);
	{	//	Write URL
		size_t len = url.length();
		WRITE(len);

		memcpy((ptr+offset), url.c_str(), len );
		offset+=len;
	}
	MAKE_MSG(buf,messages::playVideo,offset+MESSAGE_HEADER);

	util::Buffer msg( buf, offset+MESSAGE_HEADER, false );
	conn->send( &msg );
	free(buf);
}

void PlayVideoHandler::onPlayVideoEvent( const PlayVideoEventCallback &callback ) {
	_onPlayVideoEvent = callback;
}

void PlayVideoHandler::process( util::Buffer *msg ) {
	if (!_onPlayVideoEvent.empty()) {
		util::BYTE *payload = MSG_PAYLOAD(msg->bytes());

		size_t id;
		memcpy( &id, payload, sizeof(id) );
		payload += sizeof(id);

		size_t urlLen;
		memcpy( &urlLen, payload, sizeof(urlLen) );
		payload += sizeof(urlLen);

		char *ptr = (char *)malloc(urlLen);
		memcpy( ptr, payload, urlLen );
		payload += urlLen;

		std::string url(ptr,urlLen);
		dispatch( boost::bind(_onPlayVideoEvent,id,url) );
	}
}

messages::type PlayVideoHandler::type() const {
	return messages::playVideo;
}

}

