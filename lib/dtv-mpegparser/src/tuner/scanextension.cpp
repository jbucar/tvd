#include "scanextension.h"
#include "tuner.h"
#include "../service/service.h"
#include <util/assert.h>
#include <util/log.h>

namespace tuner {

ScanExtension::ScanExtension( const boost::function<void (Service *)> &onSrvReady )
{
	DTV_ASSERT(!onSrvReady.empty());
	_onSrvReady = onSrvReady;
}

ScanExtension::~ScanExtension()
{
}

//	Service notification
void ScanExtension::onServiceReady( Service *srv, bool ready ) {
	if (ready) {
		ID type = srv->type();
		bool result =
		type == tuner::service::type::dradio ||
		type == tuner::service::type::one_seg ||
		type == tuner::service::type::dtv ||
		type == tuner::service::type::dtv_hd ||
		type == tuner::service::type::dtv_ac_sd ||
		type == tuner::service::type::dtv_ac_hd;

		if (result) {
			//	Notify service ready
			_onSrvReady( srv );
		} else {
			LDEBUG( "ScanExtension", "Service filtered or not supported: type=%02x", srv->type() );
		}
	}
}

bool ScanExtension::mustNotify( bool /*inScan*/ ) const {
	return true;
}

}

