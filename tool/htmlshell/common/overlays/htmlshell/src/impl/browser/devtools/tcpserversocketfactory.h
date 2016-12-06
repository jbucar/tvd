#pragma once

#include "base/values.h"
#include "components/devtools_http_handler/devtools_http_handler.h"

namespace tvd {

class TCPServerSocketFactory : public devtools_http_handler::DevToolsHttpHandler::ServerSocketFactory {
public:
	TCPServerSocketFactory( const std::string &address, uint16_t port );

private:
	// DevToolsHttpHandler::ServerSocketFactory implementation
	scoped_ptr<net::ServerSocket> CreateForHttpServer() override;

	std::string _address;
	uint16_t _port;
	DISALLOW_COPY_AND_ASSIGN(TCPServerSocketFactory);
};

}
