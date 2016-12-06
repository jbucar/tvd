#pragma once

#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "content/public/browser/browser_main_parts.h"
#include "content/public/common/main_function_params.h"
#include "ui/gfx/geometry/rect.h"

namespace net {
class NetLog;
class URLRequestContextGetter;
}

namespace aura {
class Window;
}

namespace base {
class MessageLoop;
}

namespace content {
class WebContents;
}

namespace devtools_http_handler {
class DevToolsHttpHandler;
}

namespace tvd {

class BrowserContext;
class DisplayConfigurator;
class MainDelegate;
class PlatformDataAura;
class PluginServiceFilter;
class TCPServerSocketFactory;
class CursorWindowDelegate;

class BrowserMainParts : public content::BrowserMainParts {
public:
	explicit BrowserMainParts( MainDelegate *delegate );
	virtual ~BrowserMainParts();

	void createPlatform();
	void shutdown();
	content::WebContents *createWebContents( const gfx::Rect &bounds, int zIndex );
	BrowserContext *browserContext();
	scoped_refptr<net::URLRequestContextGetter> requestContext() const;
	net::NetLog *netLog();

	// Mouse rendering
	void showCursor();
	void hideCursor();
	void moveCursor( const gfx::Point &location );

	// BrowserMainParts implementation:
	void PreEarlyInitialization() override;
	void PreMainMessageLoopStart() override;
	void PostMainMessageLoopStart() override;
	void PreMainMessageLoopRun() override;
	bool MainMessageLoopRun( int *result_code ) override;
	void PostMainMessageLoopRun() override;
	int PreCreateThreads() override;

protected:
	void initDevToolsServer( const std::string &port_str );

private:
	MainDelegate *_delegate;
	scoped_ptr<BrowserContext> _browserCtx;
	scoped_refptr<net::URLRequestContextGetter> _requestCtxGetter;
	scoped_ptr<net::NetLog> _netLog;
	scoped_ptr<base::MessageLoop> _msgLoop;
	scoped_ptr<devtools_http_handler::DevToolsHttpHandler> _devToolsHttpHandler;
	scoped_ptr<PluginServiceFilter> _pluginServiceFilter;
	gfx::Size _bestDisplaySize;
	PlatformDataAura *_platform;
	aura::Window *_rootWin;
	scoped_ptr<CursorWindowDelegate> _cursorDelegate;
	std::unique_ptr<DisplayConfigurator> _displayConfigurator;

	DISALLOW_COPY_AND_ASSIGN(BrowserMainParts);
};

}
