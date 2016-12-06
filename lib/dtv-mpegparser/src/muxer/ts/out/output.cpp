#include "output.h"
#include "rtp.h"
#include "file.h"
#include <util/log.h>

namespace tuner {
namespace muxer {
namespace ts {

Output::Output()
{
}

Output::~Output()
{
}

bool Output::initialize() {
	return true;
}

void Output::finalize() {
}

}
}
}

