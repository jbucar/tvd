#pragma once

#include "../../player.h"

namespace canvas {
namespace ffmpeg {

class Player : public canvas::Player {
public:
	Player();
	virtual ~Player();

protected:
	//	Factory
	virtual canvas::MediaPlayer *createMediaPlayer( System *sys ) const;
};

}
}
