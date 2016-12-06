#include "viewer.h"

namespace tuner {
namespace player {
namespace es {
namespace cc {

Viewer::Viewer( bool isCC )
	: _isCC(isCC)
{
}

Viewer::~Viewer()
{
}

bool Viewer::isCC() const {
	return _isCC;
}

void Viewer::start() {
}

void Viewer::stop() {
}

} //	namespace cc
} //	namespace es
} //	namespace player
} //	namespace tuner

