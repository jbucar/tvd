#pragma once

#include "codec.h"

class Divx3Strategy : public CodecStrategy {

public:
	Divx3Strategy() {}
	virtual ~Divx3Strategy() {}

	virtual int update_frame_header( Packet *pkt );
	virtual int write_header( Packet *info, Packet *pkt );
	virtual void vcodec_info_init( codec_para_t *v_codec );

	virtual std::string name() { return "divx3"; }
	virtual int getExtraSize() { return 13; }
};
