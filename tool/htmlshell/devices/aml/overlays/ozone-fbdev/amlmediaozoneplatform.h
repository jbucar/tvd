#pragma once

#include "media/ozone/media_ozone_platform.h"

namespace aml {

class MediaOzonePlatform : public media::MediaOzonePlatform {
public:
	MediaOzonePlatform();
	~MediaOzonePlatform() override;

	// media::MediaOzonePlatform implementation:
	media::VideoDecodeAccelerator *CreateVideoDecodeAccelerator( const base::Callback<bool(void)> &make_context_current ) override;

private:
	DISALLOW_COPY_AND_ASSIGN(MediaOzonePlatform);
};

}  // namespace aml
