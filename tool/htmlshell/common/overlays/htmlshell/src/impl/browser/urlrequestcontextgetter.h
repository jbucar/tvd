#pragma once

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_job_factory.h"

namespace base {
class MessageLoop;
}

namespace net {
class HostResolver;
class MappedHostResolver;
class NetworkDelegate;
class NetLog;
class ProxyConfigService;
class URLRequestContextStorage;
class URLRequestJobFactoryImpl;
}

namespace tvd {

class URLRequestContextGetter : public net::URLRequestContextGetter {
public:
	URLRequestContextGetter( const base::FilePath& base_path,
                             base::MessageLoop* io_loop,
                             base::MessageLoop* file_loop,
                             content::ProtocolHandlerMap* protocol_handlers,
                             content::URLRequestInterceptorScopedVector request_interceptors,
                             net::NetLog* net_log );

	net::URLRequestContext *GetURLRequestContext() override;
	scoped_refptr<base::SingleThreadTaskRunner> GetNetworkTaskRunner() const override;

	net::HostResolver *hostResolver();
	net::URLRequestJobFactoryImpl *jobFactory() const;

protected:
	~URLRequestContextGetter() override;

private:
	base::FilePath base_path_;
	base::MessageLoop* io_loop_;
	base::MessageLoop* file_loop_;
	net::NetLog* net_log_;

	net::URLRequestJobFactoryImpl *_jobFactory;
	scoped_ptr<net::ProxyConfigService> _proxyConfigService;
	scoped_ptr<net::URLRequestContextStorage> _storage;
	scoped_ptr<net::URLRequestContext> _urlRequestContext;
	content::ProtocolHandlerMap _protocolHandlers;
	content::URLRequestInterceptorScopedVector _requestInterceptors;

	DISALLOW_COPY_AND_ASSIGN(URLRequestContextGetter);
};

}
