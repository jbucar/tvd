#include "extension.h"
#include "player.h"
#include "../../service.h"
#include "../../servicemanager.h"
#include "../../../demuxer/ts.h"
#include <util/url.h>
#include <util/assert.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>

namespace tuner {
namespace player {

Extension::Extension( Player *p )
	: _player(p)
{
	DTV_ASSERT(p);
}

Extension::~Extension()
{
}

//	Services notifications
void Extension::onServiceStarted( Service *srv, bool started ) {
	if (srv->id() == _player->srvID()) {
		if (started) {
			_player->onStartService( srv );
		}
		else {
			_player->onStopService( srv );
		}
	}
}

//	Start/stop streams
bool Extension::startStream( pes::FilterParams *params ) {
	return srvMgr()->startFilter( params );
}

void Extension::stopStream( ID pid ) {
	srvMgr()->stopFilter( pid );
}

}
}

