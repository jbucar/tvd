#pragma once

#include "types.h"

namespace tuner {

class ServiceManager;

namespace ca {

class FilterDelegateImpl : public FilterDelegate {
public:
	explicit FilterDelegateImpl( ServiceManager *mgr );
	virtual ~FilterDelegateImpl();

	virtual bool startFilter( PSIDemuxer *demux );
	virtual bool startFilter( pes::FilterParams *params );
	virtual void stopFilter( ID pid );

private:
	ServiceManager *_mgr;
};

}
}

