#include "amlmediaozoneplatform.h"
#include "amlvideodecodeaccelerator.h"

namespace aml {

MediaOzonePlatform::MediaOzonePlatform()
{}

MediaOzonePlatform::~MediaOzonePlatform()
{}

media::VideoDecodeAccelerator *MediaOzonePlatform::CreateVideoDecodeAccelerator( const base::Callback<bool(void)> &make_context_current ) {
	return new AmlVideoDecodeAccelerator();
}

}
