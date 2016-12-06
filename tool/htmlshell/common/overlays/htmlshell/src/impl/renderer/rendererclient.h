#pragma once

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/renderer/content_renderer_client.h"
#include "media/base/key_system_info.h"

namespace web_cache {
class WebCacheRenderProcessObserver;
}

namespace blink {
class WebFrame;
}

namespace tvd {

class SiteInstance;

class RendererClient : public content::ContentRendererClient {
public:
	RendererClient();
	~RendererClient() override;

	static SiteInstance *getSiteInstanceForMainFrame( blink::WebFrame *frame );

	// content::ContentRendererClient implementation:
	void RenderViewCreated( content::RenderView *render_view ) override;
	void RenderThreadStarted() override;
	void AddKeySystems( std::vector<media::KeySystemInfo> *key_systems ) override;

protected:
	void siteInstanceCreated( content::RenderView *renderView, content::RenderFrame *renderFrame );

private:
	SiteInstance *_site;
	scoped_ptr<web_cache::WebCacheRenderProcessObserver> _webCacheObserver;
};

}
