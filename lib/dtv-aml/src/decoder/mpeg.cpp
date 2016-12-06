#include "mpeg.h"
#include "../packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void MpegStrategy::vcodec_info_init( codec_para_t *v_codec ) {
	v_codec->has_video          = 1;
	v_codec->video_type         = VFORMAT_MPEG12;
	v_codec->video_pid          = 1;
	v_codec->am_sysinfo.format  = 4;
	v_codec->am_sysinfo.height  = 1080;
	v_codec->am_sysinfo.width   = 1920;
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
