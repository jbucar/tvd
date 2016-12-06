#include "ts.h"
#include "aml.h"
#include "audio/audio.h"
#ifndef HTMLSHELL_BUILD
#	include "generated/config.h"
#endif
#if AML_AUDIO_USE_ALSA
#include "audio/audio_alsa.h"
#endif
#if AML_AUDIO_USE_PULSE
#include "audio/audio_pulse.h"
#endif
#include <boost/algorithm/string.hpp>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stropts.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

extern "C" {
#include "amports/amstream.h"
#include "amports/vformat.h"
#include "amports/aformat.h"
}

#define TS_DEVICE_FILE    "/dev/amstream_mpts"
#define TS_SOURCE         "ts2"
#define AV_SYNC_THRESHOLD  (100*90)

namespace aml {
namespace ts {

Player::Player()
{
	_tsFD = -1;
	_audec = NULL;
}

Player::~Player()
{
}

bool Player::init() {
	//	Setup output via ALSA
	audio::out_t aout;
#if AML_AUDIO_USE_ALSA
	audio::alsa::setup(&aout);
#endif

#if AML_AUDIO_USE_PULSE
	audio::pulse::setup(&aout);
#endif
	_audec = audio::init( &aout );
	return _audec ? true : false;
}

void Player::fin() {
	if (_audec) {
		audio::fin( _audec );
		_audec = NULL;
	}
}

bool Player::play( Params &params ) {
	assert(_audec);

	printf( "[aml] Play ts: video=(%d,%d), audio=(%d,%d), pcr=%d\n",
		params.videoPID, params.videoType, params.audioPID, params.audioType, params.pcrPID );

	//	Setup vfm
	sysfs::set("/sys/class/vfm/map", "rm all");
	sysfs::set("/sys/class/vfm/map", "add default_osd osd amvideo");
	sysfs::set("/sys/class/vfm/map", "add default decoder ppmgr deinterlace amvideo");

	//	Open ts device
	_tsFD = open(TS_DEVICE_FILE, O_RDWR);
	if (_tsFD < 0) {
		printf("Cannot open TS controller: errno=%d error=%s\n", errno, strerror(errno));
		return false;
	}

	//	Setup demux source
	if (!video::setDemux( TS_SOURCE )) {
		printf( "Cannot set demux source\n" );
		stop();
		return false;
	}

	//	Setup video
	if (params.videoPID != -1) {
		//	Video
		int video_type;
		switch (params.videoType) {
			case 0x1:
				video_type = VFORMAT_MPEG12;
				break;
			case 0x2:
				video_type = VFORMAT_MPEG12;
				break;
			case 0x1b:
				video_type = VFORMAT_H264;
				break;
			default: {
				printf( "[aml] Unknown video format: type=%02x\n", params.videoType );
				video_type = VFORMAT_MPEG12;
				break;
			}
		}

		if (ioctl(_tsFD, AMSTREAM_IOC_VFORMAT, video_type) < 0) {
			printf("Set video format failed\n");
			stop();
			return false;
		}

		if (ioctl(_tsFD, AMSTREAM_IOC_VID, params.videoPID) < 0) {
			printf("Set video PID failed\n");
			stop();
			return false;
		}

		// r = codec_h_control(pcodec->handle, AMSTREAM_IOC_SYSINFO, (unsigned long)&am_sysinfo);
		// r = codec_h_control(pcodec->handle, AMSTREAM_IOC_VB_SIZE, pcodec->vbuf_size);
	}

	//	Setup audio
	int audio_type = AFORMAT_MPEG;
	if (params.audioPID != -1) {
		switch (params.audioType) {
			case 0x03:
				audio_type = AFORMAT_MPEG;
				break;
			case 0x04:
				audio_type = AFORMAT_MPEG;
				break;
			case 0x0F:
				audio_type = AFORMAT_AAC;
				break;
			case 0x81:
				audio_type = AFORMAT_AC3;
				break;
			case 0x11:
				audio_type = AFORMAT_AAC_LATM;
				break;
			default: {
				printf( "[aml] Unknown audio format: type=%02x\n", params.audioType );
				audio_type = AFORMAT_MPEG;
				break;
			}
		}

		//printf( "[aml] Set audio format: src=%02x, dst=%02x\n", params.audioType, audio_type );

		if (ioctl(_tsFD, AMSTREAM_IOC_AFORMAT, audio_type) < 0) {
			printf("Set audio type failed\n");
			stop();
			return false;
		}

		if (ioctl(_tsFD, AMSTREAM_IOC_AID, params.audioPID) < 0) {
			printf("Set audio PID failed\n");
			stop();
			return false;
		}

		// if (ioctl(_tsFD, AMSTREAM_IOC_ACHANNEL, 2) < 0) {
		// 	printf("Set audio channel failed\n");
		// 	return -1;
		// }

		// r = codec_h_control(pcodec->handle, AMSTREAM_IOC_AUDIO_INFO, (unsigned long)audio_info);
		// r = codec_h_control(pcodec->handle, AMSTREAM_IOC_AB_SIZE, pcodec->abuf_size);
	}

	//	Enable sync AV if audio and video available
	if (params.videoPID != -1 && params.audioPID != -1) {
		video::setSyncMode( true );
	}
	else {
		video::setSyncMode( false );
	}

	//	Init stream.
	if (ioctl(_tsFD, AMSTREAM_IOC_PORT_INIT, 0) < 0) {
		printf("Port init failed\n");
		stop();
		return false;
	}

	if (ioctl(_tsFD, AMSTREAM_IOC_TS_SKIPBYTE, 0) < 0) {
		printf("Set ts skipbyte failed\n");
		stop();
		return false;
	}

	//	Start audio
	if (params.audioPID != -1) {
		if (!audio::start( _audec, audio_type, AV_SYNC_THRESHOLD )) {
			return false;
		}
	}

	return true;
}

void Player::stop() {
	printf( "[aml] Stop ts\n" );

	if (_audec) {
		audio::stop( _audec );
	}

	if (_tsFD >= 0) {
		close(_tsFD);
		_tsFD = -1;
	}

	//	Restore video chain
	sysfs::set("/sys/class/vfm/map", "rm all");
	sysfs::set("/sys/class/vfm/map", "add default_osd osd amvideo");
	sysfs::set("/sys/class/vfm/map", "add default decoder ppmgr amvideo");

	//	Reset video mode
	display::resetFreeScale();
}

bool Player::getVideoInfo( int &w, int &h, int &fps ) {
	//	Get status
	struct am_io_param am_io;
	memset(&am_io,0,sizeof(am_io));
    if (ioctl(_tsFD, AMSTREAM_IOC_VDECSTAT, (unsigned long)&am_io) < 0) {
		printf( "[aml] Decoder: cannot get vdec stata\n" );
		return false;
	}

    struct vdec_status *st = &am_io.vstatus;
	if (st->status & 0x3F) {
		printf( "[aml] Get video info: w=%d, h=%d, fps=%d, error=%d, status=%d\n",
			st->width, st->height, st->fps, st->error_count, st->status );

		w = st->width;
		h = st->height;
		fps = st->fps;
		return true;
	}
	return false;
}

bool Player::getAudioInfo( int &nChannels, int &rate ) {
	//	Get status
	struct am_io_param am_io;
	memset(&am_io,0,sizeof(am_io));
    if (ioctl(_tsFD, AMSTREAM_IOC_ADECSTAT, (unsigned long)&am_io) < 0) {
		printf( "[aml] Decoder: cannot get vdec stata\n" );
		return false;
	}

    struct adec_status *st = &am_io.astatus;
    printf( "[aml] Get Audio resolution: channels=%d, sample_rate=%d, resolution=%d, error=%d, status=%x\n",
	    st->channels, st->sample_rate, st->resolution, st->error_count, st->status );

    nChannels = st->channels;
    rate = st->sample_rate;
    return true;
}

bool Player::setVolume( float /*val*/ ) {	//	TODO
	// int ret = codec_set_volume(&impl::codec, val);
	// if (ret < 0) {
	// 	printf( "[aml] Cannot set volume\n" );
	// 	return false;
	// }
	return true;
}

bool Player::setMute( bool /*st*/ ) {	//	TODO
	// int ret=codec_set_mute( &impl::codec, st ? 1 : 0 );
	// if (ret < 0) {
	// 	printf( "[aml] Cannot set mute state\n" );
	// 	return false;
	// }
	return true;
}

}	//	namespace ts
}

