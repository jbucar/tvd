#include "webapi.h"
#include "../browser/browserapi.h"
#include "../../impl/browser/browserclient.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "base/logging.h"
#include "base/strings/string_split.h"
#include <algorithm>

namespace tvd {

namespace impl {

class WebApi_1_0 : web::ApiInterface_1_0 {
public:
	explicit WebApi_1_0( WebApi *api ) : _api(api) {}
	virtual ~WebApi_1_0() {}

	virtual bool addJsApi( const std::string &name, web::RequestHandler handler ) override {
		return _api->addJsApi(name, handler);
	}

	virtual bool removeJsApi( const std::string &name ) override {
		return _api->removeJsApi(name);
	}

	virtual bool executeCallback( const web::Response &response ) override {
		if (!_api->executeCallback(response)) {
			LOG(WARNING) << "Fail to execute callback for queryId=" << response.queryId << ", browserId=" << response.browserId;
			return false;
		}
		return true;
	}

private:
	WebApi *_api;
};

}

WebApi::WebApi( BrowserClient *client )
	: ShellApi(WEB_API)
{
	_client = client;
}

WebApi::~WebApi()
{
	_client = NULL;
}

bool WebApi::init() {
	_iface = new impl::WebApi_1_0(this);
	return true;
}

void WebApi::fin() {
	if (_iface) {
		delete _iface;
		_iface = NULL;
	}
	_jsApis.clear();
}

bool WebApi::registerInterfaces() {
	if (!registerInterface(MAKE_API_VERSION(1u, 0u), _iface)) {
		return false;
	}
	return true;
}

uint32_t WebApi::getLastVersion() const {
	return MAKE_API_VERSION(WEB_API_VERSION_MAJOR, WEB_API_VERSION_MINOR);
}

bool WebApi::hasJsApi( const std::string &name ) const {
	return _jsApis.count(name) > 0;
}

bool WebApi::addJsApi( const std::string &name, web::RequestHandler handler ) {
	if (handler) {
		std::string apiName(name.size(), ' ');
		std::transform(name.begin(), name.end(), apiName.begin(), ::tolower);
		if (_jsApis.insert(std::make_pair(apiName, handler)).second) {
			LOG(INFO) << "Added javascript api: " << name;
			return true;
		}
	}

	LOG(WARNING) << "Fail to add JavaScript api: " << name;
	return false;
}

bool WebApi::removeJsApi( const std::string &name ) {
	std::string apiName(name.size(), ' ');
	std::transform(name.begin(), name.end(), apiName.begin(), ::tolower);

	std::map<std::string, web::RequestHandler>::iterator it = _jsApis.find(apiName);
	if (it != _jsApis.end()) {
		_jsApis.erase(it);
		return true;
	}
	return false;
}

bool WebApi::onMessageReceived( const web::Request &request ) {
	LOG(INFO) << "Message received: " << request.name;
	std::string apiName = request.name;
	std::transform(request.name.begin(), request.name.end(), apiName.begin(), ::tolower);
	std::map<std::string, web::RequestHandler>::const_iterator it =_jsApis.find(apiName);
	if (it != _jsApis.end()) {
		return it->second(request);
	}
	LOG(WARNING) << "JavaScript api not found: " << apiName;
	return false;
}

bool WebApi::executeCallback( const web::Response &response ) {
	return _client->executeCallback(response.browserId, response.queryId, response.error, response.params, response.isSignal);

}

}
