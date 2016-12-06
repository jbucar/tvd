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

#include "socket.h"
#include "../registrator.h"
#include "../assert.h"
#include "../log.h"
#include "../mcr.h"

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	define SOCKET_REQUESTED_HI     2
#	define SOCKET_REQUESTED_LOW    2
#	define socklen_t               int
#	define SOCK                    (::SOCKET(_hSocket))
#	define LOG_SOCKET_ERR_AUX(err) LERROR( "net", "API socket error: %d", err );
#	define LOG_SOCKET_ERR          LOG_SOCKET_ERR_AUX( ::WSAGetLastError() );
#else
#	include <sys/socket.h>
#	include <sys/ioctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <errno.h>

#	define INVALID_SOCKET -1
#	define SOCKET_ERROR   -1
#	define SOCK           (SOCKET(_hSocket))
#	define LOG_SOCKET_ERR LERROR( "net", "API socket error: %d", errno );
#endif
#define LOG_ERR          if (!result) { LOG_SOCKET_ERR; }

namespace util {
namespace net {

static void static_init() {
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( SOCKET_REQUESTED_HI, SOCKET_REQUESTED_LOW );
	err=::WSAStartup( wVersionRequested, &wsaData );
	if (err) {
		LOG_SOCKET_ERR;
	}
	else {
		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */
		if ( LOBYTE( wsaData.wVersion ) != SOCKET_REQUESTED_LOW ||
			HIBYTE( wsaData.wVersion ) != SOCKET_REQUESTED_HI )
		{
			::WSACleanup( );
			LERROR( "net", "Socket API incompatible." );
		}
	}
#endif
}

static void static_fin() {
#ifdef _WIN32
	::WSACleanup();
#endif
}

REGISTER_INIT( net ) {
	util::net::static_init();
}

REGISTER_FIN( net ) {
	util::net::static_fin();
}

/**
 * Constructor de la clase.
 */
Socket::Socket()
{
	_hSocket = INVALID_SOCKET;
}

/**
 * Destructor de la clase.
 */
Socket::~Socket()
{
	if (isOpen()) {
		close();
		_hSocket = INVALID_SOCKET;
	}
}

/**
  * Crea un socket.
  * @param type El tipo de socket.
  * @param protocol El protocolo a usar.
  * @return true en caso de poder crearlo, false en caso contrario.
  */
bool Socket::create( enum type::type type/*=stream*/, int protocol/*=0*/ ) {
	bool result=openSocket( type, protocol );
	LOG_ERR;
	return result;
}

/**
 * Realiza el shutdown del socket. 
 * @param how Indica de que forma realizarlo.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::shutdown( sd::type how/*=sd::both*/ ) {
	DTV_ASSERT(isOpen());

	bool result=::shutdown( SOCK, how ) == 0;
	LOG_ERR;
	return result;
}

/**
 * Cierra el socket.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::close() {
	DTV_ASSERT(isOpen());
#ifdef _WIN32
	bool result=(::closesocket(SOCK) != SOCKET_ERROR);
	LOG_ERR;
	return result;
#else
	return (::close(SOCK) != SOCKET_ERROR);
#endif
}

/**
 * @return Booleano indicando si el socket esta abierto o no.
 */
bool Socket::isOpen() const {
	return _hSocket != INVALID_SOCKET;
}

/**
 * Se indica la ip a la cual hacer el binding.
 * @param bindAddress El socket al cual hacer el binding.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::bind( SockAddr &bindAddress ) {
	DTV_ASSERT(isOpen());
	bool result=::bind( SOCK, bindAddress.addr(), bindAddress.len() ) != SOCKET_ERROR;
	LOG_ERR;
	return result;
}

/**
 * Se prepara para aceptar conexiones entrantes.
 * @param maxConn Indica las cantidad máxima de conexiones entrantes que pueden ser encoladas.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::listen( int maxConn/*=MAX_CONN_QUEUE*/ ) {
	DTV_ASSERT(isOpen());

	bool result=::listen(SOCK, maxConn) != SOCKET_ERROR;
	LOG_ERR;
	return result;
}

