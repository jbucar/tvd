#include "udpurlrequestjob.h"
#include "base/strings/string_number_conversions.h"
#include "net/base/net_errors.h"
#include "net/base/net_util.h"
#include "net/http/http_util.h"
#include "net/udp/udp_server_socket.h"

namespace tvd {

UdpUrlRequestJob::UdpUrlRequestJob( net::URLRequest *request, net::NetworkDelegate *network_delegate, UDPProtocolHandler *handler )
	: net::URLRequestJob(request, network_delegate), _handler(handler), _weakPtrFactory(this)
{
	_socket = nullptr;
	_killed = false;
}

UdpUrlRequestJob::~UdpUrlRequestJob()
{
}

void UdpUrlRequestJob::Start() {
	GURL url = request()->original_url();
	LOG(INFO) << "UdpUrlRequestJob[" << this << "]::" << __FUNCTION__ << "(" << url << ")";

	// Get host from url and convert to ip address
	net::IPAddressNumber ipAddr;
	if (!net::ParseIPLiteralToNumber(url.host(), &ipAddr)) {
		LOG(WARNING) << "Fail to convert host=" << url.host() << " to ip address";
		NotifyStartError(net::URLRequestStatus(net::URLRequestStatus::FAILED, net::ERR_ADDRESS_INVALID));
	}

	// Get port from url and convert to int
	int port=-1;
	base::StringToInt(url.port(), &port);

	// Create udp server socket
	_address = net::IPEndPoint(ipAddr, port);
	_sAddress = _address.ToString();
	_socket = _handler->getServerSocket(_address);

	if (!_socket) {
		LOG(WARNING) << "Fail to bind server socket";
		NotifyStartError(net::URLRequestStatus(net::URLRequestStatus::FAILED, net::ERR_SOCKET_NOT_CONNECTED));
	} else {
		LOG(INFO) << "UDP server binded to: " << _address.ToString();
		set_expected_content_size(1078987084);
		NotifyHeadersComplete();
	}
}

bool UdpUrlRequestJob::GetMimeType(std::string* mime_type) const {
	*mime_type = "video/mp2t";
	return true;
}

void UdpUrlRequestJob::Kill() {
	LOG(INFO) << "UdpUrlRequestJob[" << this << "]::" << __FUNCTION__ << "()";
	if (!_killed) {
		_handler->unrefServerSocket(_sAddress);
		_weakPtrFactory.InvalidateWeakPtrs();
		URLRequestJob::Kill();
		_killed = true;
	}
}

int UdpUrlRequestJob::ReadRawData( net::IOBuffer *buf, int buf_size ) {
	return _killed ? net::ERR_SOCKET_NOT_CONNECTED
	               : _socket->RecvFrom(buf, buf_size, &_address, base::Bind(&UdpUrlRequestJob::ReadRawDataComplete, _weakPtrFactory.GetWeakPtr()));
}

}
