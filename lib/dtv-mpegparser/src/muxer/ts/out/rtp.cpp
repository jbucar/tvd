#include "rtp.h"
#include "../../../demuxer/ts.h"
#include <util/net/socket.h>
#include <util/net/ipv4/sockaddr.h>
#include <util/buffer.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>

#define RTP_HEADER_SIZE 12
#define BITRATE 17267627

// REGISTER_INIT_CONFIG( tuner_service_player_ts_rtp ) {
// 	root().addNode("rtp")
// 		.addValue( "udp", "UDP mode", true )
// 		.addValue( "ip", "IP to use", "127.0.0.1" )
// 		.addValue( "port", "Port to use", 4000 )
// 		.addValue( "pkts", "Packet by RTP datagram", 7 );
// }

namespace tuner {
namespace muxer {
namespace ts {

RTPOutput::RTPOutput( const std::string &ip, unsigned short port, bool udp, int pkts/*=7*/ )
	: _ip(ip), _port(port), _udpMode(udp), _pktsByOutput(pkts)
{
	_socket = NULL;
	_sAddr = new util::net::SockAddr();

	_buf = NULL;
	_bIndex = 0;
	_timestamp = 0;
	_pkts = 0;
	_seq = 0;
}

RTPOutput::~RTPOutput()
{
	delete _sAddr;

	DTV_ASSERT(!_socket);
	DTV_ASSERT(!_buf);
}

bool RTPOutput::initialize() {
	LDEBUG( "RTPOutput", "Initialize" );

	//	Create socket
	_socket = new util::net::Socket();
	if (!_socket->create( util::net::type::dgram )) {
		LERROR( "RTPOutput", "Cannot create socket" );
		return false;
	}

	{	//	Setup socket address
		//	Assign ipv4 addr
		util::net::ipv4::SockAddr ipv4Addr( _ip, _port );
		*_sAddr = ipv4Addr.addr();

		//	Setup URL
		_url = _udpMode ? "udp://@" : "rtp://";
		_url += _ip;
		_url += ":";
		_url += boost::lexical_cast<std::string>( _port );
	}

	//	Initialize random seed
	srand( (unsigned int)time(NULL) );

	//	Alloc buffer
	_buf = (util::BYTE *)malloc( RTP_HEADER_SIZE + (_pktsByOutput * TS_PACKET_SIZE) );
	_bIndex = _udpMode ? 0 : RTP_HEADER_SIZE;
	_pkts = 0;

	if (!_udpMode) {	//	Setup RTP header
		//	RTP header Version = 2, Padding = 0, eXtension = 0, CSRC = 0
		_buf[0] = 0x80;
		//	RTP header with payload type of TS 102 134 and Mark = 0
		_buf[1] = 33;
		//	RTP header set the synchronization source, it's random generated
		int sscr = rand();
		util::DWORD netSSCR = util::net::uhtonl(sscr);
		memcpy( _buf+8, &netSSCR, sizeof(netSSCR) );
	}

	//	Packet counter initialized to zero
	_timestamp = rand() / ((double)RAND_MAX * 65535);
	_seq = 0;

	return true;
}

void RTPOutput::finalize() {
	LDEBUG( "RTPOutput", "Finalize" );

	_socket->close();
	DEL(_socket);
	free(_buf); _buf = NULL;
}

const std::string RTPOutput::url() const {
	return _url;
}

bool RTPOutput::write( util::Buffer *buf ) {
	size_t off = 0;
	util::BYTE *ptr = buf->bytes();
	size_t len = buf->length();

	while (off < len) {
		//	Copy
		memcpy( _buf+_bIndex, ptr+off, TS_PACKET_SIZE );
		_bIndex += TS_PACKET_SIZE;
		off += TS_PACKET_SIZE;

		_pkts++;
		if(_pkts >= _pktsByOutput) {
			if (!_udpMode) {
				//	Update #seq
				util::WORD net_seq_number = util::net::uhtons(_seq);
				memcpy( _buf+2, &net_seq_number, 2 );
				_seq++;

				//	Update timestamp (90KHz)
				if(_timestamp >= 4294967296.0) {
					_timestamp = 0.0;
				}
				_timestamp += (_pktsByOutput * TS_PACKET_SIZE * 8 * 90000) / double(BITRATE);
				util::DWORD net_time = util::net::uhtonl( util::DWORD(_timestamp) );
				memcpy( _buf+4, &net_time, 4 );
			}

			//	Send
			if (!_socket->sendto( _buf, _bIndex, 0, *_sAddr )) {
				LWARN( "RTPOutput", "Cannot send RTP packet" );
			}

			_pkts = 0;
			_bIndex = _udpMode ? 0 : RTP_HEADER_SIZE;
		}
	}

	return true;
}

}
}
}

