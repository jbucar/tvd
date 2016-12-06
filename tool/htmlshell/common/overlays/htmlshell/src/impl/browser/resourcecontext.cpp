#include "resourcecontext.h"
#include "urlrequestcontextgetter.h"

namespace tvd {

ResourceContext::ResourceContext()
	: _getter(nullptr)
{}

ResourceContext::~ResourceContext()
{}

net::HostResolver *ResourceContext::GetHostResolver() {
	LOG(INFO) << "Getting host resolver";
	CHECK(_getter);
	return _getter->hostResolver();
}

net::URLRequestContext *ResourceContext::GetRequestContext() {
	LOG(INFO) << "Getting request context";
	CHECK(_getter);
	return _getter->GetURLRequestContext();
}

void ResourceContext::setUrlRequestContextGetter( URLRequestContextGetter *getter ) {
	_getter = getter;
}

}
