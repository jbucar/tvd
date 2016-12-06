#include "browserclient.h"
#include "accesstokenstore.h"
#include "browsercontext.h"
#include "browsermainparts.h"
#include "messagefilter.h"
#include "browserpepperhostfactory.h"
#include "siteinstancehost.h"
#include "urlrequestcontextgetter.h"
#include "../app/maindelegate.h"
#include "../../util.h"
#include "../../switches.h"
#include "../../apis/browser/browserapi.h"
#include "base/auto_reset.h"
#include "base/base_switches.h"
#include "base/bind.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string_util.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/browser_ppapi_host.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/resource_context.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/web_preferences.h"
#include "ui/aura/env.h"
#include "ui/aura/window.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_utils.h"
#include "ui/events/platform/scoped_event_dispatcher.h"
#include "ui/gfx/image/image_skia.h"
#include "ppapi/host/ppapi_host.h"

#include "dtv-htmlshellsdk/src/apis/browser.h"

#if defined(OS_POSIX) && !defined(OS_MACOSX)
#	include "base/debug/leak_annotations.h"
#	include "components/crash/content/app/breakpad_linux.h"
#	include "components/crash/content/browser/crash_handler_host_linux.h"
#	include "content/public/common/content_descriptors.h"
#endif

namespace tvd {

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_ANDROID)
breakpad::CrashHandlerHostLinux *CreateCrashHandlerHost( const std::string &pType ) {
	base::FilePath dumps_path = base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(switches::kSysProfile).Append("crashes");
	{
		LOG(INFO) << "Createing crash handler host for process: " << pType;
		ANNOTATE_SCOPED_MEMORY_LEAK;
		breakpad::CrashHandlerHostLinux *crash_handler = new breakpad::CrashHandlerHostLinux(pType, dumps_path, false);
		crash_handler->StartUploaderThread();
		return crash_handler;
	}
}

int GetCrashSignalFD( const base::CommandLine &cmd ) {
	if (!breakpad::IsCrashReporterEnabled())
		return -1;

	std::string pType = cmd.GetSwitchValueASCII(switches::kProcessType);

	if (pType == switches::kRendererProcess) {
		static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
		if (!crash_handler)
			crash_handler = CreateCrashHandlerHost(pType);
		return crash_handler->GetDeathSignalSocket();
	}

	if (pType == switches::kPluginProcess) {
		static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
		if (!crash_handler)
			crash_handler = CreateCrashHandlerHost(pType);
		return crash_handler->GetDeathSignalSocket();
	}

	if (pType == switches::kPpapiPluginProcess) {
		static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
		if (!crash_handler)
			crash_handler = CreateCrashHandlerHost(pType);
		return crash_handler->GetDeathSignalSocket();
	}

	if (pType == switches::kGpuProcess) {
		static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
		if (!crash_handler)
			crash_handler = CreateCrashHandlerHost(pType);
		return crash_handler->GetDeathSignalSocket();
	}

	return -1;
}
#endif  // defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_ANDROID)

struct SortSites {
	bool operator()( SiteInstanceHost *s1, SiteInstanceHost *s2 ) const {
		return s1->zIndex() < s2->zIndex();
	}
};

BrowserClient::BrowserClient( MainDelegate *delegate )
{
	_delegate = delegate;
	_mainParts = NULL;
}

BrowserClient::~BrowserClient()
{
	_mainParts = NULL;
	if (!_sites.empty()) {
		LOG(WARNING) << "Not all site instances destroyed! sites=" << _sites.size();
		closeAllSites();
	}
}

MainDelegate *BrowserClient::delegate() {
	DCHECK(_delegate);
	return _delegate;
}

bool BrowserClient::createAndLoadSite( int id, const std::string &url, const browser::Options &opts ) {
	if (siteExists(id)) {
		LOG(WARNING) << "Fail to create site, id=" << id << " already in use";
		return false;
	}
	LOG(INFO) << "Createing site id=" << id << ", url=" << url;
	createSiteInstance(id, url, opts);
	return true;
}

void BrowserClient::closeAllSites() {
	if (!TVD_CURRENTLY_ON_UIT()) {
		util::postTask(TVD_UIT, base::Bind(&BrowserClient::closeAllSites, base::Unretained(this)));
	} else {
		for (SiteList::iterator it=_sites.begin(); it != _sites.end(); it++) {
			delete (*it);
		}
		_sites.clear();
	}
}

