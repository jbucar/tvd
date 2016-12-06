#include "audio_dsp.h"
#include "audio.h"
#include "../aml.h"
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
extern "C" {
#include "../amports/aformat.h"
}

#define MCODEC_FMT_MPEG123      (1<<0)
#define MCODEC_FMT_AAC          (1<<1)
#define MCODEC_FMT_AC3          (1<<2)
#define MCODEC_FMT_DTS          (1<<3)
#define MCODEC_FMT_FLAC         (1<<4)
#define MCODEC_FMT_COOK         (1<<5)
#define MCODEC_FMT_AMR          (1<<6)
#define MCODEC_FMT_RAAC         (1<<7)
#define MCODEC_FMT_ADPCM        (1<<8)
#define MCODEC_FMT_WMA          (1<<9)
#define MCODEC_FMT_PCM          (1<<10)
#define MCODEC_FMT_WMAPRO       (1<<11)
#define MCODEC_FMT_ALAC         (1<<12)
#define MCODEC_FMT_VORBIS       (1<<13)
#define MCODEC_FMT_AAC_LATM     (1<<14)
#define MCODEC_FMT_APE          (1<<15)
#define MCODEC_FMT_EAC3         (1<<16)

#define AUDIODSP_FORMAT_PATH                    "/sys/class/astream/format"
#define AUDIODSP_DEV_NOD                        "/dev/audiodsp0"
#define TSYNC_EVENT                             "/sys/class/tsync/event"

#define AUDIODSP_SYNC_AUDIO_PAUSE                _IO('a', 0x01)
#define AUDIODSP_SYNC_AUDIO_RESUME               _IO('a', 0x02)

#define AUDIODSP_SET_FMT                         _IOW('a',1,sizeof(long))
#define AUDIODSP_START                           _IOW('a',2,sizeof(long))
#define AUDIODSP_STOP                            _IOW('a',3,sizeof(long))
#define AUDIODSP_DECODE_START                    _IOW('a',4,sizeof(long))
#define AUDIODSP_DECODE_STOP                     _IOW('a',5,sizeof(long))
#define AUDIODSP_REGISTER_FIRMWARE               _IOW('a',6,sizeof(long))
#define AUDIODSP_UNREGISTER_ALLFIRMWARE          _IOW('a',7,sizeof(long))
#define AUDIODSP_SYNC_AUDIO_START                _IOW('a',8,unsigned long)
#define AUDIODSP_SYNC_AUDIO_TSTAMP_DISCONTINUITY _IOW('a',9,unsigned long)
#define AUDIODSP_SYNC_SET_APTS                   _IOW('a',10,unsigned long)
#define AUDIODSP_WAIT_FORMAT                     _IOW('a',11,long)

#define AUDIODSP_GET_CHANNELS_NUM               _IOR('r',1,sizeof(long))
#define AUDIODSP_GET_SAMPLERATE                 _IOR('r',2,sizeof(long))
#define AUDIODSP_GET_BITS_PER_SAMPLE            _IOR('r',3,sizeof(long))
#define AUDIODSP_GET_PTS                        _IOR('r',4,sizeof(long))
#define AUDIODSP_GET_DECODED_NB_FRAMES          _IOR('r',5,sizeof(long))
#define AUDIODSP_GET_FIRST_PTS_FLAG             _IOR('r',6,sizeof(long))
#define AUDIODSP_SYNC_GET_APTS                  _IOR('r',7,unsigned long)
#define AUDIODSP_SYNC_GET_PCRSCR                _IOR('r',8,unsigned long)
#define AUDIODSP_AUTOMUTE_ON                    _IOW('r',9,sizeof(long))
#define AUDIODSP_AUTOMUTE_OFF                   _IOW('r',10,sizeof(long))
#define AUDIODSP_GET_PCM_LEVEL					_IOR('r',12,unsigned long)
#define AUDIODSP_SET_PCM_BUF_SIZE               _IOW('r',13,long)

