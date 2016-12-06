#pragma once

namespace aml {
namespace audio {
namespace hw {

typedef enum {
	STEREO_MODE = 0,
	LEFT_CHANNEL_MONO,
	RIGHT_CHANNEL_MONO,
	CHANNELS_SWAP,
} command_t;

bool ctrl( command_t cmd );

}
}
}

