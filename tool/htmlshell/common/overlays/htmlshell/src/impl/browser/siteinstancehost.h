#pragma once

#include "dtv-htmlshellsdk/src/apis/browser.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/permission_type.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include <vector>

namespace content {
class WebContents;
class JavaScriptDialogManager;
}

namespace tvd {

class BrowserClient;
class SiteInfo;

class SiteInstanceHost : public content::WebContentsDelegate,
                         public content::WebContentsObserver {
public:
	SiteInstanceHost( BrowserClient *client, int id, content::WebContents *webContents, const tvd::browser::Options &opts );
	~SiteInstanceHost() override;

	int id() const;
	int processId() const;
	int zIndex() const;
	void load( const std::string &url );
	void show( bool needShow, bool keyboardFocus );
	void update();
	void close();

	void executeCallback( int queryId, const std::string &error, const std::string &params, bool isSignal );
	const std::string &initScript() const;
	void signalLaunched();
	void addAllowedPlugin( const std::string &name );
	bool isPluginAllowed( const std::string &name ) const;
	bool hasPermission( content::PermissionType permission );
	content::WebContents *webContents();

	// Render IDs
	bool viewIdMatch( int pid, int routingId ) const;
	bool frameIdMatch( int pid, int routingId ) const;
	void addViewId( int pid, int routingId );
	void addFrameId( int pid, int routingId );

	// content::WebContentsDelegate implementation:
	bool AddMessageToConsole( content::WebContents *source, int32_t level, const base::string16 &message, int32_t line_no, const base::string16 &source_id ) override;
	bool PreHandleKeyboardEvent( content::WebContents *source, const content::NativeWebKeyboardEvent &event, bool *isShortcut ) override;
	void BeforeUnloadFired( content::WebContents *tab, bool proceed, bool* proceed_to_fire_unload ) override;
	void CloseContents( content::WebContents *source ) override;
	content::JavaScriptDialogManager *GetJavaScriptDialogManager( content::WebContents *source ) override;

	// content::WebContentsObserver implementation:
	bool OnMessageReceived( const IPC::Message &message ) override;
	void RenderViewCreated( content::RenderViewHost *render_view_host ) override;
	void RenderProcessGone( base::TerminationStatus status ) override;
	void DidFailProvisionalLoad( content::RenderFrameHost *render_frame_host, const GURL &validated_url, int error_code, const base::string16 &error_description, bool was_ignored_by_handler ) override;
	void DocumentOnLoadCompletedInMainFrame() override;
	void EnterFullscreenModeForTab( content::WebContents *web_contents, const GURL &origin ) override;
	void ExitFullscreenModeForTab( content::WebContents *web_contents ) override;
	void ToggleFullscreenModeForTab( content::WebContents *web_contents, bool enter_fullscreen );
	bool IsFullscreenForTabOrPending( const content::WebContents *web_contents ) const override;

	bool handleMouseEvent( const gfx::Point &location );
	aura::Window *getWindow();
	bool mouseEnabled();

protected:
	// Message handlers.
	void onExtQuery( int queryId, const std::string &msg, const base::ListValue &params );
	void processQuery( int queryId, const std::string &msg, const std::string &params );

	typedef std::set<std::pair<int, int> > RenderIdSet;
	void addId( RenderIdSet *set, int pid, int routingId );
	bool idMatch( const RenderIdSet *set, int pid, int routingId ) const;

	bool canShowCursor();

private:
	int _id;
	bool _signaled;
	bool _isShowingCursor;
	bool _isFullscreen;
	tvd::browser::Options _opts;
	BrowserClient *_client;
	RenderIdSet _renderViewIds;
	RenderIdSet _renderFrameIds;
	std::vector<std::string> _allowedPlugins;
	scoped_ptr<content::JavaScriptDialogManager> _dialogManager;
	scoped_ptr<content::WebContents> _webContents;
};

}
