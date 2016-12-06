#pragma once

#include <string>

namespace util {
namespace task {
	class Dispatcher;
}
}

namespace tuner {
namespace player {
namespace es {
namespace cc {

class Viewer {
public:
	explicit Viewer( bool isCC );
	virtual ~Viewer();

	virtual void start();
	virtual void stop();

	bool isCC() const;
	virtual void show( const std::string &data )=0;
	virtual util::task::Dispatcher *dispatcher() const=0;

private:
	bool _isCC;
};

} //	namespace cc
} //	namespace es
} //	namespace player
} //	namespace tuner

