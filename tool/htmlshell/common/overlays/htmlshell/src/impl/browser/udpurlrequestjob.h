#pragma once

#include "udpprotocolhandler.h"
#include "net/base/ip_endpoint.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job.h"

namespace tvd {

class UdpUrlRequestJob : public net::URLRequestJob {
public:
	UdpUrlRequestJob( net::URLRequest *request, net::NetworkDelegate *network_delegate, UDPProtocolHandler *handler );
	~UdpUrlRequestJob() override;

	// net::URLRequestJob implementation:
	void Start() override;
	bool GetMimeType(std::string* mime_type) const override;
	void Kill() override;
	int ReadRawData( net::IOBuffer *buf, int buf_size ) override;

private:
	bool _killed;
	net::IPEndPoint _address;
	std::string _sAddress;
	net::UDPServerSocket *_socket;
	UDPProtocolHandler *_handler;
	base::WeakPtrFactory<UdpUrlRequestJob> _weakPtrFactory;
};

}