bool BrowserClient::closeSite( int id ) {
	if (siteExists(id)) {
		closeSiteOnUIT(id);
		return true;
	}
	return false;
}

void BrowserClient::closeSiteOnUIT( int id ) {
	if (!TVD_CURRENTLY_ON_UIT()) {
		util::postTask(TVD_UIT, base::Bind(&BrowserClient::closeSiteOnUIT, base::Unretained(this), id));
	} else {
		LOG(INFO) << "Closing site id=" << id;
		for (SiteList::iterator it = _sites.begin(); it != _sites.end(); it++) {
			if ((*it)->id() == id) {
				(*it)->close();
				break;
			}
		}
	}
}

void BrowserClient::onSiteClosed( int id, const std::string &error ) {
	TVD_REQUIRE_UIT();
	LOG(INFO) << "On site closed id=" << id << ", error=" << error;
	for (SiteList::iterator it = _sites.begin(); it != _sites.end(); it++) {
		if ((*it)->id() == id) {
			notifyBrowserClosed(id, error);
			delete *it;
			_sites.erase(it);
			break;
		}
	}
	updateSites();
}

bool BrowserClient::showSite( int id, bool show, bool keyboardFocus ) {
	if (siteExists(id)) {
		showSiteOnUIT(id, show, keyboardFocus);
		return true;
	}
	return false;
}

void BrowserClient::showSiteOnUIT( int id, bool show, bool keyboardFocus ) {
	if (!TVD_CURRENTLY_ON_UIT()) {
		util::postTask(TVD_UIT, base::Bind(&BrowserClient::showSiteOnUIT, base::Unretained(this), id, show, keyboardFocus));
	} else {
		SiteInstanceHost *site = getSite(id);
		if (site) {
			site->show(show, keyboardFocus);
		}
	}
}

bool BrowserClient::executeCallback( int id, int queryId, const std::string &error, const std::string &params, bool isSignal ) {
	if (siteExists(id)) {
		executeCallbackOnUIT(id, queryId, error, params, isSignal);
		return true;
	}
	return false;
}

bool BrowserClient::isPluginAllowedForSite( int renderProcId, const std::string &name ) {
	for (SiteInstanceHost *site : _sites) {
		if (site->processId() == renderProcId) {
			return site->isPluginAllowed(name);
		}
	}
	return false;
}

void BrowserClient::executeCallbackOnUIT( int id, int queryId, const std::string &error, const std::string &params, bool isSignal ) {
	if (!TVD_CURRENTLY_ON_UIT()) {
		util::postTask(TVD_UIT, base::Bind(&BrowserClient::executeCallbackOnUIT, base::Unretained(this), id, queryId, error, params, isSignal));
	} else {
		SiteInstanceHost *site = getSite(id);
		if (site) {
			site->executeCallback(queryId, error, params, isSignal);
		}
	}
}

bool BrowserClient::siteExists( int id ) {
	for (SiteInstanceHost *site : _sites) {
		if (site->id() == id) {
			return true;
		}
	}
	return false;
}

SiteInstanceHost *BrowserClient::getSite( int id ) {
	for (SiteInstanceHost *site : _sites) {
		if (site->id() == id) {
			return site;
		}
	}
	return NULL;
}

void BrowserClient::createSiteInstance( int id, const std::string &url, const browser::Options &opts ) {
	if (!TVD_CURRENTLY_ON_UIT()) {
		util::postTask(TVD_UIT, base::Bind(&BrowserClient::createSiteInstance, base::Unretained(this), id, url, opts));
	} else {
		gfx::Rect rect(opts.bounds.x, opts.bounds.y, opts.bounds.w, opts.bounds.h);
		content::WebContents *contents = _mainParts->createWebContents(rect, opts.zIndex);
		SiteInstanceHost *site = new SiteInstanceHost(this, id, contents, opts);

		_sites.push_back(site);
		_sites.sort(SortSites());

		// Add allowed plugins from browser::options
		for (std::string plugIn : opts.plugins) {
			site->addAllowedPlugin(plugIn);
		}
		// Add allowed plugins from command line
		base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
		if (cmd->HasSwitch(switches::kAllowedPlugins)) {
			std::stringstream ss(cmd->GetSwitchValueASCII(switches::kAllowedPlugins));
			std::string pName;
			while (std::getline(ss, pName, ',')) {
				LOG(INFO) << "Adding allowed plugin to site id=" << id << " plugin=" << pName;
				site->addAllowedPlugin(pName);
			}
		}

		site->load(url);
		content::RenderViewHost *renderView = contents->GetRenderViewHost();
		site->addViewId(renderView->GetProcess()->GetID(), renderView->GetRoutingID());
		site->show(opts.visible, opts.needFocus);
	}
}

