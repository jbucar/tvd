// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webmediaplayer_proxy.h"
#include "video_frame_provider.h"
#include "video_proxy.h"
#include "base/thread_task_runner_handle.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/layers/video_layer.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/blink/webmediaplayer_delegate.h"
#include "media/base/video_frame.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace tvd {

namespace {

// Limits the range of playback rate.
//
// TODO(kylep): Revisit these.
//
// Vista has substantially lower performance than XP or Windows7.  If you speed
// up a video too much, it can't keep up, and rendering stops updating except on
// the time bar. For really high speeds, audio becomes a bottleneck and we just
// use up the data we have, which may not achieve the speed requested, but will
// not crash the tab.
//
// A very slow speed, ie 0.00000001x, causes the machine to lock up. (It seems
// like a busy loop). It gets unresponsive, although its not completely dead.
//
// Also our timers are not very accurate (especially for ogg), which becomes
// evident at low speeds and on Vista. Since other speeds are risky and outside
// the norms, we think 1/16x to 16x is a safe and useful range for now.
const double kMinRate = 0.0625;
const double kMaxRate = 16.0;

}  // namespace

WebMediaPlayerProxy::WebMediaPlayerProxy(
    blink::WebMediaPlayerClient* client,
    base::WeakPtr<media::WebMediaPlayerDelegate> delegate,
    const media::WebMediaPlayerParams &params )
	:
	_paused(true),
	_playbackRate(1.0),
	_lastPausedTime(0.0),
	_client(client),
	_delegate(delegate),
	_networkState(blink::WebMediaPlayer::NetworkStateEmpty),
	_readyState(blink::WebMediaPlayer::ReadyStateHaveNothing),
	_compositorTaskRunner( params.compositor_task_runner() ? params.compositor_task_runner() : base::MessageLoop::current()->task_runner()),
	_frameProvider( new VideoFrameProvider(this) )
{
	DVLOG(1) << __FUNCTION__;
	tvd::video::setAxis(0,0,0,0);
}

WebMediaPlayerProxy::~WebMediaPlayerProxy()
{
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());

	_client->setWebLayer(nullptr);
	if (_delegate) {
		_delegate->PlayerGone(this);
	}
	_compositorTaskRunner->DeleteSoon(FROM_HERE, _frameProvider);

	tvd::video::setFullScreen();
}

void WebMediaPlayerProxy::load( LoadType load_type, const blink::WebURL &url, CORSMode cors_mode ) {
	DVLOG(1) << __FUNCTION__ << "(" << load_type << ", " << url << ", " << cors_mode << ")";

	SetNetworkState(blink::WebMediaPlayer::NetworkStateLoading);
	SetReadyState(blink::WebMediaPlayer::ReadyStateHaveMetadata);
	_client->timeChanged();
}

void WebMediaPlayerProxy::play() {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	if (_paused) {
		_paused = false;
		_lastPlayTime = base::Time::Now();
		if (_delegate) {
			_delegate->DidPlay(this);
		}
	}
}

void WebMediaPlayerProxy::pause() {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	if (!_paused) {
		_lastPausedTime = currentTime();
		_paused = true;
		if (_delegate) {
			_delegate->DidPause(this);
		}
		tvd::video::setFullScreen();
	}
}

bool WebMediaPlayerProxy::supportsSave() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return false;
}

void WebMediaPlayerProxy::seek( double seconds ) {
	DVLOG(1) << __FUNCTION__ << "(" << seconds << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	_lastPausedTime = seconds;
	if (!_paused) {
		_paused = true;
		play();
	}
	_client->timeChanged();
}

void WebMediaPlayerProxy::setRate( double rate ) {
	DVLOG(1) << __FUNCTION__ << "(" << rate << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());

	// TODO(kylep): Remove when support for negatives is added. Also, modify the
	// following checks so rewind uses reasonable values also.
	if (rate < 0.0)
		return;

	// Limit rates to reasonable values by clamping.
	if (rate != 0.0) {
		if (rate < kMinRate)
			rate = kMinRate;
		else if (rate > kMaxRate)
			rate = kMaxRate;
		if (_playbackRate == 0 && !_paused && _delegate)
			_delegate->DidPlay(this);
	} else if (_playbackRate != 0 && !_paused && _delegate) {
		_delegate->DidPause(this);
	}

	_playbackRate = rate;
}

void WebMediaPlayerProxy::setVolume( double volume ) {
	DVLOG(1) << __FUNCTION__ << "(" << volume << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());
}

void WebMediaPlayerProxy::setPreload( WebMediaPlayer::Preload preload ) {
	DVLOG(1) << __FUNCTION__ << "(" << preload << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());
}

blink::WebTimeRanges WebMediaPlayerProxy::buffered() const {
	media::Ranges<base::TimeDelta> ranges;
	ranges.Add( base::TimeDelta(), base::TimeDelta::FromSeconds(duration()) );
	for (size_t i=0; i<ranges.size(); i++) {
		base::TimeDelta s=ranges.start(i);
		base::TimeDelta e=ranges.end(i);
		DVLOG(1) << __FUNCTION__ << "(" << i << ")=" <<  s.InSeconds() << "," << e.InSeconds();
	}
	return media::ConvertToWebTimeRanges(ranges);
}

