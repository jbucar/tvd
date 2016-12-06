#include "divx3.h"
#include "../packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TODO: put width and height as parameters
#define VIDEO_WIDTH 576
#define VIDEO_HEIGHT 320

int Divx3Strategy::update_frame_header( Packet *pkt ) {
#define DIVX311_CHUNK_HEAD_SIZE 13

    const unsigned char divx311_chunk_prefix[DIVX311_CHUNK_HEAD_SIZE] = {
        0x00, 0x00, 0x00, 0x01, 0xb6, 'D', 'I', 'V', 'X', '3', '.', '1', '1'
    };

    unsigned char *new_data;
    int data_size = pkt->size;
    int new_len = data_size + DIVX311_CHUNK_HEAD_SIZE + 4;
    new_data = (unsigned char *)malloc(new_len);

    memcpy(new_data, divx311_chunk_prefix, DIVX311_CHUNK_HEAD_SIZE);
    memcpy(new_data + DIVX311_CHUNK_HEAD_SIZE + 4, pkt->data, pkt->size);

    new_data[DIVX311_CHUNK_HEAD_SIZE + 0] = (data_size >> 24) & 0xff;
    new_data[DIVX311_CHUNK_HEAD_SIZE + 1] = (data_size >> 16) & 0xff;
    new_data[DIVX311_CHUNK_HEAD_SIZE + 2] = (data_size >>  8) & 0xff;
    new_data[DIVX311_CHUNK_HEAD_SIZE + 3] = data_size & 0xff;

    pkt->assign( new_data, new_len, new_len );

    return 0;
}

int Divx3Strategy::write_header( Packet *info, Packet *pkt ) {
    int w = VIDEO_WIDTH;
    int h = VIDEO_HEIGHT;

    unsigned i = (w << 12) | (h & 0xfff);
    unsigned char divx311_add[10] = {
        0x00, 0x00, 0x00, 0x01,
        0x20, 0x00, 0x00, 0x00,
        0x00, 0x00
    };
    divx311_add[5] = (i >> 16) & 0xff;
    divx311_add[6] = (i >> 8) & 0xff;
    divx311_add[7] = i & 0xff;

    if (pkt->data) {
        memcpy(pkt->data, divx311_add, sizeof(divx311_add));
        pkt->size = sizeof(divx311_add);
    } else {
        printf("[Divx3Strategy::write_header] No enough memory!\n");
        return -1;
    }
    return 0;
}

void Divx3Strategy::vcodec_info_init( codec_para_t *v_codec ) {
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

