#include "player.h"
#include "mediaplayer.h"
#include <util/log.h>

namespace canvas {
namespace ffmpeg {

Player::Player()
{
}

Player::~Player()
{
}

//	Media player
canvas::MediaPlayer *Player::createMediaPlayer( System *sys ) const {
	return new MediaPlayer( sys );
}

}
}
