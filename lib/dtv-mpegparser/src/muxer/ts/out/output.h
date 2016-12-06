#pragma once

#include <string>

namespace util {
	class Buffer;
}

namespace tuner {
namespace muxer {
namespace ts {

class Output {
public:
	Output();
	virtual ~Output();

	virtual bool initialize();
	virtual void finalize();

	virtual const std::string url() const=0;

	virtual bool write( util::Buffer *buf )=0;
};

}
}
}

