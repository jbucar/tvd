#pragma once

#include "cc/layers/video_frame_provider.h"
#include "ui/gfx/geometry/rect.h"

namespace media {
class VideoFrame;
}

namespace tvd {

class WebMediaPlayerProxy;

class VideoFrameProvider: public cc::VideoFrameProvider {
public:
	explicit VideoFrameProvider( WebMediaPlayerProxy *wmpp );
	~VideoFrameProvider() override;

	void SetVideoFrameProviderClient( cc::VideoFrameProvider::Client *client ) override;
	bool UpdateCurrentFrame( base::TimeTicks deadline_min, base::TimeTicks deadline_max ) override;
	bool HasCurrentFrame() override;
	scoped_refptr<media::VideoFrame> GetCurrentFrame() override;
	void PutCurrentFrame() override;

private:
	gfx::Rect _lastBounds;
	WebMediaPlayerProxy *_wmpp;
	cc::VideoFrameProvider::Client *_client;
	scoped_refptr<media::VideoFrame> _hole;
};

}
