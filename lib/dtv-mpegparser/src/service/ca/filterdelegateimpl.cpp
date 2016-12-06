#include "filterdelegateimpl.h"
#include "../servicemanager.h"
#include <util/assert.h>

namespace tuner {
namespace ca {

FilterDelegateImpl::FilterDelegateImpl( ServiceManager *mgr )
	: _mgr(mgr)
{
	DTV_ASSERT(mgr);
}

FilterDelegateImpl::~FilterDelegateImpl()
{
}

bool FilterDelegateImpl::startFilter( PSIDemuxer *demux ) {
	return _mgr->startFilter( demux );
}

bool FilterDelegateImpl::startFilter( pes::FilterParams *params ) {
	return _mgr->startFilter( params );
}

void FilterDelegateImpl::stopFilter( ID pid ) {
	return _mgr->stopFilter( pid );
}

}
}

