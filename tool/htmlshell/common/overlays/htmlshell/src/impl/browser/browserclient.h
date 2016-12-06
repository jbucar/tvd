#pragma once

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/aura/client/event_client.h"
#include <list>

namespace ui {
class ScopedEventDispatcher;
}

namespace net {
class URLRequestJobFactoryImpl;
}

namespace tvd {

namespace browser {
typedef struct OptionsS Options;
}

class BrowserMainParts;
class MainDelegate;
class SiteInstanceHost;
class URLRequestContextGetter;

class BrowserClient : public content::ContentBrowserClient,
                      public ui::PlatformEventDispatcher,
                      public aura::client::EventClient {
public:
	explicit BrowserClient( MainDelegate *delegate );
	~BrowserClient() override;

	MainDelegate *delegate();

	bool createAndLoadSite( int id, const std::string &url, const browser::Options &opts );
	virtual bool closeSite( int id );
	virtual void onSiteClosed( int id, const std::string &error="" );
	virtual void closeAllSites();

	void updateSites();
	virtual bool showSite( int id, bool show, bool keyboardFocus );

	void showCursor();
	void hideCursor();

	SiteInstanceHost *updateSite( int viewPid, int viewRoutingId, int framePid, int frameRoutingId );
	bool executeCallback( int id, int queryId, const std::string &error, const std::string &params, bool isSignal );
	bool isPluginAllowedForSite( int renderProcId, const std::string &name );
	bool siteHasPermission( content::WebContents *webContents, content::PermissionType permission );

	void shutdown();

	net::URLRequestJobFactoryImpl *jobFactory() const;

	void setOriginDispatcher( scoped_ptr<ui::ScopedEventDispatcher> disp );

	// ui::PlatformEventDispatcher implementation:
	bool CanDispatchEvent( const ui::PlatformEvent &event ) override;
	uint32_t DispatchEvent( const ui::PlatformEvent &event ) override;

	// aura::client::EventClient implementation:
	bool CanProcessMouseEventsWithinSubtree( const aura::Window *window ) const override;
	bool CanProcessEventsWithinSubtree( const aura::Window *window ) const override;
	ui::EventTarget *GetToplevelEventTarget() override;

	// ContentBrowserClient implementation:
	content::BrowserMainParts *CreateBrowserMainParts( const content::MainFunctionParams &parameters ) override;
	net::URLRequestContextGetter *CreateRequestContext( content::BrowserContext* browser_context,
                                                        content::ProtocolHandlerMap* protocol_handlers,
                                                        content::URLRequestInterceptorScopedVector request_interceptors ) override;
	net::URLRequestContextGetter *CreateRequestContextForStoragePartition( content::BrowserContext* browser_context,
                                                                           const base::FilePath& partition_path,
                                                                           bool in_memory,
                                                                           content::ProtocolHandlerMap* protocol_handlers,
                                                                           content::URLRequestInterceptorScopedVector request_interceptors ) override;
	bool IsHandledURL( const GURL& url ) override;
	void OverrideWebkitPrefs( content::RenderViewHost *rvh, content::WebPreferences *prefs ) override;
	void RenderProcessWillLaunch( content::RenderProcessHost *host ) override;
	void AppendExtraCommandLineSwitches( base::CommandLine *command_line, int child_process_id ) override;
	content::AccessTokenStore *CreateAccessTokenStore() override;
	void DidCreatePpapiPlugin( content::BrowserPpapiHost *browser_host );

#if defined(OS_POSIX) && !defined(OS_MACOSX)
	void GetAdditionalMappedFilesForChildProcess( const base::CommandLine &command_line, int child_process_id, content::FileDescriptorInfo *mappings ) override;
#endif  // defined(OS_POSIX) && !defined(OS_MACOSX)

protected:
	virtual void createSiteInstance( int id, const std::string &url, const browser::Options &opts );
	virtual void notifyBrowserClosed( int id, const std::string &error );

	SiteInstanceHost *getSite( int id );
	bool siteExists( int id );

	void showSiteOnUIT( int id, bool show, bool keyboardFocus );
	void closeSiteOnUIT( int id );
	void executeCallbackOnUIT( int id, int queryId, const std::string &error, const std::string &params, bool isSignal );
	void checkPluginAllowedForSiteOnUIT( int renderProcId, const std::string &name, bool *result );

	typedef std::list<SiteInstanceHost*> SiteList;

private:
	MainDelegate *_delegate;
	BrowserMainParts *_mainParts;
	SiteList _sites;
	scoped_ptr<ui::ScopedEventDispatcher> _origDispatcher;
	scoped_refptr<URLRequestContextGetter> _urlRequestGetter;
};

}
