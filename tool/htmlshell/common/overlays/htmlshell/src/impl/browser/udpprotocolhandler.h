#pragma once

#include "net/url_request/url_request_job_factory.h"
#include <map>

namespace net {
class IPEndPoint;
class UDPServerSocket;
}

namespace tvd {

class UDPProtocolHandler : public net::URLRequestJobFactory::ProtocolHandler {
public:
	~UDPProtocolHandler() override {};

	net::URLRequestJob *MaybeCreateJob( net::URLRequest *request, net::NetworkDelegate *network_delegate ) const override;

	net::UDPServerSocket *getServerSocket( const net::IPEndPoint &address );
	void unrefServerSocket( const std::string &address );

protected:
	struct SocketRefS {
		int refs;
		net::UDPServerSocket *socket;
	};
	typedef struct SocketRefS SocketRef;
	typedef std::map<std::string, SocketRef> SocketMap;

private:
	SocketMap _sockets;
};

}
