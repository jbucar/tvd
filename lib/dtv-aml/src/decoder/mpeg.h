#ifndef _MPEG_H
#define _MPEG_H

#include "codec.h"

class MpegStrategy : public CodecStrategy {

public:
	MpegStrategy() {}
	virtual ~MpegStrategy() {}

	virtual void vcodec_info_init( codec_para_t *v_codec );

	virtual std::string name() { return "mpeg"; }
};

#endif