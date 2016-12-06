#include "udpprotocolhandler.h"
#include "udpurlrequestjob.h"
#include "../../util.h"
#include "net/base/net_errors.h"
#include "net/udp/udp_server_socket.h"

namespace tvd {

net::URLRequestJob *UDPProtocolHandler::MaybeCreateJob( net::URLRequest *request, net::NetworkDelegate *network_delegate ) const {
	TVD_REQUIRE_IOT();
	return new UdpUrlRequestJob(request, network_delegate, const_cast<UDPProtocolHandler*>(this));
}

net::UDPServerSocket *UDPProtocolHandler::getServerSocket( const net::IPEndPoint &address ) {
	std::string sAddress = address.ToString();
	LOG(INFO) << "UDPProtocolHandler::getServerSocket(" << sAddress << ")";

	SocketRef sock = {0, nullptr};
	auto res = _sockets.insert(std::make_pair(sAddress, sock));
	res.first->second.refs++;
	if (res.second) {
		// No UDPServerSocket exist for address
		res.first->second.socket = new net::UDPServerSocket(nullptr, net::NetLog::Source());
	} else {
		// Found existing UDPServerSocket for address
		res.first->second.socket->Close();
	}
	if (res.first->second.socket->Listen(address) == net::OK) {
		return res.first->second.socket;
	}
	return nullptr;
}

void UDPProtocolHandler::unrefServerSocket( const std::string &address ) {
	LOG(INFO) << "UDPProtocolHandler::unrefServerSocket(" << address << ")";
	auto it = _sockets.find(address);
	if (it != _sockets.end()) {
		if (--it->second.refs <= 0) {
			it->second.socket->Close();
			delete it->second.socket;
			_sockets.erase(it);
		}
	}
}

}
