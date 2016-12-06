#pragma once

#include "codec.h"

class Divx5Strategy : public CodecStrategy {

public:
	Divx5Strategy() {}
	virtual ~Divx5Strategy() {}

	virtual int update_frame_header( Packet *pkt );
	virtual int write_header( Packet *info, Packet *pkt );
	virtual void vcodec_info_init( codec_para_t *v_codec );

	virtual std::string name() { return "divx5"; }
};
