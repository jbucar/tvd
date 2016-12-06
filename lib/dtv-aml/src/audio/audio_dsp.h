#pragma once

namespace aml {
namespace audio {

struct AudioInfo_S;
typedef struct AudioInfo_S AudioInfo;

namespace dsp {

struct audio_s;
typedef struct audio_s audio_t;

struct audio_s {
	int fd;
};
typedef struct audio_s audio_t;


bool init( audio_t *aud );
void fin( audio_t *aud );

bool start( audio_t *aud );
void stop( audio_t *aud );

bool canDecode( int format );
int bufferSize();
bool getInfo( audio_t *dsp, AudioInfo *info );
int read( audio_t *aud, char *buffer, int size );
unsigned long getCurrentPTS(audio_t *);
bool autoMute( audio_t *dsp, bool on );

bool tsync_pause( audio_t * );
bool tsync_resume( audio_t * );
bool tsync_pre_start( audio_t * );
bool tsync_start( audio_t *, unsigned long pts );
bool tsync_tstamp( audio_t *, unsigned long pts );
bool tsync_set_apts( audio_t *, unsigned long pts );
bool tsync_get_apts( audio_t *, unsigned long &pts );
bool tsync_get_pcr( audio_t *, unsigned long &pcr );

}
}
}

