#include "browsermainparts.h"
#include "browsercontext.h"
#include "cursorwindowdelegate.h"
#include "displayconfigurator.h"
#include "platformdataaura.h"
#include "pluginservicefilter.h"
#include "devtools/delegate.h"
#include "devtools/tcpserversocketfactory.h"
#include "../app/maindelegate.h"
#include "../common/client.h"
#include "../browser/browserclient.h"
#include "../../switches.h"
#include "../../util.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/geolocation_provider.h"
#include "content/public/browser/gpu_data_manager.h"
#include "content/public/browser/plugin_service.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "ipc/ipc_message.h"
#include "net/base/filename_util.h"
#include "net/log/net_log.h"
#include "net/url_request/url_request_context_getter.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#include "ui/base/ime/input_method_initializer.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/platform/scoped_event_dispatcher.h"
#include "ui/gfx/geometry/size.h"
#include "ui/ozone/public/ozone_switches.h"

namespace tvd {

BrowserMainParts::BrowserMainParts( MainDelegate *delegate )
	: _delegate(delegate)
{
	_platform = nullptr;
	_rootWin = nullptr;
}

BrowserMainParts::~BrowserMainParts()
{
	CHECK(!_platform);
	CHECK(!_rootWin);
	CHECK(!_devToolsHttpHandler);
}

BrowserContext *BrowserMainParts::browserContext() {
	return _browserCtx.get();
}

scoped_refptr<net::URLRequestContextGetter> BrowserMainParts::requestContext() const {
	return _requestCtxGetter;
}

net::NetLog *BrowserMainParts::netLog() {
	return _netLog.get();
}

content::WebContents *BrowserMainParts::createWebContents( const gfx::Rect &bounds, int zIndex ) {
	VLOG(1) << "Createing web contents";
	gfx::Size defaultSize = util::getWindowSize();
	content::WebContents::CreateParams params(_browserCtx.get(), nullptr);
	params.routing_id = MSG_ROUTING_NONE;
	params.initial_size = bounds.IsEmpty() ? defaultSize : bounds.size();
	content::WebContents *contents = content::WebContents::Create(params);
	gfx::Rect tmp = bounds;
	if (tmp.IsEmpty()) {
		tmp.set_size(defaultSize);
	}

	aura::Window *child = contents->GetNativeView();
	child->SetName("HTMLSHELL_BROWSER");
	child->set_id(zIndex);
	child->SetBounds(tmp);
	if (!_rootWin->Contains(child)) {
		_rootWin->AddChild(child);
	}
	return contents;
}

void BrowserMainParts::shutdown() {
	LOG(INFO) << "Shuting down browser main parts";
	if (_platform) {
		delete _platform;
		_rootWin = nullptr;
		_platform = nullptr;
	}
}

void BrowserMainParts::PreEarlyInitialization() {
#if !defined(OS_CHROMEOS) && defined(USE_AURA) && defined(OS_LINUX)
	ui::InitializeInputMethodForTesting();
#endif
#if defined(OS_ANDROID)
	net::NetworkChangeNotifier::SetFactory(new net::NetworkChangeNotifierFactoryAndroid());
#endif
}

void BrowserMainParts::PreMainMessageLoopStart() {
#if defined(USE_AURA) && defined(USE_X11)
	ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif
	if (!base::MessageLoop::current()) {
		// Create the browser message loop.
		_msgLoop.reset(new base::MessageLoopForUI());
		_msgLoop->set_thread_name("BrowserMainPartsT");
	}
}

void BrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_ANDROID)
	base::MessageLoopForUI::current()->Start();
#endif
}

void BrowserMainParts::PreMainMessageLoopRun() {
	// Enable geolocation
	content::GeolocationProvider::GetInstance()->UserDidOptIntoLocationServices();

	_netLog.reset(new net::NetLog());
	_browserCtx.reset(new BrowserContext(_delegate->browserClient()));
	_requestCtxGetter = _browserCtx->GetRequestContext();

	const base::CommandLine *cmd = base::CommandLine::ForCurrentProcess();
	if (cmd->HasSwitch(switches::kOzonePlatform) && cmd->GetSwitchValueASCII(switches::kOzonePlatform) == "gbm") {
		LOG(INFO) << __FUNCTION__ << "() => Configuring display";
		_displayConfigurator.reset(new DisplayConfigurator(this));
		_displayConfigurator->initialize();
	} else {
		createPlatform();
	}

	if (cmd->HasSwitch(switches::kRemoteDebuggingPort)) {
		initDevToolsServer(cmd->GetSwitchValueASCII(switches::kRemoteDebuggingPort));
	}

	_pluginServiceFilter.reset(new PluginServiceFilter(_delegate));
	content::PluginService::GetInstance()->SetFilter(_pluginServiceFilter.get());
}

bool BrowserMainParts::MainMessageLoopRun( int *result_code ) {
	LOG(INFO) << "Run main message loop!";
	util::postTask(TVD_IOT, base::Bind(&MainDelegate::setupProtocolHandlers, base::Unretained(_delegate)));
	util::postTask(TVD_FILET, base::Bind(&MainDelegate::loadLibraries, base::Unretained(_delegate)));
	return false;
}

void BrowserMainParts::PostMainMessageLoopRun() {
	_requestCtxGetter = nullptr;
	_devToolsHttpHandler.reset();
	_cursorDelegate.reset();
	_browserCtx.reset();
}

int BrowserMainParts::PreCreateThreads() {
	// Initialize the GpuDataManager before IO access restrictions are applied and before the IO thread is started.
	content::GpuDataManager::GetInstance();
	return 0;
}

void BrowserMainParts::initDevToolsServer( const std::string &port_str ) {
	uint16_t port = 0;
	int temp_port;
	if (base::StringToInt(port_str, &temp_port) && temp_port > 0 && temp_port < 65535) {
		port = static_cast<uint16_t>(temp_port);
		LOG(INFO) << "Devtools server listening on port: " << port;
		_devToolsHttpHandler.reset(
			new devtools_http_handler::DevToolsHttpHandler(
				scoped_ptr<TCPServerSocketFactory>(new TCPServerSocketFactory("0.0.0.0", port)),
				"", new DevToolsDelegate(), base::FilePath(), base::FilePath(), "", Client::GetShellUserAgent()));
	}
	else {
		LOG(WARNING) << "Invalid devtools http debugger port number: " << temp_port;
	}
}

void BrowserMainParts::showCursor() {
	_cursorDelegate->ShowCursor();
}

void BrowserMainParts::hideCursor() {
	_cursorDelegate->HideCursor();
}

void BrowserMainParts::moveCursor( const gfx::Point &location ) {
	_cursorDelegate->moveCursor(location);
}

void BrowserMainParts::createPlatform() {
	gfx::Size winSize = util::getWindowSize();
	LOG(INFO) << __FUNCTION__ << "() => winSize=" << winSize.ToString();
	if (!_platform) {
		_platform = new PlatformDataAura(gfx::Rect(winSize));
		_rootWin = _platform->GetDefaultParent(nullptr, nullptr, gfx::Rect());

		_cursorDelegate.reset(new CursorWindowDelegate(_rootWin));
		_delegate->browserClient()->setOriginDispatcher(std::move(ui::PlatformEventSource::GetInstance()->OverrideDispatcher(_delegate->browserClient())));
		if (!aura::client::GetEventClient(_rootWin)) {
			aura::client::SetEventClient(_rootWin, _delegate->browserClient());
		}
	}
}

}
