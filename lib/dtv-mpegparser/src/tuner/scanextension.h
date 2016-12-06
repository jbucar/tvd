#pragma once

#include "types.h"
#include "../service/extension/extension.h"
#include <map>

namespace tuner {

class ScanExtension : public Extension {
public:
	ScanExtension( const boost::function<void (Service *)> &onSrvReady );
	virtual ~ScanExtension();

	//	Notifications
	virtual void onServiceReady( Service *srv, bool ready );
	virtual bool mustNotify( bool inScan ) const;

private:
	boost::function<void (Service *)> _onSrvReady;
};

}

