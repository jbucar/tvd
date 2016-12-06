#include "audio_pulse.h"
#include "pulse_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pulse/volume.h>
#include <pulse/error.h>

#define pulse_print(format,...) fprintf(stderr,"[audio_pulse] %s: " format, __FUNCTION__,##__VA_ARGS__)

namespace aml {
namespace audio {
namespace pulse {

typedef struct {
	pa_wrapper *handle;
	pa_sample_spec spec;
	int bits_per_frame;
	pa_cvolume volume;
	int skip;
} pulse_type_t;

static int start( out_t* aout, AudioInfo *info ) {
	pulse_print(": channels=%d, samplerate=%d\n", info->channels, info->samplerate );

	pulse_type_t *type = (pulse_type_t *)malloc(sizeof(pulse_type_t));
	if (!type) {
		pulse_print("error; not enough memory!\n");
		return -1;
	}
	memset(type, 0, sizeof(pulse_type_t));

	//	The Sample format to use
	type->spec.format = PA_SAMPLE_S16LE;
	type->spec.rate = info->samplerate;
	type->spec.channels = info->channels;

	type->bits_per_frame = info->bits_per_sample;
	type->skip = (64*1024);

	//	Create a new playback stream
	int error;
	type->handle = pa_wrapper_new(NULL, "aml", PA_STREAM_PLAYBACK, NULL, "playback", &type->spec, NULL, NULL, &error);
	if (!type->handle) {
		pulse_print( "error; pa_wrapper_new() failed: %s\n", pa_strerror(error));
		free(type);
		return -1;
	}

	pa_cvolume_init(&type->volume);

	//	Initialize private data
	aout->private_data = (void *)type;

	pa_wrapper_flush( type->handle, &error );

	return 0;
}

static int stop(out_t* aout) {
	pulse_print("\n");
	assert(aout);
	pulse_type_t *priv = (pulse_type_t *)aout->private_data;
	assert(priv);
	assert(priv->handle);
	int error;
	pa_wrapper_flush( priv->handle, &error );
	pa_wrapper_free( priv->handle );
	priv->handle = NULL;
	return 0;
}

static int play( out_t *aout, unsigned char *src, int len ) {
	//pulse_print("len=%d\n", len );
	assert(aout);
	pulse_type_t *priv = (pulse_type_t *)aout->private_data;
	assert(priv);
	assert(priv->handle);

	priv->skip -= len;
	if (priv->skip >= 0) {
		return len;
	}

	//	play it
	int error;
	if (pa_wrapper_write( priv->handle, src, (size_t)len, &error) < 0) {
		pulse_print( "error; pa_wrapper_write() failed: %s\n", pa_strerror(error));
		return -1;
	}

	return len;
}

static int pauseImpl( out_t *aout, int pause ) {
	assert(aout);
	pulse_type_t *priv = (pulse_type_t *)aout->private_data;
	assert(priv);
	assert(priv->handle);

	const char *st = pause ? "pause" : "resume";
	pulse_print( "%s\n", st );

	int error;
	if (pa_wrapper_pause( priv->handle, pause, &error) < 0) {
		pulse_print( "error; pa_wrapper_pause() failed: %s\n", pa_strerror(error));
		return -1;
	}

	return 0;
}

static int pause(out_t* aout) {
	return pauseImpl( aout, 1 );
}

static int resume(out_t* aout) {
	return pauseImpl( aout, 0 );
}

static unsigned long latency(out_t* aout) {
	assert(aout);
	pulse_type_t *priv = (pulse_type_t *)aout->private_data;
	assert(priv);
	assert(priv->handle);

	int error;
	pa_usec_t latency = pa_wrapper_get_latency(priv->handle, &error);
	if (latency == (pa_usec_t)-1) {
		pulse_print( "error; pa_wrapper_get_latency() failed: %s\n", pa_strerror(error));
		return 0;

	}
	//pulse_print( "latency=%0lld\n", latency/1000 );
	return (unsigned long)latency/1000;
}

static int mute(out_t* aout, unsigned int en) {
	assert(aout);
	pulse_type_t *priv = (pulse_type_t *)aout->private_data;
	assert(priv);
	assert(priv->handle);

	pulse_print("en=%d\n", en);

	if (en) {
		pa_cvolume_mute(&priv->volume, priv->spec.channels);
	}
	else {
		pa_cvolume_reset(&priv->volume, priv->spec.channels);
	}

	return 0;
}

static int set_volume(out_t* aout, float vol) {
	pulse_print("\n");
	return 0;
}

static int set_lrvolume(out_t* /*aout*/, float /*lvol*/, float /*rvol*/) {
	pulse_print("\n");
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

