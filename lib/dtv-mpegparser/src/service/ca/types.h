#pragma once

#include "../types.h"

#define CW_KEY_SIZE 8

namespace util {
	class Buffer;
}

namespace tuner {

class PSIDemuxer;

namespace ca {

class FilterDelegate {
public:
	FilterDelegate() {}
	virtual ~FilterDelegate() {}

	virtual bool startFilter( PSIDemuxer *demux )=0;
	virtual bool startFilter( pes::FilterParams *params )=0;
	virtual void stopFilter( ID pid )=0;
};

typedef struct {
	util::BYTE even[CW_KEY_SIZE];
	util::BYTE odd[CW_KEY_SIZE];
} Keys;

}
}

