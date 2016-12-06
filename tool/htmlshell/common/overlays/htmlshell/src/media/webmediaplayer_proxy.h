#pragma once

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/base/pipeline.h"
#include "media/base/renderer_factory.h"
#include "media/base/text_track.h"
#include "media/blink/buffered_data_source.h"
#include "media/blink/buffered_data_source_host_impl.h"
#include "media/blink/encrypted_media_player_support.h"
#include "media/blink/video_frame_compositor.h"
#include "media/blink/webmediaplayer_params.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/renderers/skcanvas_video_renderer.h"
#include "third_party/WebKit/public/platform/WebMediaPlayer.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerClient.h"

namespace blink {
class WebLocalFrame;
}

namespace base {
class SingleThreadTaskRunner;
}

namespace cc_blink {
class WebLayerImpl;
}

namespace media {
class WebMediaPlayerDelegate;
}

namespace tvd {

class VideoFrameProvider;

class MEDIA_EXPORT WebMediaPlayerProxy : public NON_EXPORTED_BASE(blink::WebMediaPlayer),
                                         public base::SupportsWeakPtr<WebMediaPlayerProxy> {
public:
	WebMediaPlayerProxy(blink::WebMediaPlayerClient* client,
                        base::WeakPtr<media::WebMediaPlayerDelegate> delegate,
                        const media::WebMediaPlayerParams &params);
	~WebMediaPlayerProxy() override;

	void load( LoadType load_type, const blink::WebURL &url, CORSMode cors_mode ) override;

	// Playback controls.
	void play() override;
	void pause() override;
	bool supportsSave() const override;
	void seek(double seconds) override;
	void setRate(double rate) override;
	void setVolume(double volume) override;
	void setPreload(blink::WebMediaPlayer::Preload preload) override;
	blink::WebTimeRanges buffered() const override;
	blink::WebTimeRanges seekable() const override;

	void paint( blink::WebCanvas *canvas, const blink::WebRect &rect, unsigned char alpha, SkXfermode::Mode mode ) override;

	bool hasVideo() const override;
	bool hasAudio() const override;

	// Dimensions of the video.
	blink::WebSize naturalSize() const override;

	// Getters of playback state.
	bool paused() const override;
	bool seeking() const override;
	double duration() const override;
	double currentTime() const override;

	blink::WebMediaPlayer::NetworkState networkState() const override;
	blink::WebMediaPlayer::ReadyState readyState() const override;

	bool didLoadingProgress() override;

	bool hasSingleSecurityOrigin() const override;
	bool didPassCORSAccessCheck() const override;

	double mediaTimeForTimeValue(double timeValue) const override;

	unsigned decodedFrameCount() const override;
	unsigned droppedFrameCount() const override;
	unsigned audioDecodedByteCount() const override;
	unsigned videoDecodedByteCount() const override;

	void setSinkId(const blink::WebString &sink_id, const blink::WebSecurityOrigin &security_origin, blink::WebSetSinkIdCallbacks *web_callbacks) override;
	gfx::Rect videoBounds() const;

protected:
	void SetNetworkState(blink::WebMediaPlayer::NetworkState state);
	void SetReadyState(blink::WebMediaPlayer::ReadyState state);

private:
	bool _paused;
	double _playbackRate;
	double _lastPausedTime;
	base::Time _lastPlayTime;
	blink::WebMediaPlayerClient* _client;
	base::WeakPtr<media::WebMediaPlayerDelegate> _delegate;
	base::ThreadChecker _mainThreadChecker;
	blink::WebMediaPlayer::NetworkState _networkState;
	blink::WebMediaPlayer::ReadyState _readyState;
	scoped_refptr<base::SingleThreadTaskRunner> _compositorTaskRunner;
	VideoFrameProvider *_frameProvider;
	media::SkCanvasVideoRenderer _skcanvasVideoRenderer;
	scoped_ptr<cc_blink::WebLayerImpl> _videoLayer;

	DISALLOW_COPY_AND_ASSIGN(WebMediaPlayerProxy);
};

}
