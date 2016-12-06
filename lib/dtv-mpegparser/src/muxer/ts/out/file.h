#pragma once

#include "output.h"

namespace tuner {
namespace muxer {
namespace ts {

class FileOutput : public Output {
public:
	explicit FileOutput( const std::string &fileName );
	virtual ~FileOutput();

	virtual bool initialize();
	virtual void finalize();
	virtual const std::string url() const;
	virtual bool write( util::Buffer *buf );

private:
	std::string _fileName;
	int _fd;
};

}
}
}

