#include "urlrequestcontextgetter.h"
#include "networkdelegate.h"
#include "../common/client.h"
#include "../../util.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/threading/sequenced_worker_pool.h"
#include "base/threading/worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/cookie_store_factory.h"
#include "content/public/common/content_switches.h"
#include "net/base/cache_type.h"
#include "net/cert/cert_verifier.h"
#include "net/cookies/cookie_monster.h"
#include "net/dns/host_resolver.h"
#include "net/dns/mapped_host_resolver.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_server_properties_impl.h"
#include "net/http/transport_security_state.h"
#include "net/proxy/proxy_service.h"
#include "net/ssl/channel_id_service.h"
#include "net/ssl/default_channel_id_store.h"
#include "net/ssl/ssl_config_service_defaults.h"
#include "net/url_request/data_protocol_handler.h"
#include "net/url_request/file_protocol_handler.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_intercepting_job_factory.h"
#include "net/url_request/url_request_job_factory_impl.h"
#include "url/url_constants.h"

#if defined(USE_NSS_CERTS)
#	include "net/cert_net/nss_ocsp.h"
#endif

namespace tvd {

URLRequestContextGetter::URLRequestContextGetter( const base::FilePath &base_path,
                                                  base::MessageLoop* io_loop,
                                                  base::MessageLoop* file_loop,
                                                  content::ProtocolHandlerMap* protocol_handlers,
                                                  content::URLRequestInterceptorScopedVector request_interceptors,
                                                  net::NetLog *net_log )
    : base_path_(base_path),
      io_loop_(io_loop),
      file_loop_(file_loop),
      net_log_(net_log),
      _requestInterceptors(std::move(request_interceptors))
{
	TVD_REQUIRE_UIT();
	std::swap(_protocolHandlers, *protocol_handlers);
	_proxyConfigService = net::ProxyService::CreateSystemProxyConfigService(io_loop_->task_runner(), file_loop_->task_runner());
}

URLRequestContextGetter::~URLRequestContextGetter() {
}

net::URLRequestContext *URLRequestContextGetter::GetURLRequestContext() {
	TVD_REQUIRE_IOT();

	if (!_urlRequestContext.get()) {
		_urlRequestContext.reset(new net::URLRequestContext());
		_urlRequestContext->set_net_log(net_log_);

		_storage.reset(new net::URLRequestContextStorage(_urlRequestContext.get()));
		_storage->set_cookie_store(CreateCookieStore(content::CookieStoreConfig()));
		_storage->set_network_delegate(make_scoped_ptr(new NetworkDelegate()));
		_storage->set_channel_id_service(make_scoped_ptr(new net::ChannelIDService(new net::DefaultChannelIDStore(nullptr), base::WorkerPool::GetTaskRunner(true))));
		_storage->set_http_user_agent_settings(make_scoped_ptr(new net::StaticHttpUserAgentSettings("en-us,en", Client::GetShellUserAgent())));

		scoped_ptr<net::HostResolver> host_resolver(net::HostResolver::CreateDefaultResolver(_urlRequestContext->net_log()));

		_storage->set_cert_verifier(net::CertVerifier::CreateDefault());
		_storage->set_transport_security_state(make_scoped_ptr(new net::TransportSecurityState));

		// TODO(jam): use v8 if possible, look at chrome code.
		_storage->set_proxy_service(net::ProxyService::CreateUsingSystemProxyResolver(std::move(_proxyConfigService), 0, _urlRequestContext->net_log()));

		_storage->set_ssl_config_service(new net::SSLConfigServiceDefaults);
		_storage->set_http_auth_handler_factory(net::HttpAuthHandlerFactory::CreateDefault(host_resolver.get()));
		_storage->set_http_server_properties(scoped_ptr<net::HttpServerProperties>(new net::HttpServerPropertiesImpl()));

		base::FilePath cache_path = base_path_.Append(FILE_PATH_LITERAL("Cache"));
		scoped_ptr<net::HttpCache::DefaultBackend> main_backend(new net::HttpCache::DefaultBackend(
			net::DISK_CACHE, net::CACHE_BACKEND_DEFAULT, cache_path, 0,
			content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::CACHE).get()));

		net::HttpNetworkSession::Params network_session_params;
		network_session_params.cert_verifier = _urlRequestContext->cert_verifier();
		network_session_params.transport_security_state = _urlRequestContext->transport_security_state();
		network_session_params.channel_id_service = _urlRequestContext->channel_id_service();
		network_session_params.proxy_service = _urlRequestContext->proxy_service();
		network_session_params.ssl_config_service = _urlRequestContext->ssl_config_service();
		network_session_params.http_auth_handler_factory = _urlRequestContext->http_auth_handler_factory();
		network_session_params.network_delegate = _urlRequestContext->network_delegate();
		network_session_params.http_server_properties = _urlRequestContext->http_server_properties();
		network_session_params.net_log = _urlRequestContext->net_log();
		network_session_params.ignore_certificate_errors = false;

		// Give |_storage| ownership at the end in case it's |mapped_host_resolver|.
		_storage->set_host_resolver(std::move(host_resolver));
		network_session_params.host_resolver = _urlRequestContext->host_resolver();

		_storage->set_http_network_session(make_scoped_ptr(new net::HttpNetworkSession(network_session_params)));
		_storage->set_http_transaction_factory(make_scoped_ptr(new net::HttpCache(_storage->http_network_session(), std::move(main_backend), true)));

		scoped_ptr<net::URLRequestJobFactoryImpl> job_factory(new net::URLRequestJobFactoryImpl());
		_jobFactory = job_factory.get();

		// Keep ProtocolHandlers added in sync with BrowserClient::IsHandledURL().
	    content::ProtocolHandlerMap::iterator it(_protocolHandlers.begin());
		for (; it != _protocolHandlers.end(); ++it) {
			if (!job_factory->SetProtocolHandler(it->first, make_scoped_ptr(it->second.release()))) {
				LOG(ERROR) << "Fail to set protocol handler for scheme: " << it->first;
			}
		}
		_protocolHandlers.clear();
	    job_factory->SetProtocolHandler(url::kDataScheme, make_scoped_ptr(new net::DataProtocolHandler()));
	    job_factory->SetProtocolHandler(url::kFileScheme, make_scoped_ptr(new net::FileProtocolHandler(content::BrowserThread::GetBlockingPool()->GetTaskRunnerWithShutdownBehavior(base::SequencedWorkerPool::SKIP_ON_SHUTDOWN))));

	    // Set up interceptors in the reverse order.
	    scoped_ptr<net::URLRequestJobFactory> top_job_factory = std::move(job_factory);
		content::URLRequestInterceptorScopedVector::reverse_iterator i(_requestInterceptors.rbegin());
		for (;i != _requestInterceptors.rend(); ++i) {
			top_job_factory.reset(new net::URLRequestInterceptingJobFactory(std::move(top_job_factory), make_scoped_ptr(*i)));
		}
		_requestInterceptors.weak_clear();

		_storage->set_job_factory(std::move(top_job_factory));

#if defined(USE_NSS_CERTS)
		// Only do this for the first (global) request context.
		static bool request_context_for_nss_set = false;
		if (!request_context_for_nss_set) {
			net::SetURLRequestContextForNSSHttpIO(_urlRequestContext.get());
			request_context_for_nss_set = true;
		}
#endif
	}

	return _urlRequestContext.get();
}

scoped_refptr<base::SingleThreadTaskRunner> URLRequestContextGetter::GetNetworkTaskRunner() const {
	return content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::IO);
}

net::HostResolver *URLRequestContextGetter::hostResolver() {
	return _urlRequestContext->host_resolver();
}

net::URLRequestJobFactoryImpl *URLRequestContextGetter::jobFactory() const {
	return _jobFactory;
}

}
