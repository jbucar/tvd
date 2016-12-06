#include "../apis/web.h"
#include "mocks.h"
#include <map>
#include <string>
#include <vector>
#include <assert.h>

namespace tvd {
namespace web {
namespace test {

namespace impl {

class WebApiMock : public web::ApiInterface_1_0 {
public:
	WebApiMock() {
		onExecuteCallback = NULL;
	}

	virtual ~WebApiMock() {}

	virtual bool addJsApi( const std::string &name, web::RequestHandler handler ) {
		if (!name.empty() && handler) {
			std::pair<JsApiMap::iterator, bool> ret = jsApis.insert(std::make_pair(name,handler));
			return ret.second;
		}
		return false;
	}

	virtual bool removeJsApi( const std::string &name ) {
		return jsApis.erase(name) > 0;
	}

	virtual bool executeCallback( const web::Response &response ) {
		if (onExecuteCallback != NULL) {
			return onExecuteCallback(response);
		}
		return false;
	}

	typedef std::map<std::string,web::RequestHandler> JsApiMap;
	JsApiMap jsApis;
	ExecuteCallback onExecuteCallback;
};

static WebApiMock *mockIface = NULL;

}

void *getMockInterface() {
	if (!impl::mockIface) {
		impl::mockIface = new impl::WebApiMock();
	}
	return impl::mockIface;
}

void destroyMockInterface() {
	if (impl::mockIface) {
		delete impl::mockIface;
		impl::mockIface = NULL;
	}
}

void onExecuteCallback( ExecuteCallback cb ) {
	assert(impl::mockIface);
	impl::mockIface->onExecuteCallback = cb;
}

bool simulateApiCall( const std::string &api, const web::Request &req ) {
	assert(impl::mockIface);
	impl::WebApiMock::JsApiMap::iterator it = impl::mockIface->jsApis.find(api);
	if (it != impl::mockIface->jsApis.end()) {
		return (it->second)(req);
	}
	return false;
}

}
}
}
