#include "rendererclient.h"
#include "siteinstance.h"
#include "../common/client.h"
#include "../common/messages.h"
#include "../../util.h"
#include "base/logging.h"
#include "components/web_cache/renderer/web_cache_render_process_observer.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_frame.h"
#include "components/cdm/renderer/widevine_key_systems.h"
#include "media/base/eme_constants.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "widevine_cdm_version.h"

namespace tvd {

namespace impl {
static RendererClient *client = NULL;
}

RendererClient::RendererClient()
{
	CHECK(!impl::client);
	impl::client = this;
	_site = nullptr;
}

RendererClient::~RendererClient()
{
	CHECK(impl::client == this);
	impl::client = NULL;
	if (_site) {
		delete _site;
		_site = nullptr;
	}
}

// static
SiteInstance *RendererClient::getSiteInstanceForMainFrame( blink::WebFrame *frame ) {
	TVD_REQUIRE_RT_RETURN(nullptr);
	content::RenderView *renderView = impl::client->_site->render_view();
	if (renderView && renderView->GetWebView() && renderView->GetWebView()->mainFrame() == frame) {
		return impl::client->_site;
	}
	return nullptr;
}

void RendererClient::RenderThreadStarted() {
	_webCacheObserver.reset(new web_cache::WebCacheRenderProcessObserver());
	content::RenderThread::Get()->AddObserver(_webCacheObserver.get());
}

void RendererClient::RenderViewCreated( content::RenderView *render_view ) {
	siteInstanceCreated(render_view, render_view->GetMainRenderFrame());
}

void RendererClient::siteInstanceCreated( content::RenderView *renderView, content::RenderFrame *renderFrame ) {
	// Probably a second frame was loaded
	if (!_site) {
		// Retrieve the site id synchronously. This will also register the
		// routing ids with the site info object in the browser process.
		int siteId;
		std::string js;
		content::RenderThread::Get()->Send(new Tvd_GetNewSiteInfo(renderView->GetRoutingID(), renderFrame->GetRoutingID(), &siteId, &js));
		CHECK(siteId >= 0);

		VLOG(1) << "New site instance created";
		_site = new SiteInstance(renderView, siteId, js);
	} else {
		LOG(WARNING) << "A site instance already exists in this renderer";
	}
}

void RendererClient::AddKeySystems( std::vector<media::KeySystemInfo> *key_systems ) {
#if defined(WIDEVINE_CDM_AVAILABLE)
	media::SupportedCodecs codecs =
		  media::EME_CODEC_MP4_AAC
		| media::EME_CODEC_MP4_AVC1
		| media::EME_CODEC_WEBM_VP8
		| media::EME_CODEC_WEBM_VP9
		| media::EME_CODEC_WEBM_OPUS
		| media::EME_CODEC_WEBM_VORBIS;
	AddWidevineWithCodecs(cdm::WIDEVINE, codecs,
		media::EmeRobustness::HW_SECURE_ALL,          // Max audio robustness.
		media::EmeRobustness::HW_SECURE_ALL,          // Max video robustness.
		media::EmeSessionTypeSupport::NOT_SUPPORTED,  // persistent-license.
		media::EmeSessionTypeSupport::NOT_SUPPORTED,  // persistent-release-message.
		media::EmeFeatureSupport::REQUESTABLE,        // Persistent state.
		media::EmeFeatureSupport::NOT_SUPPORTED,      // Distinctive identifier.
		key_systems);
#endif
}

}
