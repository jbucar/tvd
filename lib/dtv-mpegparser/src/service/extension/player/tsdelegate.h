 #pragma once

#include "delegate.h"
#include <boost/thread.hpp>

namespace util {
	class Buffer;
}

namespace tuner {

namespace stream {
	class BasicPipe;
}

namespace muxer {
namespace ts {
	class Muxer;
	class Output;
}
}

namespace ca {
	class Decrypter;
}

namespace player {
namespace ts {

class Delegate : public player::Delegate {
public:
	explicit Delegate( muxer::ts::Output *out );
	Delegate( int streamID, muxer::ts::Output *out );
	virtual ~Delegate();

	//	Getters
	virtual const std::string url() const;

	//	Start/stop
	virtual StreamInfo *canPlay( const ElementaryInfo &info, ID tag ) const;
	virtual void start();
	virtual void stop();

protected:
	virtual bool init();
	virtual void fin();
	virtual pes::mode::type mode() const;
	virtual stream::Pipe *pipe( pes::type::type esType ) const;
	void reader();

private:
	muxer::ts::Muxer *_muxer;
	stream::BasicPipe *_pipe;
	ca::Decrypter *_csa;
	boost::thread _thread;
	int _streamID;
};

}
}
}

