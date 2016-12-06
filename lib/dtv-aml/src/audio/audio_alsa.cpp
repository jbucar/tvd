#include "audio_alsa.h"
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define alsa_print(format,...) fprintf(stderr,"[audio_alsa] " format,##__VA_ARGS__)
#define PCM_DEVICE_DEFAULT "default"

namespace aml {
namespace audio {
namespace alsa {

typedef struct {
	snd_pcm_t *handle;
	snd_pcm_format_t format;
	size_t bits_per_sample;
	size_t bits_per_frame;
	int buffer_size;
	unsigned int channelcount;
	unsigned int rate;
	int oversample;
	int realchanl;
	int flag;
	int stop_flag;
	int pause_flag;
	int mute_flag;
	int pass_flag;
	float volume_deamp;
} alsa_param_t;

static u_char output_buffer[64 * 1024];
static snd_pcm_uframes_t chunk_size = 1024;

static void pcm_deamplification(alsa_param_t *alsa_param, u_char *pcm_data, size_t pcm_frames);
static size_t pcm_write(alsa_param_t * alsa_param, u_char * data, size_t count);
static int alsa_play_impl(alsa_param_t * alsa_param, u_char * data, unsigned len);

static int set_params(alsa_param_t *alsa_params) {
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_uframes_t bufsize;
	int err;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	err = snd_pcm_hw_params_any(alsa_params->handle, hwparams);
	if (err < 0) {
		alsa_print("alsa_set_params: Broken configuration for this PCM: no configurations available\n");
		return err;
	}

	err = snd_pcm_hw_params_set_access(alsa_params->handle, hwparams,
		SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
		alsa_print("alsa_set_params: Access type not available\n");
		return err;
	}

	err = snd_pcm_hw_params_set_format(alsa_params->handle, hwparams, alsa_params->format);
	if (err < 0) {
		alsa_print("alsa_set_params: Sample format non available\n");
		return err;
	}

	err = snd_pcm_hw_params_set_channels(alsa_params->handle, hwparams, alsa_params->channelcount);
	if (err < 0) {
		alsa_print("alsa_set_params: Channels count non available\n");
		return err;
	}

	err = snd_pcm_hw_params_set_rate_near(alsa_params->handle, hwparams, &alsa_params->rate, 0);
	assert(err >= 0);

	alsa_params->bits_per_sample = snd_pcm_format_physical_width(alsa_params->format);
	alsa_params->bits_per_frame = alsa_params->bits_per_sample * alsa_params->channelcount;

	err = snd_pcm_hw_params_set_period_size_near(alsa_params->handle, hwparams, &chunk_size, NULL);
	if (err < 0) {
		alsa_print("alsa_set_params: Unable to set period size \n");
		return err;
	}

	err = snd_pcm_hw_params(alsa_params->handle, hwparams);
	if (err < 0) {
		alsa_print("alsa_set_params: Unable to install hw params:\n");
		return err;
	}

	err = snd_pcm_hw_params_get_buffer_size(hwparams, &bufsize);
	if (err < 0) {
		alsa_print("alsa_set_params: Unable to get buffersize \n");
		return err;
	}
	alsa_params->buffer_size = bufsize * alsa_params->bits_per_frame / 8;

	err = snd_pcm_sw_params_current(alsa_params->handle, swparams);
	if (err < 0) {
		alsa_print("alsa_set_params: Unable to get sw-parameters\n");
		return err;
	}

	err = snd_pcm_sw_params(alsa_params->handle, swparams);
	if (err < 0) {
		alsa_print("alsa_set_params: Unable to get sw-parameters\n");
		return err;
	}

	return 0;
}

static int check_passthrough() {
	int  val = 0;
	char bcmd[256] = {0};
	const char *path = "/sys/class/audiodsp/digital_raw";
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		read(fd, bcmd, sizeof(bcmd));
		close(fd);
		if (strstr(bcmd, "RAW") != 0)
			val = 1;
	}
	return val;
}

static unsigned oversample_play(alsa_param_t * alsa_param, u_char * src, unsigned count) {
	int frames = 0;
	int ret=0;
	int i;
	unsigned short * to, *from;
	to = (unsigned short *)output_buffer;
	from = (unsigned short *)src;

	if (alsa_param->realchanl == 2) {
		if (alsa_param->oversample == -1) {
			frames = count * 8 / alsa_param->bits_per_frame;
			frames = frames & (~(32 - 1));
			for (i = 0; i < (frames * 2); i += 4) { // i for sample
				*to++ = *from++;
				*to ++ = *from++;
				from += 2;
			}
			pcm_deamplification(alsa_param, output_buffer, frames / 2);
			ret = pcm_write(alsa_param, output_buffer, frames / 2);
			ret = ret * alsa_param->bits_per_frame / 8;
			ret = ret * 2;
		} else if (alsa_param->oversample == 1) {
			frames = count * 8 / alsa_param->bits_per_frame;
			frames = frames & (~(16 - 1));
			short l, r;
			for (i = 0; i < (frames * 2); i += 2) {
				l = *from++;
				r = *from++;
				*to++ = l;
				*to++ = r;
				*to++ = l;
				*to++ = r;
			}
			pcm_deamplification(alsa_param, output_buffer, frames * 2);
			ret = pcm_write(alsa_param, output_buffer, frames * 2);
			ret = ret * alsa_param->bits_per_frame / 8;
			ret = ret / 2;
		} else if (alsa_param->oversample == 2) {
			frames = count * 8 / alsa_param->bits_per_frame;
			frames = frames & (~(8 - 1));
			short l, r;
			for (i = 0; i < (frames * 2); i += 2) {
				l = *from++;
				r = *from++;
				*to++ = l;
				*to++ = r;
				*to++ = l;
				*to++ = r;
				*to++ = l;
				*to++ = r;
				*to++ = l;
				*to++ = r;
			}
			pcm_deamplification(alsa_param, output_buffer, frames * 4);
			ret = pcm_write(alsa_param, output_buffer, frames * 4);
			ret = ret * alsa_param->bits_per_frame / 8;
			ret = ret / 4;
		}
	} else if (alsa_param->realchanl == 1) {
		if (alsa_param->oversample == -1) {
			frames = count * 8 / alsa_param->bits_per_frame;
			frames = frames & (~(32 - 1));
			for (i = 0; i < (frames * 2); i += 2) {
				*to++ = *from;
				*to++ = *from++;
				from++;
			}
			pcm_deamplification(alsa_param, output_buffer, frames);
			ret = pcm_write(alsa_param, output_buffer, frames);
			ret = ret * alsa_param->bits_per_frame / 8;
		} else if (alsa_param->oversample == 0) {
			frames = count * 8 / (alsa_param->bits_per_frame >> 1);
			frames = frames & (~(16 - 1));
			for (i = 0; i < (frames); i++) {
				*to++ = *from;
				*to++ = *from++;
			}
			pcm_deamplification(alsa_param, output_buffer, frames);
			ret = pcm_write(alsa_param, output_buffer, frames);
			ret = ret * (alsa_param->bits_per_frame) / 8;
			ret = ret / 2;
		} else if (alsa_param->oversample == 1) {
			frames = count * 8 / (alsa_param->bits_per_frame >> 1);
			frames = frames & (~(8 - 1));
			for (i = 0; i < (frames); i++) {
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from++;
			}
			pcm_deamplification(alsa_param, output_buffer, frames * 2);
			ret = pcm_write(alsa_param, output_buffer, frames * 2);
			ret = ret * (alsa_param->bits_per_frame) / 8;
			ret = ret / 4;
		} else if (alsa_param->oversample == 2) {
			frames = count * 8 / (alsa_param->bits_per_frame >> 1);
			frames = frames & (~(8 - 1));
			for (i = 0; i < (frames); i++) {
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from;
				*to++ = *from++;
			}

			pcm_deamplification(alsa_param, output_buffer, frames * 4);
			ret = pcm_write(alsa_param, output_buffer, frames * 4);
			ret = ret * (alsa_param->bits_per_frame) / 8;
			ret = ret / 8;
		}
	}

	return ret;
}

static size_t pcm_write(alsa_param_t * alsa_param, u_char * data, size_t count) {
	snd_pcm_sframes_t r;
	size_t result = 0;

	while (count > 0) {
		r = snd_pcm_writei(alsa_param->handle, data, count);
		if (r == -EINTR) {
			r = 0;
		}
		if (r == -ESTRPIPE) {
			while ((r = snd_pcm_resume(alsa_param->handle)) == -EAGAIN) {
				sleep(1);
			}
		}

		if (r < 0) {
			alsa_print("pcm_write error: write failed: ret=%ld\n", r);
			if ((r = snd_pcm_prepare(alsa_param->handle)) < 0) {
				return 0;
			}
		}

		if (r > 0) {
			result += r;
			count -= r;
			data += r * alsa_param->bits_per_frame / 8;
		}
	}
	return result;
}

static void pcm_deamplification(alsa_param_t *alsa_param, u_char *pcm_data, size_t pcm_frames) {
	int i, nsamples;
	short *pcm_samples;

	if (alsa_param->pass_flag) {
		// audio passthough is active, do not touch pcm_data,
		return;
	}

	if (alsa_param->volume_deamp >= 1.0f) {
		// deamplification required.
		return;
	}

	if (alsa_param->format != SND_PCM_FORMAT_S16_LE) {
		// we only handle 16-bit little endian pcm samples.
		return;
	}

	pcm_samples = (short*)pcm_data;
	nsamples = pcm_frames * alsa_param->channelcount;

	if (alsa_param->mute_flag || alsa_param->volume_deamp <= 0.0f) {
		// fast path for mute or volume <= 0.0.
		for (i = 0; i < nsamples; i++) {
			pcm_samples[i] = 0;
		}
	} else {
		int value;
		for (i = 0; i < nsamples; i++) {
			// must be int. so that we can check over/under flow.
			value = (int)((float)pcm_samples[i] * alsa_param->volume_deamp);
			pcm_samples[i] = (short)value;
		}
	}
}

static int alsa_play_impl(alsa_param_t * alsa_param, u_char * data, unsigned len) {
	size_t l = 0, r;

	if (!alsa_param->flag) {
		l = len * 8 / alsa_param->bits_per_frame;
		l = l & (~(32 - 1)); /*driver only support  32 frames each time */
		pcm_deamplification(alsa_param, data, l);
		r = pcm_write(alsa_param, data, l);
		r = r * alsa_param->bits_per_frame / 8;
	} else {
		r = oversample_play(alsa_param, data, len);
	}

	return r ;
}

static int start( out_t* aout, AudioInfo *info ) {
	//alsa_print("start\n");

	alsa_param_t *alsa_param = (alsa_param_t *)malloc(sizeof(alsa_param_t));
	if (!alsa_param) {
		alsa_print("alsa_init: alloc alsa_param failed, not enough memory!\n");
		return -1;
	}
	memset(alsa_param, 0, sizeof(alsa_param_t));

	if (info->samplerate >= (88200 + 96000) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = -1;
		alsa_param->rate = 48000;
	} else if (info->samplerate >= (64000 + 88200) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = -1;
		alsa_param->rate = 44100;
	} else if (info->samplerate >= (48000 + 64000) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = -1;
		alsa_param->rate = 32000;
	} else if (info->samplerate >= (44100 + 48000) / 2) {
		alsa_param->oversample = 0;
		alsa_param->rate = 48000;
		if (info->channels == 1) {
			alsa_param->flag = 1;
		} else if (info->channels == 2) {
			alsa_param->flag = 0;
		}
	} else if (info->samplerate >= (32000 + 44100) / 2) {
		alsa_param->oversample = 0;
		alsa_param->rate = 44100;
		if (info->channels == 1) {
			alsa_param->flag = 1;
		} else if (info->channels == 2) {
			alsa_param->flag = 0;
		}
	} else if (info->samplerate >= (24000 + 32000) / 2) {
		alsa_param->oversample = 0;
		alsa_param->rate = 32000;
		if (info->channels == 1) {
			alsa_param->flag = 1;
		} else if (info->channels == 2) {
			alsa_param->flag = 0;
		}
	} else if (info->samplerate >= (22050 + 24000) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = 1;
		alsa_param->rate = 48000;
	} else if (info->samplerate >= (16000 + 22050) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = 1;
		alsa_param->rate = 44100;
	} else if (info->samplerate >= (12000 + 16000) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = 1;
		alsa_param->rate = 32000;
	} else if (info->samplerate >= (11025 + 12000) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = 2;
		alsa_param->rate = 48000;
	} else if (info->samplerate >= (8000 + 11025) / 2) {
		alsa_param->flag = 1;
		alsa_param->oversample = 2;
		alsa_param->rate = 44100;
	} else {
		alsa_param->flag = 1;
		alsa_param->oversample = 2;
		alsa_param->rate = 32000;
	}

	alsa_param->channelcount = 2;
	alsa_param->realchanl = info->channels;
	alsa_param->format = SND_PCM_FORMAT_S16_LE;
	alsa_param->mute_flag = 0;
	alsa_param->pass_flag = check_passthrough();
	alsa_param->volume_deamp = 1.0f;

	{	//	Open pcm
		int err = snd_pcm_open(&alsa_param->handle, PCM_DEVICE_DEFAULT, SND_PCM_STREAM_PLAYBACK, 0);
		if (err < 0) {
			alsa_print("alsa_init: audio open error: %s\n", snd_strerror(err));
			return -1;
		}
	}

	//	Setup parameters
	set_params(alsa_param);

	//	Initialize private data
	aout->private_data = (void *)alsa_param;

	return 0;
}

static int pause(out_t* aout) {
	alsa_param_t *alsa_params = (alsa_param_t *)aout->private_data;

	//alsa_print("alsa_pause\n");

	int res;
	alsa_params->pause_flag = 1;
	while ((res = snd_pcm_pause(alsa_params->handle, 1)) == -EAGAIN) {
		sleep(1);
	}

	return res;
}

static int resume(out_t* aout) {
	alsa_param_t *alsa_params = (alsa_param_t *)aout->private_data;

	//alsa_print("alsa_resume\n");

	int res;
	alsa_params->pause_flag = 0;
	while ((res = snd_pcm_pause(alsa_params->handle, 0)) == -EAGAIN) {
		sleep(1);
	}

	return res;
}

static int stop(out_t* aout) {
	//alsa_print("alsa_stop\n");

	alsa_param_t *alsa_params = (alsa_param_t *)aout->private_data;

	snd_pcm_drop(alsa_params->handle);
	snd_pcm_close(alsa_params->handle);

	free(alsa_params);
	aout->private_data = NULL;

	return 0;
}

static int play( out_t *aout, unsigned char *buf, int len ) {
	alsa_param_t *alsa_params = (alsa_param_t *)aout->private_data;
	return alsa_play_impl( alsa_params, buf, len );
}

static unsigned long latency(out_t* aout) {
	alsa_param_t *alsa_params = (alsa_param_t *)aout->private_data;
	assert(alsa_params);

	snd_pcm_sframes_t frames = 0;
	snd_pcm_delay(alsa_params->handle, &frames);
	if (frames < 0) {
		snd_pcm_forward(alsa_params->handle, -frames);
		frames = 0;
	}

	return (frames * 1000) / alsa_params->rate;
}

static int mute(out_t* aout, unsigned int en) {
	alsa_param_t *alsa_param = (alsa_param_t *)aout->private_data;
	alsa_param->mute_flag = en;
	return 0;
}

static int set_volume(out_t* aout, float vol) {
	alsa_param_t *alsa_param = (alsa_param_t *)aout->private_data;

	if (!alsa_param)
		return 0;

	if (vol > 1.0f)
		alsa_param->volume_deamp = 1.0f;
	else if (vol < 0.0f)
		alsa_param->volume_deamp = 0.0f;
	else
		alsa_param->volume_deamp = vol;

	return 0;
}

static int set_lrvolume(out_t* /*aout*/, float /*lvol*/, float /*rvol*/) {
	return 0;
}

void setup( out_t *aout ) {
	memset(aout,0,sizeof(aout));
	aout->start = start;
	aout->stop = stop;
	aout->play = play;
	aout->pause = pause;
	aout->resume = resume;
	aout->latency = latency;
	aout->mute = mute;
	aout->set_volume = set_volume;
	aout->set_lrvolume = set_lrvolume;
}

}
}
}

