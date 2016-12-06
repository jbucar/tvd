#include "delegate.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "ui/base/resource/resource_bundle.h"
#include "grit/htmlshell_resources.h"

namespace tvd {

std::string DevToolsDelegate::GetDiscoveryPageHTML() {
	return ResourceBundle::GetSharedInstance().GetRawDataResource(IDR_HTMLSHELL_DEVTOOLS_DISCOVERY_PAGE).as_string();
}

std::string DevToolsDelegate::GetFrontendResource( const std::string &path ) {
	return content::DevToolsFrontendHost::GetFrontendResource(path).as_string();
}

std::string DevToolsDelegate::GetPageThumbnailData( const GURL &url ) {
	return std::string();
}

content::DevToolsExternalAgentProxyDelegate *DevToolsDelegate::HandleWebSocketConnection( const std::string &path ) {
	return nullptr;
}

}
