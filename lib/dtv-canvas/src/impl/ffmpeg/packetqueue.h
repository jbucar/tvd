#pragma once

#include "types.h"
#include <boost/thread.hpp>
#include <list>

namespace canvas {
namespace ffmpeg {

class PacketQueue {
public:
	PacketQueue();
	~PacketQueue();

	void start();
	void stop();
	
	int get( struct AVPacket *pkt );
	void push( struct AVPacket *pkt );
	bool isFull();
	void flush();

private:
	bool _exit;
	boost::mutex _mutex;
	boost::condition_variable _cWakeup;
	std::list<struct AVPacket> _pkts;
};

}
}

