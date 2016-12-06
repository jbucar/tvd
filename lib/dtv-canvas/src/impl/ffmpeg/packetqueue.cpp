#include "packetqueue.h"
#include <util/log.h>
#include <boost/foreach.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
}

#define MIN_FRAMES 10

namespace canvas {
namespace ffmpeg {

PacketQueue::PacketQueue()
{
	_exit = false;
}

PacketQueue::~PacketQueue()
{
}

void PacketQueue::start() {
	LDEBUG( "ffmpeg", "Start queue" );
	_exit = false;
}

void PacketQueue::stop() {
	LDEBUG( "ffmpeg", "Stop queue" );
	_exit = true;
	_cWakeup.notify_all();
}

void PacketQueue::flush() {
	LDEBUG( "ffmpeg", "Flush queue" );
	_mutex.lock();
	BOOST_FOREACH( struct AVPacket &pkt, _pkts ) {
		av_free_packet(&pkt);
	}
	_pkts.clear();
	_mutex.unlock();
}

int PacketQueue::get( struct AVPacket *pkt ) {
	int ret;

	{	//	Lock
		boost::unique_lock<boost::mutex> lock( _mutex );
		while (!_exit && _pkts.empty()) {
			_cWakeup.wait( lock );
		}

		if (_exit) {
			ret=-1;
		}
		else if (!_pkts.empty()) {
			*pkt = _pkts.front();
			_pkts.pop_front();
			ret=1;
		} else {
			ret = 0;
		}
	}

	return ret;
}

void PacketQueue::push( struct AVPacket *pkt ) {
	//	Duplicate packet
	if (av_dup_packet(pkt) < 0) {
		LERROR( "ffmpeg", "Cannot duplicate packet" );
	}
	else {
		_mutex.lock();
		_pkts.push_back( *pkt );
		_mutex.unlock();
		_cWakeup.notify_all();
	}
}

bool PacketQueue::isFull() {
	_mutex.lock();
	int pktCount = (int)_pkts.size();
	_mutex.unlock();
	return pktCount > MIN_FRAMES;
}

}
}

