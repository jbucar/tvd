#pragma once

#include "../extension.h"

namespace tuner {
namespace player {

class Player;

class Extension : public tuner::Extension {
public:
	explicit Extension( Player *p );
	virtual ~Extension();

	//	Start/stop streams
	bool startStream( pes::FilterParams *params );
	void stopStream( ID pid );

	//	Services notifications
	virtual void onServiceStarted( Service *srv, bool started );

private:
	Player *_player;
};

}
}

