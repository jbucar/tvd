#pragma once

namespace aml {
namespace audio {

typedef struct AudioInfo_S {
	int channels;
	int samplerate;
	int bits_per_sample;
} AudioInfo;

struct out_s;
typedef out_s out_t;

struct out_s {
	int (*start)(out_t *, AudioInfo *info);
	int (*stop)(out_t *);
	int (*pause)(out_t *);
	int (*resume)(out_t *);
	int (*play)( out_t *, unsigned char *buf, int len );
	unsigned long(*latency)(out_t *); /* get latency in ms */
	int (*mute)(out_t *, unsigned int ); /* 1: enable mute ; 0: disable mute */
	int (*set_volume)(out_t *, float);
	int (*set_lrvolume)(out_t *, float,float);
	void *private_data;
};
typedef out_s out_t;

struct dec_s;
typedef struct dec_s dec_t;

dec_t *init( out_t *aout );
void fin( dec_t *audec );

bool start( dec_t *audec, int format, int avsync_threshold );
void stop(dec_t *audec);

}
}

