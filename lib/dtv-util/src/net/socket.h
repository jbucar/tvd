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

#pragma once

#include "types.h"
#include "sockaddr.h"

namespace util {
namespace net {

#define MAX_CONN_QUEUE	5
  
/**
  * La clase \a Socket provee métodos y propiedades para comunicarse con otro socket a través de la red. 
  * Permite realizar conexiones tanto sincrónicas como asincrónicas.
  */
class Socket {
public:
	Socket();
	virtual ~Socket();

	bool create( enum type::type type=type::stream, int protocol=0 );
	bool shutdown( sd::type how=sd::both );
	bool close();
	bool isOpen()const;
	bool bind( SockAddr &addr );
	bool listen( int maxConn=MAX_CONN_QUEUE );
	bool connect( const SockAddr &addr );
	bool accept( Socket *sock );

	SSIZE_T send( const void *msg, size_t size );
	SSIZE_T sendto( const void *msg, size_t len, int flags, const SockAddr &to );
	SSIZE_T recv( void *buf, size_t size );
	SSIZE_T recvfrom( void *buf, size_t size );
	SSIZE_T recvfrom( void *buf, size_t size, SockAddr &from );

	bool local( SockAddr &addr );
	bool peer( SockAddr &addr );

	SOCKET fd() const;

	//	multicast
	//bool addMembership( const SockAddr &addr, DWORD addrIface=0 );
	//bool leaveMembership( const SockAddr &addr, DWORD addrIface=0 );

	//	options
	bool setRcvBuf( int size );
	bool getRcvBuf( int &size );
	
	bool setSndBuf( int size );
	bool getSndBuf( int &size );

	bool setSndTimeout( int timeout );
	bool getSndTimeout( int &timeout );

	bool setRcvTimeout( int timeout );
	bool getRcvTimeout( int &timeout );

	bool setTCPNoDelay( bool noDelay );
	bool getTCPNoDelay( bool &noDelay );

	bool setReuseAddr( bool reuse );
	bool getReuseAddr( bool &reuse );

	bool setAllowBroadcast( bool allowBroadcast );
	bool getAllowBroadcast( bool &allowBroadcast );

	bool setNonBlocking( bool enable );

protected:
	//	Set socket option
	bool openSocket( type::type type, int protocol=0 );
	bool setOptions( int level, int opt, const void *optVal, size_t optLen );
	bool getOptions( int level, int opt, void *optVal, size_t optLen );

private:
	friend class SocketEvent;
	SOCKET _hSocket;
};

template<class T>
T *createServer( type::type type, SockAddr &addr ) {
	T *sock = new T();
	if (sock) {
		bool result=sock->create( type );
		if (result) {
			result=sock->bind( addr );
		}

		if (!result) {
			sock->close();
			delete sock;
			sock=NULL;
		}
	}
	return sock;
}

template<class T>
T *createTCPServer( SockAddr &addr, int queue=5 ) {
	T *sock=createServer<T>( type::stream, addr );
	if (sock && !sock->listen( queue )) {
		sock->close();
		delete sock;
		sock=NULL;
	}
	return sock;
}

}	//	net
}	//	util

