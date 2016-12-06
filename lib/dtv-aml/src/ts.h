#pragma once

#include <vector>
#include <string>

namespace aml {

namespace audio {
struct dec_s;
typedef struct dec_s dec_t;
}

namespace ts {

struct ParamsS {
	int videoPID;
	int videoType;
	int audioPID;
	int audioType;
	int	pcrPID;
};
typedef struct ParamsS Params;


class Player {
public:
	Player();
	~Player();

	bool init();
	void fin();

	bool play( Params &params );
	void stop();

	bool getVideoInfo( int &w, int &h, int &fps );
	bool getAudioInfo( int &nChannels, int &rate );
	bool setVolume( float vol );
	bool setMute( bool state );

private:
	int _tsFD;
	audio::dec_t *_audec;
};

}	//	namespace ts
}	//	namespace aml