blink::WebTimeRanges WebMediaPlayerProxy::seekable() const {
	const blink::WebTimeRange seekable_range(0.0, duration());
	return blink::WebTimeRanges(&seekable_range, 1);
}

bool WebMediaPlayerProxy::hasVideo() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return true;
}

bool WebMediaPlayerProxy::hasAudio() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return true;
}

blink::WebSize WebMediaPlayerProxy::naturalSize() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	gfx::Size res = tvd::video::getVideoResolution();
	DVLOG(1) << __FUNCTION__ << "(" << res.width() << "," << res.height() << ")";
	return res;
}

bool WebMediaPlayerProxy::paused() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	DVLOG(1) << __FUNCTION__ << "(" << _paused << ")";
	return _paused;
}

bool WebMediaPlayerProxy::seeking() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return false;
}

double WebMediaPlayerProxy::duration() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());

	double tmp=std::numeric_limits<double>::infinity();
	DVLOG(1) << __FUNCTION__ << "(" << tmp << ")";
	return tmp;
}

double WebMediaPlayerProxy::currentTime() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	double curr = _lastPausedTime;
	if (!_paused) {
		base::Time now = base::Time::Now();
		base::TimeDelta delta = now - _lastPlayTime;
		curr = delta.InSecondsF() + _lastPausedTime;
	}
	DVLOG(1) << __FUNCTION__ << "(" << curr << ")";
	return curr;
}

blink::WebMediaPlayer::NetworkState WebMediaPlayerProxy::networkState() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return _networkState;
}

blink::WebMediaPlayer::ReadyState WebMediaPlayerProxy::readyState() const {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return _readyState;
}

bool WebMediaPlayerProxy::didLoadingProgress() {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return true;
}

bool WebMediaPlayerProxy::hasSingleSecurityOrigin() const {
	DVLOG(1) << __FUNCTION__;
	return true;
}

bool WebMediaPlayerProxy::didPassCORSAccessCheck() const {
	DVLOG(1) << __FUNCTION__;
	return true;
}

gfx::Rect WebMediaPlayerProxy::videoBounds() const {
	return tvd::video::computeVideoBounds(_videoLayer->layer());
}

void WebMediaPlayerProxy::paint(blink::WebCanvas* canvas, const blink::WebRect& rect, unsigned char alpha, SkXfermode::Mode mode) {
	DVLOG(1) << __FUNCTION__ << "(" << rect.x << "," << rect.y << "," << rect.width << "," << rect.height << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());

	tvd::video::setAxis( rect.x, rect.y, rect.width, rect.height );
	_skcanvasVideoRenderer.Copy(media::VideoFrame::CreateTransparentFrame(gfx::Size(rect.width, rect.height)).get(), canvas, media::Context3D());

	if (!_videoLayer) {
		scoped_refptr<cc::VideoLayer> layer = cc::VideoLayer::Create(cc_blink::WebLayerImpl::LayerSettings(), _frameProvider, media::VIDEO_ROTATION_0);
		_videoLayer.reset(new cc_blink::WebLayerImpl(layer));
		_videoLayer->setOpaque(false);
		_client->setWebLayer(_videoLayer.get());
		SetReadyState(blink::WebMediaPlayer::ReadyStateHaveEnoughData);
	}
}

double WebMediaPlayerProxy::mediaTimeForTimeValue(double timeValue) const {
	DVLOG(1) << __FUNCTION__;
	return base::TimeDelta::FromSecondsD(timeValue).InSecondsF();
}

unsigned WebMediaPlayerProxy::decodedFrameCount() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return 0;
}

unsigned WebMediaPlayerProxy::droppedFrameCount() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return 0;
}

unsigned WebMediaPlayerProxy::audioDecodedByteCount() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return 0;
}

unsigned WebMediaPlayerProxy::videoDecodedByteCount() const {
	DVLOG(1) << __FUNCTION__;
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	return 0;
}

void WebMediaPlayerProxy::SetNetworkState(blink::WebMediaPlayer::NetworkState state) {
	DVLOG(1) << __FUNCTION__ << "(" << state << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	_networkState = state;
	_client->networkStateChanged();
}

void WebMediaPlayerProxy::SetReadyState(blink::WebMediaPlayer::ReadyState state) {
	DVLOG(1) << __FUNCTION__ << "(" << state << ")";
	DCHECK(_mainThreadChecker.CalledOnValidThread());

	if (state == WebMediaPlayer::ReadyStateHaveEnoughData &&
		_networkState == WebMediaPlayer::NetworkStateLoading) {
		SetNetworkState(WebMediaPlayer::NetworkStateLoaded);
	}
	_readyState = state;
	_client->readyStateChanged();
}

void WebMediaPlayerProxy::setSinkId(const blink::WebString &sink_id, const blink::WebSecurityOrigin &security_origin, blink::WebSetSinkIdCallbacks *web_callbacks) {
	DCHECK(_mainThreadChecker.CalledOnValidThread());
	scoped_ptr<blink::WebSetSinkIdCallbacks> callback(web_callbacks);
	callback->onError(blink::WebSetSinkIdError::NotSupported);
}

}
