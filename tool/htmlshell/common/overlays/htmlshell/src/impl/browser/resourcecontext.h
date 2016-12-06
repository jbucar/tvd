#pragma once

#include "content/public/browser/resource_context.h"

namespace tvd {

class URLRequestContextGetter;

class ResourceContext : public content::ResourceContext {
public:
	ResourceContext();
	~ResourceContext() override;

	void setUrlRequestContextGetter( URLRequestContextGetter *getter );

	// content::ResourceContext implementation:
	net::HostResolver *GetHostResolver() override;
	net::URLRequestContext *GetRequestContext() override;

private:
	URLRequestContextGetter* _getter;

	DISALLOW_COPY_AND_ASSIGN(ResourceContext);
};

}