/**
 * Se conecta con un socket.
 * @param connectAddress Indicada el socket al cual conectarse.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::connect( const SockAddr &peer ) {
	DTV_ASSERT(isOpen());
	bool result=::connect( SOCK, peer.addr(), peer.len() ) != SOCKET_ERROR;
	LOG_ERR;
	return result;
}

/**
 * Retorna en @b sConnect el socket correspondiente al primer pedido de conección
 * en la cola de conecciones pendientes del @c Socket
 * @param[out] sConnect El socket de la conección aceptada.
 * @return true si la conección fue exitosa, false en caso contrario.
 */
bool Socket::accept( Socket *sConnect ) {
	DTV_ASSERT(isOpen());
	if (!sConnect || sConnect->_hSocket != INVALID_SOCKET) {
		return false;
	}
	struct sockaddr a;
	socklen_t nLengthAddr = sizeof(struct sockaddr);
	sConnect->_hSocket = (SOCKET) ::accept(SOCK, (struct sockaddr *)&a, &nLengthAddr);
	bool result=sConnect->_hSocket != INVALID_SOCKET;
	LOG_ERR;
	return result;
}

/**
 * Devuelve la dirección del socket al cual se esta conectado.
 * @param[out] peerAddress Dirección del socket al cual se esta conectado.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::peer( SockAddr &peerAddress ) {
	DTV_ASSERT(isOpen());

	struct sockaddr_storage addr;
	socklen_t nLengthAddr=sizeof(addr);
	bool result=::getpeername( SOCK, (struct sockaddr *)&addr, &nLengthAddr ) != SOCKET_ERROR;
	if (result) {
		peerAddress = SockAddr( (struct sockaddr *)&addr, nLengthAddr );
	}
	LOG_ERR;
	return result;
}

/**
 * Devuelve la dirección del socket local.
 * @param[out] localAddress Dirección del socket local.
 * @return true en caso de haber realizado la operación de forma exitosa, false en caso contrario.
 */
bool Socket::local( SockAddr &localAddress ) {
	DTV_ASSERT(isOpen());

	struct sockaddr_storage addr;
	socklen_t nLengthAddr;
	nLengthAddr=sizeof(addr);
	bool result=::getsockname(SOCK, (struct sockaddr *)&addr, &nLengthAddr) != SOCKET_ERROR;
	if (result) {
		localAddress = SockAddr( (struct sockaddr *)&addr, nLengthAddr );
	}
	LOG_ERR;
	return result;
}

/**
 * Envía un mensaje.
 * @param msg El mensaje a enviar.
 * @param len La longitud de msg.
 * @return Los datos envíados o -1 en caso de error.
 */
SSIZE_T Socket::send( const void *msg, size_t len ) {
	return ::send( SOCK, (const char *)msg, len, 0 );
}

/**
 * Envía un mensaje a una dirección específica.
 * @param msg El mensaje a enviar.
 * @param len La longitud del mensaje.
 * @param flags Dependiente de la plataforma.
 * @param to Dirección destino.
 * @return La cantidad de bytes enviados o -1 en caso de error.
 */
SSIZE_T Socket::sendto( const void *msg, size_t len, int flags, const SockAddr &to ) {
	return ::sendto( SOCK, (const char *)msg, len, flags, to.addr(), to.len() );
}

SSIZE_T Socket::recv( void *buf, size_t size ) {
	return ::recv( SOCK, (char *)buf, size, 0 );
}

SSIZE_T Socket::recvfrom( void *buf, size_t size ) {
	return ::recvfrom( SOCK, (char *)buf, size, 0, NULL, 0 );
}

SSIZE_T Socket::recvfrom( void *buf, size_t size, SockAddr &from ) {
	struct sockaddr_storage priv;
	socklen_t nLengthAddr=sizeof(priv);
	int ret=::recvfrom( SOCK, (char *)buf, size, 0, (struct sockaddr *)&priv, &nLengthAddr );
	if (ret > 0) {
		from = SockAddr( (struct sockaddr *)&priv, nLengthAddr );
	}
	return ret;
}

/**
 * @return El descriptor del socket.
 */
SOCKET Socket::fd() const {
	return _hSocket;
}

