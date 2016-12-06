#include "tcpserversocketfactory.h"
#include "net/base/net_errors.h"
#include "net/socket/tcp_server_socket.h"

namespace tvd {

static int kBackLog = 10;

TCPServerSocketFactory::TCPServerSocketFactory( const std::string &address, uint16_t port )
	: _address(address), _port(port)
{}

scoped_ptr<net::ServerSocket> TCPServerSocketFactory::CreateForHttpServer() {
	scoped_ptr<net::ServerSocket> socket(new net::TCPServerSocket(nullptr, net::NetLog::Source()));
	if (socket->ListenWithAddressAndPort(_address, _port, kBackLog) != net::OK) {
		return scoped_ptr<net::ServerSocket>();
	}
	return socket;
}

}