SiteInstanceHost *BrowserClient::updateSite( int viewPid, int viewRoutingId, int framePid, int frameRoutingId ) {
	for (SiteInstanceHost *site : _sites) {
		if (site->viewIdMatch(viewPid, viewRoutingId)) {
			site->addFrameId(framePid, frameRoutingId);
			return site;
		}
		else if (site->frameIdMatch(framePid, frameRoutingId)) {
			site->addViewId(viewPid, viewRoutingId);
			return site;
		}
	}

	LOG(WARNING) << "Site instance for view(" << viewPid << "," << viewRoutingId << ") "
	             << "and frame(" << framePid << "," << frameRoutingId << ") doesn't exist";
	return NULL;
}

void BrowserClient::updateSites() {
	TVD_REQUIRE_UIT();
	for (SiteInstanceHost *site : _sites) {
		site->update();
	}
}

void BrowserClient::notifyBrowserClosed( int id, const std::string &error) {
	ShellApi *api = _delegate->getShellApi(BROWSER_API);
	if (api) {
		static_cast<BrowserApi*>(api)->browserClosed(id, error);
	}
}

void BrowserClient::shutdown() {
	_mainParts->shutdown();
}

net::URLRequestJobFactoryImpl *BrowserClient::jobFactory() const {
	URLRequestContextGetter *reqCtx = static_cast<URLRequestContextGetter*>(_mainParts->requestContext().get());
	if (reqCtx) {
		return reqCtx->jobFactory();
	}
	return nullptr;
}

content::BrowserMainParts *BrowserClient::CreateBrowserMainParts( const content::MainFunctionParams &/*params*/ ) {
	LOG(INFO) << "Browser main parts created";
	// This object becomes a scoped_ptr member of content::BrowserMainLoop so don't delete it here.
	// See (content/browser/browser_main_loop.h)
	_mainParts = new BrowserMainParts(_delegate);
	return _mainParts;
}

net::URLRequestContextGetter* BrowserClient::CreateRequestContext( content::BrowserContext * /*browser_context*/,
                                                                   content::ProtocolHandlerMap *protocol_handlers,
                                                                   content::URLRequestInterceptorScopedVector request_interceptors )
{
	VLOG(1) << "Create request context";
	DCHECK(!_urlRequestGetter.get());
	BrowserContext *browserCtx = _mainParts->browserContext();
	_urlRequestGetter = new URLRequestContextGetter( browserCtx->GetPath(),
                                                     content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::IO),
                                                     content::BrowserThread::UnsafeGetMessageLoopForThread(content::BrowserThread::FILE),
                                                     protocol_handlers,
                                                     std::move(request_interceptors),
                                                     _mainParts->netLog() );
	browserCtx->setUrlRequestContextGetter( _urlRequestGetter.get() );
	return _urlRequestGetter.get();
}

net::URLRequestContextGetter* BrowserClient::CreateRequestContextForStoragePartition( content::BrowserContext *browser_context,
                                                                                      const base::FilePath& partition_path,
                                                                                      bool in_memory,
                                                                                      content::ProtocolHandlerMap *protocol_handlers,
                                                                                      content::URLRequestInterceptorScopedVector request_interceptors )
{
	VLOG(1) << "Create request context for storage partition";
	return nullptr;
}

bool BrowserClient::IsHandledURL( const GURL& url ) {
	VLOG(1) << "IsHandledURL url=" << url.spec();
	if (url.is_valid()) {
		DCHECK_EQ(url.scheme(), base::ToLowerASCII(url.scheme()));
		// Keep in sync with ProtocolHandlers added by URLRequestContextGetter::GetURLRequestContext().
		static const char* const kProtocolList[] = {
			content::kChromeUIScheme,
			content::kChromeDevToolsScheme,
			url::kBlobScheme,
			url::kFileSystemScheme,
			url::kDataScheme,
			url::kFileScheme,
		};
		for (size_t i=0; i < arraysize(kProtocolList); ++i) {
			if (url.scheme() == kProtocolList[i]) {
				return true;
			}
		}
	}
	return false;
}