////	multicast
//bool Socket::addMembership( const SockAddr &addr, DWORD addrIface/*=INADDR_ANY*/ ) {
//#ifdef _WIN32
//	struct ip_mreq_source mreq;
//#else
//	struct ip_mreq mreq;
//#endif
//
//	memset(&mreq, 0, sizeof(mreq));
//	mreq.imr_multiaddr.s_addr=addr;
//    mreq.imr_interface.s_addr=htonl(addrIface);
//	return setOptions( IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq) );
//}
//
//bool Socket::leaveMembership( const SockAddr &addr, DWORD addrIface/*=INADDR_ANY*/ ) {
//#ifdef _WIN32
//	struct ip_mreq_source mreq;
//#else
//	struct ip_mreq mreq;
//#endif
//
//	memset(&mreq, 0, sizeof(mreq));
//	mreq.imr_multiaddr.s_addr=addr;
//    mreq.imr_interface.s_addr=htonl(addrIface);
//	return setOptions( IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq) );
//}

//	options
bool Socket::setRcvBuf( int size ) {
	return setOptions( SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
}

bool Socket::getRcvBuf( int &size ) {
	size=0;
	return getOptions( SOL_SOCKET, SO_RCVBUF, &size, sizeof(size) );
}

bool Socket::setSndBuf( int size ) {
	return setOptions( SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
}

bool Socket::getSndBuf( int &size ) {
	return getOptions( SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
}

bool Socket::setSndTimeout( int timeout ) {
	return setOptions( SOL_SOCKET, SO_SNDBUF, &timeout, sizeof(timeout));
}

bool Socket::getSndTimeout( int &timeout ) {
	return getOptions( SOL_SOCKET, SO_SNDBUF, &timeout, sizeof(timeout));
}

bool Socket::setRcvTimeout( int timeout ) {
	return setOptions( SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

bool Socket::getRcvTimeout( int &timeout ) {
	return getOptions( SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

bool Socket::setTCPNoDelay( bool noDelay ) {
	int tcpNoDelay=noDelay;
	return setOptions( IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(int));
}

bool Socket::getTCPNoDelay( bool &noDelay ) {
	bool result;
	int tcpNoDelay=noDelay;
	result=setOptions( IPPROTO_TCP, TCP_NODELAY, &tcpNoDelay, sizeof(int));
	noDelay=tcpNoDelay ? true : false;
	return result;
}

bool Socket::setReuseAddr( bool reuse ) {
	int reuseAddr=reuse;
	return setOptions( SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));
}

bool Socket::getReuseAddr( bool &reuse ) {	
	int reuseAddr=reuse;
	bool result=setOptions( SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));
	reuse = reuseAddr ? true : false;
	return result;
}

bool Socket::setAllowBroadcast( bool allowBroadcast ) {
	int allow=allowBroadcast;
	return setOptions( SOL_SOCKET, SO_BROADCAST, (const char *)&allow, sizeof(int));
}

bool Socket::getAllowBroadcast( bool &allowBroadcast ) {
	int allow=allowBroadcast;
	bool result=setOptions( SOL_SOCKET, SO_BROADCAST, (const char *)&allow, sizeof(int));
	allow = allowBroadcast ? true : false;
	return result;
}

bool Socket::setNonBlocking( bool enable ) {
	DTV_ASSERT(isOpen());
	
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.

#ifdef _WIN32
	u_long iMode=enable ? 1 : 0;
	bool result= ::ioctlsocket( SOCK, FIONBIO, &iMode ) != SOCKET_ERROR;
#else
	DWORD iMode=enable ? 1 : 0;
	bool result= ::ioctl( SOCK, FIONBIO, &iMode ) != SOCKET_ERROR;
#endif
	LOG_ERR;
	return result;
}

//	Aux
bool Socket::openSocket( type::type type, int protocol/*=0*/ ) {
#ifdef _WIN32
	_hSocket = (SOCKET)::WSASocket( AF_INET, type, protocol, 0, 0, WSA_FLAG_OVERLAPPED );
#else
	_hSocket = (SOCKET)::socket(AF_INET, type, protocol );
#endif
	return _hSocket != INVALID_SOCKET;
}

bool Socket::setOptions( int level, int opt, const void *optVal, size_t optLen ) {
	DTV_ASSERT(isOpen());

	bool result=::setsockopt( SOCK, level, opt, (const char *)optVal, optLen ) != SOCKET_ERROR;
	LOG_ERR;
	return result;
}

bool Socket::getOptions( int level, int opt, void *optVal, size_t optLen ) {
	DTV_ASSERT(isOpen());

	socklen_t len=optLen;
	bool result=::getsockopt( SOCK, level, opt, (char *)optVal, &len ) != SOCKET_ERROR && (socklen_t)optLen == len;
	LOG_ERR;
	return result;
}

}
}

