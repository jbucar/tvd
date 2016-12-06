#include "divx5.h"
#include "../packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TODO: put width and height as parameters
#define VIDEO_WIDTH 352
#define VIDEO_HEIGHT 288

int Divx5Strategy::update_frame_header( Packet *pkt ) {	
	
	// Discard data packet with size below 8 bytes
	// This happens on divx5 data packets
	if(pkt->size <= 8) {
		return 1;
	}

	return 0;
}

int Divx5Strategy::write_header( Packet *info, Packet *pkt ) {
	if (pkt->data && (pkt->alloc_size >= info->size)) {
		memcpy(pkt->data, info->data, info->size);
		pkt->size = info->size;
	} else {
		printf("[Divx5Strategy::write_header] No enough memory!\n");
		return -1;
	}
	return 0;
}

void Divx5Strategy::vcodec_info_init( codec_para_t *v_codec ) {
	v_codec->has_video          = 1;
	v_codec->video_type         = VFORMAT_MPEG4;
	v_codec->video_pid          = 1;
	v_codec->am_sysinfo.format  = 4;
	v_codec->am_sysinfo.height  = VIDEO_HEIGHT;
	v_codec->am_sysinfo.width   = VIDEO_WIDTH;
	v_codec->am_sysinfo.rate    = 4002;
	v_codec->am_sysinfo.ratio   = 1;
	v_codec->am_sysinfo.ratio64 = 1;
	v_codec->noblock = 1;
	v_codec->am_sysinfo.param = (void *)0;
	v_codec->stream_type = STREAM_TYPE_ES_VIDEO;

    // printf("[rako] video: type=%x, pid=%x, format=%x, height=%d, width=%d, rate=%d, ratio=%d, ratio64=%lld, noblock=%d, am_sysinfo.param=%p, stream_type=%d\n",
	//     v_codec->video_type, v_codec->video_pid, v_codec->am_sysinfo.format, v_codec->am_sysinfo.height, v_codec->am_sysinfo.width, v_codec->am_sysinfo.rate,
	//     v_codec->am_sysinfo.ratio, v_codec->am_sysinfo.ratio64, v_codec->noblock,
	//     v_codec->am_sysinfo.param, v_codec->stream_type);
}

