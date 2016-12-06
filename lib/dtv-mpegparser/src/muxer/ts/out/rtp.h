#pragma once

#include "output.h"
#include <util/types.h>

namespace util {
namespace net {
	class Socket;
	class SockAddr;
}
}

namespace tuner {
namespace muxer {
namespace ts {

class RTPOutput : public Output {
public:
	RTPOutput( const std::string &ip, unsigned short port, bool udp=true, int pkts=7 );
	virtual ~RTPOutput();

	virtual const std::string url() const;
	virtual bool initialize();
	virtual void finalize();
	virtual bool write( util::Buffer *buf );

private:
	std::string _ip;
	unsigned short _port;
	bool _udpMode;
	int _pktsByOutput;
	util::net::Socket *_socket;
	util::net::SockAddr *_sAddr;
	double _timestamp;
	int _pkts;
	util::WORD _seq;
	int _bIndex;
	util::BYTE *_buf;
	std::string _url;
};

}
}
}

