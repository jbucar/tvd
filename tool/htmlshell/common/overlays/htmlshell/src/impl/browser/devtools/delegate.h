#pragma once

#include "content/public/browser/devtools_agent_host.h"
#include "components/devtools_http_handler/devtools_http_handler_delegate.h"

namespace tvd {

class DevToolsDelegate : public devtools_http_handler::DevToolsHttpHandlerDelegate {
public:
	DevToolsDelegate() {}
	~DevToolsDelegate() override {}

	// devtools_http_handler::DevToolsHttpHandlerDelegate implementation.
	std::string GetDiscoveryPageHTML() override;
	std::string GetFrontendResource(const std::string& path) override;
	std::string GetPageThumbnailData(const GURL& url) override;
	content::DevToolsExternalAgentProxyDelegate *HandleWebSocketConnection(const std::string& path) override;

private:
	DISALLOW_COPY_AND_ASSIGN(DevToolsDelegate);
};

}