#define dsp_print(format,...) fprintf(stderr,"[audio_dsp] %s: " format, __FUNCTION__,##__VA_ARGS__)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif
#define PARSER_WAIT_MAX 100
#define OUTPUT_BUFFER (32*1024)

namespace aml {
namespace audio {
namespace dsp {

typedef struct {
	int id;
	int fmt;
	char name[64];
} firmware_s_t;

typedef struct {
	int cmd;
	int fmt;
	int data_len;
	char *data;
} audiodsp_cmd_t;

struct aml_audio_format_s {
	const char *name;
	int mcodec_fmt;
};

static struct aml_audio_format_s aml_audio_format[] = {
	{ "NA",                -1                  },
	{ "amadec_mpeg",       MCODEC_FMT_MPEG123  },
	{ "amadec_pcm_s16le",  MCODEC_FMT_PCM      },
	{ "amadec_pcm_s16be",  MCODEC_FMT_PCM      },
	{ "amadec_pcm_u8",     MCODEC_FMT_PCM      },
	{ "amadec_pcm_bluray", MCODEC_FMT_PCM      },
	{ "amadec_adpcm",      MCODEC_FMT_ADPCM    },
	{ "amadec_aac_latm",   MCODEC_FMT_AAC_LATM },
	{ "amadec_aac",        MCODEC_FMT_AAC      },
	{ "amadec_ac3",        MCODEC_FMT_AC3      },
	{ "amadec_eac3",       MCODEC_FMT_EAC3     },
	{ "amadec_dts",        MCODEC_FMT_DTS      },
	{ "amadec_flac",       MCODEC_FMT_FLAC     },
	{ "amadec_cook",       MCODEC_FMT_COOK     },
	{ "amadec_amr",        MCODEC_FMT_AMR      },
	{ "amadec_raac",       MCODEC_FMT_RAAC     },
	{ "amadec_wmapro",     MCODEC_FMT_WMAPRO   },
	{ "amadec_wma",        MCODEC_FMT_WMA      },
	{ "amadec_alac",       MCODEC_FMT_ALAC     },
	{ "amadec_vorbis",     MCODEC_FMT_VORBIS   },
	{ "amadec_ape",        MCODEC_FMT_APE      },
	{ "amadec_alaw",       -1                  },
	{ "amadec_mulaw",      -1                  },
	{ NULL,                -1                  }
};

static firmware_s_t firmware_list[] = {
	{  0, MCODEC_FMT_MPEG123, "audiodsp_codec_mad.bin"},
	{  1, MCODEC_FMT_AAC, "audiodsp_codec_aac_helix.bin"},
	{  2, MCODEC_FMT_AC3 | MCODEC_FMT_EAC3, "audiodsp_codec_ddp_dcv.bin"},
	{  3, MCODEC_FMT_DTS, "audiodsp_codec_dca.bin"},
	{  4, MCODEC_FMT_FLAC, "audiodsp_codec_flac.bin"},
	{  5, MCODEC_FMT_COOK, "audiodsp_codec_cook.bin"},
	{  6, MCODEC_FMT_AMR, "audiodsp_codec_amr.bin"},
	{  7, MCODEC_FMT_RAAC, "audiodsp_codec_raac.bin"},
	{  8, MCODEC_FMT_ADPCM, "audiodsp_codec_adpcm.bin"},
	{  9, MCODEC_FMT_WMA, "audiodsp_codec_wma.bin"},
	{ 10, MCODEC_FMT_PCM, "audiodsp_codec_pcm.bin"},
	{ 11, MCODEC_FMT_WMAPRO, "audiodsp_codec_wmapro.bin"},
	{ 12, MCODEC_FMT_ALAC, "audiodsp_codec_alac.bin"},
	{ 13, MCODEC_FMT_VORBIS, "audiodsp_codec_vorbis.bin"},
	{ 14, MCODEC_FMT_AAC_LATM, "audiodsp_codec_aac.bin"},
	{ 15, MCODEC_FMT_APE, "audiodsp_codec_ape.bin"},

	/*	TODO: Rako: faltan
	*		audiodsp_codec_ac3.bin
	*		audiodsp_codec_dtshd.bin
	*		audiodsp_codec_mad_old.bin
	*		audiodsp_codec_mp3_lp.bin
	*		audiodsp_codec_null.bin
	*		audiodsp_codec_ogg.bin
	*/
};

typedef struct {
	int audio_id;
	char type[16];
} audio_type_t;

static audio_type_t audio_type[] = {
	{AFORMAT_AAC, "aac"},
	{AFORMAT_AC3, "ac3"},
	{AFORMAT_DTS, "dts"},
	{AFORMAT_FLAC, "flac"},
	{AFORMAT_COOK, "cook"},
	{AFORMAT_AMR, "arm"},
	{AFORMAT_RAAC, "raac"},
	{AFORMAT_ADPCM, "adpcm"},
	{AFORMAT_WMA, "wma"},
	{AFORMAT_WMAPRO, "wmapro"},
	{AFORMAT_ALAC, "alac"},
	{AFORMAT_VORBIS, "vorbis"},
	{AFORMAT_AAC_LATM, "aac_latm"},
	{AFORMAT_APE, "ape"},
	{AFORMAT_MPEG, "mp3"}
};

static int get_audio_format() {
	int fd;
	char format[21];
	int len;
	int fmt = -1;

	//	Open astream
	fd = open(AUDIODSP_FORMAT_PATH, O_RDONLY);
	if (fd < 0) {
		dsp_print("error; amadec device not found\n");
		return fmt;
	}

	//	Read format
	len = ::read(fd, format, 20);
	if (len <= 0) {
		dsp_print("error; cannot read from amadec device\n");
		close(fd);
		return fmt;
	}
	format[len-1] = 0;

	//	Get format
	int i=0;
	while (aml_audio_format[i].name &&
		strncmp(format,aml_audio_format[i].name,strlen(aml_audio_format[i].name)))
	{
		i++;
	}
	if (aml_audio_format[i].name) {
		fmt = aml_audio_format[i].mcodec_fmt;
	}

	close(fd);
	//dsp_print("format=%s, fmt=%d\n", format, fmt);
	return fmt;
}

static bool register_firmware(int fd, int fmt, char *name) {
	audiodsp_cmd_t cmd;
	cmd.cmd = AUDIODSP_REGISTER_FIRMWARE;
	cmd.fmt = fmt;
	cmd.data = name;
	cmd.data_len = strlen(name);

	int ret = ioctl(fd, AUDIODSP_REGISTER_FIRMWARE, &cmd);
	if (ret < 0) {
		dsp_print("error; cannot register firmware: name=%s, fmt=%d, ret=%d\n", name, fmt, ret);
		return false;
	}
	return true;
}

static firmware_s_t *find_firmware_by_fmt(int m_fmt) {
	int i;
	int num;
	firmware_s_t *f;

	num = ARRAY_SIZE(firmware_list);

	for (i = 0; i < num; i++) {
		f = &firmware_list[i];
		if (f->fmt & m_fmt) {
			return f;
		}
	}

	return NULL;
}

static bool audiodsp_start( audio_t *aud ) {
	//	Get format from audiodsp
	int format = get_audio_format();
	if (format < 0) {
		dsp_print("error; Unknown audio format\n");
		return false;
	}

	//	Find firmware file (.bin)
	if (find_firmware_by_fmt(format) == NULL) {
		dsp_print("error; Unknown firmware for format: format=%d\n", format );
		return false;
	}

	//	Set buffer
	if (ioctl(aud->fd, AUDIODSP_SET_PCM_BUF_SIZE, OUTPUT_BUFFER ) < 0) {
		dsp_print("error; Cannot set pcm buffer size: size=%d\n", OUTPUT_BUFFER );
		return false;
	}

	//	Set format
	if (ioctl(aud->fd, AUDIODSP_SET_FMT, format ) < 0) {
		dsp_print("error; Unknown firmware for format: format=%d\n", format );
		return false;
	}

	//	Start
	if (ioctl(aud->fd, AUDIODSP_START, 0) < 0) {
		dsp_print("error; Cannot start\n" );
		return false;
	}

	//	Start decode
	if (ioctl(aud->fd, AUDIODSP_DECODE_START, 0) < 0) {
		dsp_print("error; Cannot start decode\n" );
		stop(aud);
		return false;
	}

	return true;
}

bool init( audio_t *aud ) {
	//	Setup struct
	memset(aud,0,sizeof(audio_t));
	aud->fd = -1;

	int num = ARRAY_SIZE(firmware_list);
	dsp_print( "registrating ARC codecs: num=%d\n", num );

	//	Open
	int fd = open(AUDIODSP_DEV_NOD, O_RDONLY, 0644);
	if (fd < 0) {
		dsp_print("error; unable to open audio dsp: file=%s, err=%s\n", AUDIODSP_DEV_NOD, strerror(errno));
		return false;
	}

	//	Unregister all firmwares
	ioctl(fd, AUDIODSP_UNREGISTER_ALLFIRMWARE, 0);

	//	Register firmware_list firmwares
	int numRegistrated=0;
	for (int i=0; i < num; i++) {
		firmware_s_t *f = &firmware_list[i];
		if (register_firmware(fd, f->fmt, f->name)) {
			numRegistrated++;
		}
	}

	if (numRegistrated > 0) {
		aud->fd = fd;
		return true;
	}
	else {
		dsp_print("error; no firmware available\n");
		close(fd);
		return false;
	}
}

void fin( audio_t *aud ) {
	if (aud->fd >= 0) {
		dsp_print( "\n" );

		close(aud->fd);
		aud->fd = -1;
	}
}

bool start( audio_t *aud ) {
	//dsp_print( "\n" );
	assert(aud->fd >= 0);
	//	Start dsp
	return audiodsp_start(aud);
}

void stop( audio_t *aud ) {
	//dsp_print( "\n" );
	assert(aud);
	assert(aud->fd >= 0);

	if (ioctl(aud->fd, AUDIODSP_DECODE_STOP, 0) < 0) {
		dsp_print("error; Cannot stop decoder\n" );
	}

	if (ioctl(aud->fd, AUDIODSP_STOP, 0) < 0) {
		dsp_print( "error; ioctl failed\n" );
	}
}

bool canDecode( int format ) {
	bool result=false;

	int num = ARRAY_SIZE(audio_type);
	for (int i = 0; i < num; i++) {
		audio_type_t *t = &audio_type[i];
		if (t->audio_id == format) {
			result=true;
			break;
		}
	}

	//dsp_print( "format=%d, result=%d\n", format, result );
	return result;
}

int bufferSize() {
	return OUTPUT_BUFFER;
}

#define HANDLE_ERROR(msg) \
	err_count++; \
	if (err_count > PARSER_WAIT_MAX) { \
		dsp_print("error; %s\n", msg ); \
		return false; \
	} \
	else { \
		usleep(200000); \
		continue; \
	}

bool getInfo( audio_t *aud, audio::AudioInfo *info ) {
	assert(info);

	//	Wait format
	int err_count = 0;
	while (true) {
		int val = -1;

		//	Wait first PTS
		if (ioctl(aud->fd, AUDIODSP_GET_FIRST_PTS_FLAG, &val ) < 0) {
			dsp_print( "error; Cannot get first pts flag\n" );
			return false;
		}
		if (val != 1) {
			HANDLE_ERROR("get first pts flag");
		}

		//	Wait for format
		if (ioctl(aud->fd, AUDIODSP_WAIT_FORMAT, 0) < 0) {
			HANDLE_ERROR("waiting format");
		}

		//	Get Channels
		val = -1;
		if (ioctl(aud->fd, AUDIODSP_GET_CHANNELS_NUM, (unsigned long *)&val) < 0) {
			dsp_print("error; Cannot get #channels\n" );
			return false;
		}
		if (val == -1) {
			HANDLE_ERROR("get channels number");
		}
		info->channels = val;

		//	Get samplerate
		val = -1;
		if (ioctl(aud->fd, AUDIODSP_GET_SAMPLERATE, (unsigned long *)&val) < 0) {
			dsp_print("error; Cannot get samplerate\n" );
			return false;
		}
		if (val == -1) {
			HANDLE_ERROR("get samplerate");
		}
		info->samplerate = val;

		//	Get bits per sample
		val = -1;
		if (ioctl(aud->fd, AUDIODSP_GET_BITS_PER_SAMPLE, &val) < 0) {
			dsp_print("error; Cannot get bits per sample\n" );
			return false;
		}
		if ((int)val == -1) {
			HANDLE_ERROR("get bits per sample");
		}
		info->bits_per_sample = val;

		break;
	}

	return true;
}

int read( audio_t *aud, char *buffer, int size ) {
	assert(aud);
	assert(aud->fd >= 0);
	return ::read(aud->fd, buffer, size);
}

unsigned long getCurrentPTS(audio_t *aud) {
	assert(aud);
	assert(aud->fd >= 0);
	unsigned long val=-1;
	if (ioctl(aud->fd, AUDIODSP_GET_PTS, &val) < 0) {
		dsp_print("Cannot get bits per sample\n" );
		return false;
	}
	return val;
}

bool autoMute( audio_t *aud, bool on ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, on ? AUDIODSP_AUTOMUTE_ON : AUDIODSP_AUTOMUTE_OFF, 0) < 0) {
		dsp_print("Cannot set automute\n" );
		return false;
	}
	return true;
}

bool tsync_pause( audio_t *aud ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_AUDIO_PAUSE, 0) < 0) {
		dsp_print("Cannot set pause\n" );
		return false;
	}
	return true;
}

