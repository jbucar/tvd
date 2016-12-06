#pragma once

#include "../../types.h"
#include "../../../demuxer/types.h"

namespace tuner {
namespace player {

class StreamInfo;

//	Types
struct StreamS {
	ElementaryInfo info;
	util::BYTE tag;
	StreamInfo *sInfo;
};
typedef struct StreamS Stream;

typedef bool (*fncFindStream)( const Stream * );

}	//	namespace player
}	//	namespace tuner

