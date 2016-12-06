#include "audio.h"
#include "audio_hw.h"
#include "audio_dsp.h"
#include "../aml.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <queue>

#define audio_print(format,...) fprintf(stderr,"[audio] %s: " format, __FUNCTION__, ##__VA_ARGS__)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))
#endif
#define SYSTIME_CORRECTION_THRESHOLD    (90000*15/100)
#define APTS_DISCONTINUE_THRESHOLD      (90000*3)

namespace aml {
namespace audio {

namespace msg {
struct adec_cmd;
typedef adec_cmd adec_cmd_t;
typedef std::queue<adec_cmd_t *> Queue;
}

//	Types
typedef enum {
	IDLE,
	TERMINATED,
	INITTED,
	ACTIVE,
	PAUSED,
} adec_state_t;

struct dec_s {
	adec_state_t state;
	int auto_mute;
	int muted;
	int avsync_threshold;
	int user_audio_delay;
	out_t aout;
	dsp::audio_t dsp;
	int decoderBufferSize;
	unsigned char *decoderBuffer;
	unsigned long dsp_audio_pts;
	unsigned long dsp_last_audio_pts;
	unsigned long dsp_last_pts_valid;
	int apts_interrupt;

	msg::Queue *queue;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

namespace msg {

//	Messages
typedef enum {
	CMD_START = 1 << 0,
	CMD_PAUSE = 1 << 1,
	CMD_RESUME = 1 << 2,
	CMD_STOP = 1 << 3,
	CMD_MUTE = 1 << 4,
	CMD_SET_VOL = 1 << 5,
	CMD_GET_VOL = 1 << 6,
	CMD_CHANL_SWAP = 1 << 7,
	CMD_LEFT_MONO = 1 << 8,
	CMD_RIGHT_MONO = 1 << 9,
	CMD_STEREO = 1 << 10,
	CMD_SET_LRVOL = 1<<20,
	CMD_RELEASE = 1 << 31,
} ctrl_cmd_t;

struct adec_cmd {
	int type; /* 1 = reply; 0 = no reply */
	int has_arg; /* 1 = value is valid; 0 = value is unvalid */
	ctrl_cmd_t ctrl_cmd;
	union {
		int en;
		float volume;
	} value;
	union {
		int en;
		float volume;
	} value_ext;
};
typedef adec_cmd adec_cmd_t;

static void sendMsg( dec_t *audec, ctrl_cmd_t c ) {
	adec_cmd_t *cmd = (adec_cmd_t *)calloc(1,sizeof(adec_cmd_t));
	if (cmd) {
		cmd->ctrl_cmd = c;
		pthread_mutex_lock(&audec->mutex);
		audec->queue->push( cmd );
		pthread_cond_signal(&audec->cond);
		pthread_mutex_unlock(&audec->mutex);
	} else {
		audio_print("sendMsg error: alloc failed, no memory!\n");
	}
}

static adec_cmd_t *getMsg( dec_t *audec ) {
	pthread_mutex_lock(&audec->mutex);
	adec_cmd_t *cmd = NULL;
	if (!audec->queue->empty()) {
		cmd = audec->queue->front();
		audec->queue->pop();
	}
	pthread_mutex_unlock(&audec->mutex);
	return cmd;
}

}	//	namespace msg


static unsigned long calc_pts(dec_t *audec) {
	out_t *aout;
	dsp::audio_t *asrc;

	aout = &audec->aout;
	asrc = &audec->dsp;

	unsigned long pts = dsp::getCurrentPTS(asrc);
	if (pts == (unsigned long)-1) {
		audio_print("get get_cur_pts failed\n");
		return -1;
	}
	audec->dsp_audio_pts = pts;

	{	//	Audio output latency
		unsigned long delay_pts = aout->latency(aout) * 90;
		if (delay_pts < pts) {
			pts -= delay_pts;
		} else {
			pts = 0;
		}
	}

	{	//	User audio delay (positive or negative) in pts units.
		int audio_delay_pts = audec->user_audio_delay * 90;
		if (abs(audio_delay_pts) < (long int)pts) {
			pts -= audio_delay_pts;
		}
	}

	return pts;
}

static int pts_start(dec_t *audec) {
	dsp::audio_t *asrc = &audec->dsp;

	audec->dsp_last_pts_valid = 0;
	dsp::tsync_pre_start(asrc);
	usleep(1000);

	unsigned long pts = calc_pts(audec);
	if ((int)pts == -1) {
		dsp::tsync_get_apts(asrc,pts);
	}

	dsp::tsync_start( asrc, pts );
	//audio_print("audio pts start from 0x%lx threshold=%d\n", pts, audec->avsync_threshold);

	return 0;
}

static int refresh_pts(dec_t *audec) {
	unsigned long pts;
	unsigned long systime;
	unsigned long last_pts = audec->dsp_last_audio_pts;
	unsigned long last_kernel_pts = audec->dsp_audio_pts;

	if (!dsp::tsync_get_pcr( &audec->dsp, systime )) {
		audio_print("Cannot get PCR\n" );
		return -1;
	}

	//	get audio time stamp
	pts = calc_pts(audec);
	if ((int)pts == -1 || last_pts == pts) {
		audio_print("No new PTS\n" );
		return -1;
	}

	if ((abs(pts - last_pts) > APTS_DISCONTINUE_THRESHOLD) && (audec->dsp_last_pts_valid)) {
		//	report audio time interruption
		audio_print("set automute: 0x%lx->0x%lx, 0x%x\n", last_pts, pts, abs(pts - last_pts));

		dsp::tsync_tstamp(&audec->dsp,pts);

		audec->dsp_last_audio_pts = pts;
		audec->dsp_last_pts_valid = 1;
		audec->auto_mute = 1;
		audec->apts_interrupt=10;
		return 0;
	}

	if (last_kernel_pts == audec->dsp_audio_pts) {
		audio_print("kernel and dsp PTS equal\n" );
		return 0;
	}

	audec->dsp_last_audio_pts = pts;
	audec->dsp_last_pts_valid = 1;

	//#define DEBUG_PTS
#ifdef DEBUG_PTS
	{
		unsigned long vpts;
		char buf[64];
		aml::sysfs::get( "/sys/class/tsync/pts_video", buf, sizeof(buf));// read vpts
		if (sscanf(buf, "0x%lx", &vpts) < 1) {
			audio_print("warning, unable to get vpts: buf=%s", buf );
			return -1;
		}

		audio_print("Audio diff: daudio=%08x, dvideo=%08x, apts=%08lx, pcr=%08lx, vpts=%08lx\n",
			abs(pts-systime), abs(vpts-systime), pts, systime, vpts );
	}
#endif

	if (abs(pts - systime) < audec->avsync_threshold) {
		audec->apts_interrupt=0;
		return 0;
	}
	else if(audec->apts_interrupt > 0){
		audec->apts_interrupt --;
		return 0;
	}

	//	report apts-system time difference
	dsp::tsync_set_apts(&audec->dsp,pts);

	return 0;
}

static int track_switch_pts(dec_t *audec) {
	unsigned long apts = calc_pts(audec);
	if ((int)apts == -1) {
		audio_print("error; unable to get apts\n");
		return 1;
	}

	unsigned long pcr;
	dsp::tsync_get_pcr( &audec->dsp, pcr );

	if((apts > pcr) && (apts - pcr > 0x100000))
		return 0;

	if (abs(apts - pcr) < audec->avsync_threshold || (apts <= pcr)) {
		return 0;
	} else {
		return 1;
	}

}

static void flag_check(dec_t *audec) {
	out_t *aout = &audec->aout;

	if (audec->auto_mute) {
		aout->pause(aout);
		audio_print("automute; pause audio out!\n");
		while (audec->state > TERMINATED && track_switch_pts(audec)) {
			usleep(1000);
		}
		aout->resume(aout);
		audio_print("automute; resume audio out\n");
		audec->auto_mute = 0;
	}
}

static void start_adec(dec_t *audec) {
	if (audec->state != INITTED) {
		return;
	}

	audio_print( "\n" );

	dsp::audio_t *asrc = &audec->dsp;
	out_t *aout = &audec->aout;

	//	Start DSP
	if (!dsp::start(asrc)) {
		return;
	}

	AudioInfo info;
	if (!dsp::getInfo(asrc,&info)) {
		return;
	}

	audio_print("channels=%d, samplerate=%d, bitspersample=%d\n",
		info.channels, info.samplerate, info.bits_per_sample );

	//	Init audio output
	if (aout->start(aout,&info) < 0) {
		return;
	}

	pts_start(audec);

	audec->state = ACTIVE;
}

static void stop_adec(dec_t *audec) {
	if (audec->state > INITTED) {
		audio_print("\n");

		out_t *aout = &audec->aout;
		audec->state = INITTED;
		aout->mute(aout, 1); //mute output, some repeat sound in audioflinger after stop
		aout->stop(aout);
		dsp::stop(&audec->dsp);
	}
}

static void pause_adec(dec_t *audec) {
	if (audec->state == ACTIVE) {
		out_t *aout = &audec->aout;
		audio_print("\n");
		audec->state = PAUSED;
		dsp::tsync_pause(&audec->dsp);
		aout->pause(aout);
	}
}

static void resume_adec(dec_t *audec) {
	if (audec->state == PAUSED) {
		out_t *aout = &audec->aout;
		audio_print("\n");
		audec->state = ACTIVE;
		aout->resume(aout);
		dsp::tsync_resume(&audec->dsp);
	}
}

static void mute_adec(dec_t *audec, int en) {
	out_t *aout = &audec->aout;
	if (aout->mute) {
		audio_print("enable=%d\n", en);
		aout->mute(aout, en);
		audec->muted = en;
	}
}

static void set_volume_adec(dec_t *audec, float vol) {
	out_t *aout = &audec->aout;
	if (aout->set_volume) {
		audio_print("vol=%f\n", vol);
		aout->set_volume(aout, vol);
	}
}

static void set_lrvolume_adec(dec_t *audec, float lvol,float rvol) {
	out_t *aout = &audec->aout;
	if (aout->set_lrvolume) {
		audio_print("left=%f, right=%f\n", lvol, rvol);
		aout->set_lrvolume(aout, lvol,rvol);
	}
}

static void processCmds( dec_t *audec ) {
	//	Get message
	msg::adec_cmd_t *msg = msg::getMsg(audec);
	if (!msg) {
		return;
	}

	audio_print("process cmd=%x, state=%d\n",
		msg->ctrl_cmd, audec->state );

	switch (msg->ctrl_cmd) {
		case msg::CMD_RELEASE:
			audec->state = TERMINATED;
			break;
		case msg::CMD_START:
			start_adec(audec);
			break;
		case msg::CMD_STOP:
			stop_adec(audec);
			break;
		case msg::CMD_PAUSE:
			pause_adec(audec);
			break;
		case msg::CMD_RESUME:
			resume_adec(audec);
			break;
		case msg::CMD_MUTE: {
			if (msg->has_arg) {
				mute_adec(audec, msg->value.en);
			}
			break;
		}
		case msg::CMD_SET_VOL: {
			if (msg->has_arg) {
				set_volume_adec(audec, msg->value.volume);
			}
			break;
		}
		case msg::CMD_SET_LRVOL: {
			if (msg->has_arg) {
				set_lrvolume_adec(audec, msg->value.volume,msg->value_ext.volume);
			}
			break;
		}
		case msg::CMD_CHANL_SWAP:
			hw::ctrl( hw::CHANNELS_SWAP );
			break;
		case msg::CMD_LEFT_MONO:
			hw::ctrl( hw::LEFT_CHANNEL_MONO );
			break;
		case msg::CMD_RIGHT_MONO:
			hw::ctrl( hw::RIGHT_CHANNEL_MONO );
			break;
		case msg::CMD_STEREO:
			hw::ctrl( hw::STEREO_MODE );
			break;
		default:
			audio_print("Unknow Command!\n");
			break;
	};

	free(msg);
}

static void play_audio( dec_t *audec ) {
	unsigned char *buffer = (unsigned char *)(((unsigned long)audec->decoderBuffer + 32) & (~0x1f));
	dsp::audio_t *asrc = &audec->dsp;
	out_t *aout = &audec->aout;
	int buflen=0;

	audio_print( "state=%d, buflen=%d\n", audec->state, buflen );
	while (audec->state > INITTED) {
		//	Process commands
		processCmds( audec );
		if (audec->state > INITTED) {
			//	Read audio buffer
			if (buflen < (audec->decoderBufferSize/2)) {
				int len = dsp::read( asrc, (char *)(buffer+buflen), (audec->decoderBufferSize-buflen) );
				if (len > 0) {
					//audio_print( "read from dsp: len=%d\n", len );
					buflen += len;
				}
			}

			if (audec->state != PAUSED) {
				flag_check(audec);
			}

			if (buflen >= 1024) {
				//	Check PTS
				refresh_pts(audec);

				//	Fill audio
				int len = aout->play( aout, buffer, buflen );
				if (len > 0) {
					buflen -= len;
					if (buflen) {
						audio_print( "swap buffer: len=%d, buflen=%d\n", len, buflen );
						memcpy(buffer, buffer+len, buflen);
					}
				}
				else if (len < 0) {
					//audio_print( "Audio write fail: ret=%d\n", len );
					buflen = 0;
				}
			}
		}
	}
}

static void *message_loop(void *args) {
	dec_t *audec = (dec_t *)args;

	audio_print( "begin thread\n" );

	audec->state = INITTED;
	while (audec->state != TERMINATED) {
		if (audec->state > INITTED) {
			play_audio(audec);
		}
		else {
			//	Wait for commands
			pthread_mutex_lock(&audec->mutex);
			while (audec->queue->empty()) {
				audio_print("wait for message\n");
				pthread_cond_wait(&audec->cond, &audec->mutex);
			}
			pthread_mutex_unlock(&audec->mutex);

			//	Process commands
			processCmds( audec );
		}
	}

	audio_print( "end thread\n" );
	pthread_exit(NULL);

	return NULL;
}

static void free_audio( dec_t *audec ) {
	delete audec->queue;
	free(audec->decoderBuffer);
	free(audec);
}

dec_t *init( out_t *aout ) {
	if (!aout->start) {
		audio_print( "error; audio output must be initialized\n" );
		return NULL;
	}

	dec_t *audec = (dec_t *)calloc(1,sizeof(dec_t));
	if (!audec) {
		audio_print( "error; calloc failed\n" );
		return audec;
	}


	//	Setup object
	memcpy(&audec->aout,aout,sizeof(out_t));
	audec->queue = new msg::Queue();
	audec->decoderBufferSize = dsp::bufferSize();
	audec->decoderBuffer = (unsigned char *)calloc(1,audec->decoderBufferSize);

	//	Initialize DSP
	if (!dsp::init(&audec->dsp)) {
		free_audio(audec);
		return NULL;
	}

	//	Init thread
	pthread_mutex_init(&audec->mutex, NULL);
	pthread_cond_init(&audec->cond, NULL);
	int ret = pthread_create(&audec->thread, NULL, (void* (*)(void *))message_loop, (void *)audec);
	if (ret != 0) {
		audio_print("error; create adec main thread failed!\n");
		dsp::fin(&audec->dsp);
		free_audio(audec);
		return NULL;
	}

	return audec;
}

void fin( dec_t *audec ) {
	assert(audec);
	audio_print( "\n" );

	//	Release
	msg::sendMsg( audec, msg::CMD_RELEASE );

	//	Join thread
	pthread_join(audec->thread, NULL);
	pthread_mutex_destroy(&audec->mutex);
	pthread_cond_destroy(&audec->cond);

	//	Finalize DSP
	dsp::fin(&audec->dsp);

	//	Free object
	free_audio(audec);
}

bool start( dec_t *audec, int format, int avsync_threshold ) {
	audio_print( "format=%d, av_threshold=%d\n", format, avsync_threshold );

	if (dsp::canDecode(format) < 0) {
		audio_print( "error: cannot decode audio format: format=%d\n", format );
		return false;
	}

	//	Fix avsync threshold
	if (avsync_threshold <= 0) {
		avsync_threshold = SYSTIME_CORRECTION_THRESHOLD;
	}
	audec->avsync_threshold = avsync_threshold;

	//	Start
	msg::sendMsg( audec, msg::CMD_START );

	return true;
}

void stop( dec_t *audec ) {
	assert(audec);
	audio_print( "\n" );

	//	Stop
	msg::sendMsg( audec, msg::CMD_STOP );
}

}
}