void BrowserClient::OverrideWebkitPrefs( content::RenderViewHost *rvh, content::WebPreferences *prefs ) {
	// NOTE: La lista completa de preferencias de WebKit esta en el methodo RenderViewHostImpl::ComputeWebkitPrefs
	// en el archivo $CHROMIUM_SRC/content/browser/renderer_host/render_view_host_impl.cc
	prefs->touch_enabled = false;
	prefs->device_supports_touch = false;
}

void BrowserClient::RenderProcessWillLaunch( content::RenderProcessHost *host ) {
	LOG(INFO) << "New render process will be launched";
	host->GetChannel()->AddFilter(new MessageFilter(this, host));
}

#if defined(OS_POSIX) && !defined(OS_MACOSX)
void BrowserClient::GetAdditionalMappedFilesForChildProcess( const base::CommandLine &cmd, int child_pid, content::FileDescriptorInfo *mappings ) {
	LOG(INFO) << "GetAdditionalMappedFilesForChildProcess child_pid=" << child_pid;
	int crash_signal_fd = GetCrashSignalFD(cmd);
	if (crash_signal_fd >= 0) {
		mappings->Share(kCrashDumpSignal, crash_signal_fd);
	}
}
#endif  // defined(OS_POSIX) && !defined(OS_MACOSX)

void BrowserClient::AppendExtraCommandLineSwitches( base::CommandLine *command_line, int child_process_id ) {
	const char *switches[] = {
		switches::kEnableCrashReporter,
		switches::kSysProfile,
		switches::kUsrProfile,
		switches::kExtraUA,
		switches::kWindowSize
	};
	command_line->CopySwitchesFrom(*base::CommandLine::ForCurrentProcess(), switches, 5);
}

bool BrowserClient::CanProcessMouseEventsWithinSubtree( const aura::Window *window ) const {
	for (SiteInstanceHost *site : _sites) {
		if (site->getWindow() == window) {
			return site->mouseEnabled();
		}
	}
	return true;
}

bool BrowserClient::CanProcessEventsWithinSubtree( const aura::Window *window ) const {
	return true;
}

ui::EventTarget *BrowserClient::GetToplevelEventTarget() {
	return aura::Env::GetInstance();
}

bool BrowserClient::CanDispatchEvent( const ui::PlatformEvent &event ) {
	LOG(INFO) << "BrowserClient::CanDispatchEvent()";
	return true;
}

uint32_t BrowserClient::DispatchEvent( const ui::PlatformEvent &event ) {
	if (!ui::EventFromNative(event)->IsMouseEvent()) {
		return ui::POST_DISPATCH_PERFORM_DEFAULT;
	}

	// Check if any site accepts mouse events and process
	gfx::Point location = ui::EventSystemLocationFromNative(event);
	ui::EventType type = ui::EventTypeFromNative(event);
	for (SiteInstanceHost *site : _sites) {
		if (site->handleMouseEvent(location)) {
			if (type == ui::ET_MOUSE_DRAGGED || type == ui::ET_MOUSE_MOVED) {
				_mainParts->moveCursor(location);
			}
			return ui::POST_DISPATCH_PERFORM_DEFAULT;
		}
	}
	// Any site accepts mouse events so ignore this event
	return ui::POST_DISPATCH_STOP_PROPAGATION;
}

void BrowserClient::setOriginDispatcher( scoped_ptr<ui::ScopedEventDispatcher> disp ) {
	_origDispatcher.reset(disp.release());
}

void BrowserClient::showCursor() {
	_mainParts->showCursor();
}

void BrowserClient::hideCursor() {
	_mainParts->hideCursor();
}

content::AccessTokenStore *BrowserClient::CreateAccessTokenStore() {
	return new AccessTokenStore(_mainParts->browserContext());
}

bool BrowserClient::siteHasPermission( content::WebContents *webContents, content::PermissionType permission ) {
	TVD_REQUIRE_UIT();
	for (SiteInstanceHost *site : _sites) {
		if (site->webContents() == webContents) {
			return site->hasPermission(permission);
		}
	}
	return false;
}

void BrowserClient::DidCreatePpapiPlugin( content::BrowserPpapiHost *browser_host ) {
	browser_host->GetPpapiHost()->AddHostFactoryFilter(scoped_ptr<ppapi::host::HostFactory>(new BrowserPepperHostFactory(browser_host)));
}

}
