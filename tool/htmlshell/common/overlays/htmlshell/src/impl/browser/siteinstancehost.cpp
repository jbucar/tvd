#include "siteinstancehost.h"
#include "browserclient.h"
#include "javascriptdialogmanager.h"
#include "../app/maindelegate.h"
#include "../common/client.h"
#include "../common/messages.h"
#include "../../apis/browser/browserapi.h"
#include "../../apis/web/webapi.h"
#include "../../util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string_number_conversions.h"
#include "base/json/json_string_value_serializer.h"
#include "base/message_loop/message_loop.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/render_frame_host.h"
#include "ui/aura/window.h"
#include "ui/gfx/native_widget_types.h"

namespace tvd {

SiteInstanceHost::SiteInstanceHost( BrowserClient *client, int id, content::WebContents *webContents, const tvd::browser::Options &opts )
    : WebContentsObserver(webContents),
      _id(id),
      _signaled(false),
      _isShowingCursor(false),
      _isFullscreen(false),
      _opts(opts),
      _client(client)
{
	TVD_REQUIRE_UIT();
	_webContents.reset(webContents);
	_webContents->SetDelegate(this);
	_webContents->SetUserAgentOverride(_opts.extraUA + " " + Client::GetShellUserAgent());
	_opts.visible = false;
	_opts.needFocus = false;
}

SiteInstanceHost::~SiteInstanceHost()
{
	if (_isShowingCursor) {
		_client->hideCursor();
	}
	_renderViewIds.clear();
	_renderFrameIds.clear();
}

int SiteInstanceHost::id() const {
	return _id;
}

int SiteInstanceHost::processId() const {
	return _webContents->GetSiteInstance()->GetProcess()->GetID();
}

void SiteInstanceHost::addAllowedPlugin( const std::string &name ) {
	_allowedPlugins.push_back(name);
}

bool SiteInstanceHost::isPluginAllowed( const std::string &name ) const {
	for (std::string pName : _allowedPlugins) {
		if (pName == name) {
			return true;
		}
	}
	return false;
}

int SiteInstanceHost::zIndex() const {
	return _webContents->GetNativeView()->id();
}

bool SiteInstanceHost::canShowCursor() {
	return _opts.visible && _opts.needFocus && _opts.enableMouse;
}

bool SiteInstanceHost::handleMouseEvent( const gfx::Point &location ) {
	gfx::Rect bounds = _webContents->GetNativeView()->bounds();
	bounds.set_width(bounds.width()+1);
	bounds.set_height(bounds.height()+1);

	if (canShowCursor() && bounds.Contains(location)) {
		if (!_isShowingCursor) {
			_isShowingCursor = true;
			_client->showCursor();
		}
		return true;
	}
	return false;
}

aura::Window *SiteInstanceHost::getWindow() {
	return _webContents->GetNativeView();
}

bool SiteInstanceHost::mouseEnabled() {
	return _opts.enableMouse;
}

void SiteInstanceHost::load( const std::string& urlstr ) {
	LOG(INFO) << "load url: " << urlstr;
	GURL url(urlstr);
	content::NavigationController::LoadURLParams params(url);
	params.transition_type = ui::PageTransitionFromInt(ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
	params.override_user_agent = content::NavigationController::UA_OVERRIDE_TRUE;
	_webContents->GetController().LoadURLWithParams(params);
}

void SiteInstanceHost::close() {
	LOG(INFO) << "close id=" << _id;
	if (_webContents->NeedToFireBeforeUnload()) {
		_webContents->DispatchBeforeUnload(false);
	} else {
		_webContents->ClosePage();
	}
}

bool SiteInstanceHost::AddMessageToConsole( content::WebContents * /*source*/, int32_t level, const base::string16 &message, int32_t line_no, const base::string16 &source_id ) {
	LOG(INFO) << base::UTF16ToUTF8(message) << " (source=" << base::UTF16ToUTF8(source_id) << ", line=" << line_no << ")";
	static_cast<BrowserApi*>(_client->delegate()->getShellApi(BROWSER_API))->onWebLog(_id, {level, line_no, base::UTF16ToUTF8(message), base::UTF16ToUTF8(source_id)});
	return true;
}

bool SiteInstanceHost::PreHandleKeyboardEvent( content::WebContents * /*source*/, const content::NativeWebKeyboardEvent &event, bool * /*isShortcut*/ ) {
	VLOG(1) << "PreHandleKeyboardEvent windowsKeyCode=" << event.windowsKeyCode;
	return static_cast<BrowserApi*>(_client->delegate()->getShellApi(BROWSER_API))->onKeyEvent(_id, event.windowsKeyCode, event.type == blink::WebKeyboardEvent::KeyUp);
}

bool SiteInstanceHost::OnMessageReceived( const IPC::Message &message ) {
	bool handled = true;
	IPC_BEGIN_MESSAGE_MAP(SiteInstanceHost, message)
		IPC_MESSAGE_HANDLER(Tvd_ExtQuery, onExtQuery)
		IPC_MESSAGE_UNHANDLED(handled = false)
	IPC_END_MESSAGE_MAP()
	return handled;
}

void SiteInstanceHost::RenderViewCreated( content::RenderViewHost *render_view_host ) {
	if (_opts.transparentBg) {
		render_view_host->GetWidget()->GetView()->SetBackgroundColor(SK_ColorTRANSPARENT);
	} else {
		render_view_host->GetWidget()->GetView()->SetBackgroundColorToDefault();
	}
}

void SiteInstanceHost::RenderProcessGone( base::TerminationStatus status ) {
	LOG(INFO) << "Render process terminated with status=" << status;
	std::string errorMsg = "";
	switch (status) {
		case base::TERMINATION_STATUS_NORMAL_TERMINATION: break; // zero exit status
		case base::TERMINATION_STATUS_ABNORMAL_TERMINATION: // non-zero exit status
			errorMsg = "Renderer process terminated abnormaly";
			break;
		case base::TERMINATION_STATUS_PROCESS_WAS_KILLED: // e.g. SIGKILL or task manager kill
			errorMsg = "Renderer process was killed";
			break;
		case base::TERMINATION_STATUS_PROCESS_CRASHED: // e.g. Segmentation fault
			errorMsg = "Renderer process exit with segmentation fault";
			break;
		default:
			errorMsg = "Renderer process exit with unknown reason";
	}
	_client->onSiteClosed(_id, errorMsg);
}

void SiteInstanceHost::DidFailProvisionalLoad( content::RenderFrameHost *render_frame_host, const GURL &validated_url, int error_code, const base::string16 &error_description, bool was_ignored_by_handler ) {
	// If main frame fail to load close browser with error
	if (!render_frame_host->GetParent()) {
		LOG(WARNING) << "Provisional load fail! url=" << validated_url << ", error=" << error_code << ": " << error_description;
		util::postTask(TVD_UIT, base::Bind(base::IgnoreResult(&BrowserClient::onSiteClosed), base::Unretained(_client), _id, "Fail to load url=" + validated_url.spec()));
	}
}

void SiteInstanceHost::onExtQuery( int queryId, const std::string &msg, const base::ListValue &params ) {
	std::string paramsStr;
	JSONStringValueSerializer jsonSerializer(&paramsStr);
	const base::Value *p = NULL;
	if (params.Get(0, &p) && jsonSerializer.Serialize(*p)) {
		VLOG(1) << "Extension query received! SiteID=" << _id << ", QuerID=" << queryId << ", msg=" << msg;
	} else {
		LOG(WARNING) << "Fail to get params for extension query: " << msg;
	}

	if (!TVD_CURRENTLY_ON_UIT()) {
		// Post task to UI thread
		util::postTask(TVD_UIT, base::Bind(&SiteInstanceHost::processQuery, base::Unretained(this), queryId, msg, paramsStr));
	} else {
		processQuery(queryId, msg, paramsStr);
	}
}

void SiteInstanceHost::processQuery( int queryId, const std::string &msg, const std::string &params ) {
	TVD_REQUIRE_UIT();
	if (msg == "htmlshell" && params == "\"async_exit_complete\"") {
		LOG(INFO) << "Async exit completed! _id=" << _id;
		_webContents->ClosePage();
	}
	else if (!static_cast<WebApi*>(_client->delegate()->getShellApi("webapi"))->onMessageReceived({_id, queryId, msg, params})) {
		LOG(WARNING) << "Extension query not handled! msg=" << msg;
	}
}

void SiteInstanceHost::executeCallback( int queryId, const std::string &error, const std::string &params, bool isSignal ) {
	TVD_REQUIRE_UIT();
	scoped_ptr<base::Value> paramsValue;
	base::ListValue resp;

	if (!params.empty()) {

		int errorCode;
		std::string errorMsg;
		JSONStringValueDeserializer jsonSerializer(params);
		paramsValue = jsonSerializer.Deserialize(&errorCode, &errorMsg);

		base::ListValue *respPtr = nullptr;
		if (paramsValue.get() && paramsValue->GetAsList(&respPtr)) {
			resp.Swap(respPtr);
		} else {
			LOG(WARNING) << "Invalid params: " << params << ", errorMsg=" << errorMsg;
		}
	}
	Send(new Tvd_ExtQueryResponse(routing_id(), _id, queryId, error, resp, isSignal));
}

bool SiteInstanceHost::viewIdMatch( int pid, int routingId ) const {
	return idMatch(&_renderViewIds, pid, routingId);
}

bool SiteInstanceHost::frameIdMatch( int pid, int routingId ) const {
	return idMatch(&_renderFrameIds, pid, routingId);
}

void SiteInstanceHost::addViewId( int pid, int routingId ) {
	addId(&_renderViewIds, pid, routingId);
}

void SiteInstanceHost::addFrameId( int pid, int routingId ) {
	addId(&_renderFrameIds, pid, routingId);
}

void SiteInstanceHost::addId( RenderIdSet *set, int pid, int routingId ) {
	CHECK(pid > 0);
	CHECK(routingId > 0);

	if (!set->empty()) {
		RenderIdSet::const_iterator it = set->find(std::make_pair(pid, routingId));
		if (it != set->end()) {
			return;
		}
	}
	set->insert(std::make_pair(pid, routingId));
}

bool SiteInstanceHost::idMatch( const RenderIdSet *set, int pid, int routingId ) const {
	if (set->empty()) {
		return false;
	}
	RenderIdSet::const_iterator it = set->find(std::make_pair(pid, routingId));
	return (it != set->end());
}

const std::string &SiteInstanceHost::initScript() const {
	return _opts.initJS;
}

void SiteInstanceHost::signalLaunched() {
	if (!_signaled) {
		static_cast<BrowserApi*>(_client->delegate()->getShellApi(BROWSER_API))->browserLaunched(_id);
		_signaled = true;
	}
}

void SiteInstanceHost::show( bool needShow, bool keyboardFocus ) {
	TVD_REQUIRE_UIT();
	bool needUpdateSites = false;
	if (needShow != _opts.visible) {
		_opts.visible = needShow;
		needUpdateSites = true;
	}
	if (keyboardFocus != _opts.needFocus) {
		_opts.needFocus = keyboardFocus;
		needUpdateSites = true;
	}
	if (needUpdateSites) {
		_client->updateSites();
	}
}

void SiteInstanceHost::update() {
	TVD_REQUIRE_UIT();
	if (_opts.visible) {
		_webContents->GetNativeView()->Show();
		if (_opts.needFocus) {
			_webContents->Focus();
		}
	} else {
		_webContents->GetNativeView()->Hide();
	}
}

void SiteInstanceHost::DocumentOnLoadCompletedInMainFrame() {
	LOG(INFO) << "Site complete loading, id=" << _id << ", rph_id=" << processId();
	static_cast<BrowserApi*>(_client->delegate()->getShellApi(BROWSER_API))->browserLoaded(_id);
	if (canShowCursor()) {
		_isShowingCursor = true;
		_client->showCursor();
	}
}

void SiteInstanceHost::EnterFullscreenModeForTab( content::WebContents *web_contents, const GURL &origin )  {
	ToggleFullscreenModeForTab(web_contents, true);
}

void SiteInstanceHost::ExitFullscreenModeForTab( content::WebContents *web_contents ) {
	ToggleFullscreenModeForTab(web_contents, false);
}

void SiteInstanceHost::ToggleFullscreenModeForTab( content::WebContents *web_contents, bool enter_fullscreen ) {
	if (_isFullscreen != enter_fullscreen) {
		_isFullscreen = enter_fullscreen;
		web_contents->GetRenderViewHost()->GetWidget()->WasResized();
	}
}

bool SiteInstanceHost::IsFullscreenForTabOrPending( const content::WebContents *web_contents ) const {
	return _isFullscreen;
}

content::WebContents *SiteInstanceHost::webContents() {
	return _webContents.get();
}

bool SiteInstanceHost::hasPermission( content::PermissionType permission ) {
	if (permission == content::PermissionType::GEOLOCATION) {
		return _opts.enableGeolocation;
	}
	return false;
}

void SiteInstanceHost::BeforeUnloadFired(content::WebContents *tab, bool proceed, bool *proceed_to_fire_unload ) {
	LOG(INFO) << "BeforeUnloadFired() id=" << _id << ", proceed=" << proceed;
	*proceed_to_fire_unload = proceed;
}

content::JavaScriptDialogManager *SiteInstanceHost::GetJavaScriptDialogManager( content::WebContents *source ) {
	LOG(INFO) << "GetJavaScriptDialogManager";
	if (!_dialogManager) {
		_dialogManager.reset(new JavaScriptDialogManager());
	}
	return _dialogManager.get();
}

void SiteInstanceHost::CloseContents( content::WebContents *source ) {
	LOG(INFO) << "CloseContents";
	_client->onSiteClosed(_id, "");
}

}