bool tsync_resume( audio_t *aud ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_AUDIO_RESUME, 0) < 0) {
		dsp_print("Cannot set resume\n" );
		return false;
	}
	return true;
}

bool tsync_pre_start( audio_t * ) {
	return aml::sysfs::set( TSYNC_EVENT, "AUDIO_PRE_START" ) == 0;
}

bool tsync_start( audio_t *aud, unsigned long pts ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_AUDIO_START, &pts) < 0) {
		dsp_print("Cannot start tsync\n" );
		return false;
	}
	return true;
}

bool tsync_tstamp( audio_t *aud, unsigned long pts ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_AUDIO_TSTAMP_DISCONTINUITY, &pts) < 0) {
		dsp_print("Cannot set tsync tstamp discontinuity\n" );
		return false;
	}
	return true;
}

bool tsync_set_apts( audio_t *aud, unsigned long pts ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_SET_APTS, &pts) < 0) {
		dsp_print("Cannot set tsync apts\n" );
		return false;
	}
	return true;
}

bool tsync_get_apts( audio_t *aud, unsigned long &pts ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_GET_APTS, &pts) < 0) {
		dsp_print("Cannot get tsync apts\n" );
		return false;
	}
	return true;
}

bool tsync_get_pcr( audio_t *aud, unsigned long &pcr ) {
	assert(aud);
	assert(aud->fd >= 0);
	if (ioctl(aud->fd, AUDIODSP_SYNC_GET_PCRSCR, &pcr) < 0) {
		dsp_print("Cannot get tsync pcr\n" );
		return false;
	}
	return true;
}

}
}
}

