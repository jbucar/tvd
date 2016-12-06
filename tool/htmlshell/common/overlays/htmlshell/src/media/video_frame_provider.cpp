#include "video_frame_provider.h"
#include "video_proxy.h"
#include "webmediaplayer_proxy.h"

#if defined(VIDEO_HOLE)
#	define CREATE_FRAME CreateHoleFrame
#else
#	define CREATE_FRAME CreateTransparentFrame
#endif

namespace tvd {

VideoFrameProvider::VideoFrameProvider( WebMediaPlayerProxy *wmpp )
	: _wmpp(wmpp)
{
	_client = nullptr;
	_hole = media::VideoFrame::CREATE_FRAME(gfx::Size(720,576));
}

VideoFrameProvider::~VideoFrameProvider()
{
	if (_client) {
		_client->StopUsingProvider();
		_client = nullptr;
	}
}

void VideoFrameProvider::SetVideoFrameProviderClient( cc::VideoFrameProvider::Client *client ) {
	if (_client) {
		_client->StopUsingProvider();
	}
	_client = client;
}

bool VideoFrameProvider::UpdateCurrentFrame( base::TimeTicks deadline_min, base::TimeTicks deadline_max ) {
	return HasCurrentFrame();
}

bool VideoFrameProvider::HasCurrentFrame() {
	gfx::Rect rect = _wmpp->videoBounds();
	if (_lastBounds != rect) {
		_lastBounds = rect;
		return true;
	}
	return false;
}

scoped_refptr<media::VideoFrame> VideoFrameProvider::GetCurrentFrame() {
	return _hole;
}

void VideoFrameProvider::PutCurrentFrame() {
	tvd::video::setAxis( _lastBounds.x(), _lastBounds.y(), _lastBounds.width(), _lastBounds.height() );
}

}
