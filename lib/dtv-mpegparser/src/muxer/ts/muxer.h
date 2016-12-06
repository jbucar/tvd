#pragma once

#include "../../demuxer/types.h"
#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace util {
	class Buffer;
}

namespace tuner {

class Pmt;

namespace muxer {
namespace ts {

namespace pt = boost::posix_time;

class Output;

class Muxer {
public:
	Muxer( Output *out );
	virtual ~Muxer();

	bool initialize();
	void finalize();

	void start( Pmt *pmt, ID tsID=0xABCD );
	void stop();
	bool process( util::Buffer *buf );

	Output *output() const;

	void enablePSI( bool st );
	void enableStats( bool st );

protected:
	//	Output
	bool write( util::Buffer *buf );
	bool writePSI( util::Buffer *buf );

	//	Muxer methods
	void makePAT( ID tsID, Pmt *pmt );
	void makePMT( Pmt *pmt );
	ID selectPID( Pmt *pmt );

	void addStream( util::BYTE *ptr, int &off, const ElementaryInfo &info );
	void beginPacket( BYTE *ptr, int &off, ID pid );
	void endPacket( BYTE *ptr, int &off );
	void updateCC( util::BYTE *ptr );

	//	Stats
	typedef std::map<ID,unsigned long long int> Stats;
	void printStats();
	void doStats( util::Buffer *buf );

private:
	Output *_out;

	ID _srvPID;
	util::Buffer *_pat;
	util::Buffer *_pmt;

	bool _withPSI;
	pt::ptime _lastPSI;

	bool _enableStats;
	Stats _stats;
};

}
}
}

