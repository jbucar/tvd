#include "mocks.h"
#include "../apis/system.h"
#include <vector>
#include <map>

namespace tvd {
namespace system {
namespace test {

namespace impl {

class SystemApiMock : public system::ApiInterface_1_0 {
public:
	virtual ~SystemApiMock() {}

	virtual std::string getCmdSwitch( const std::string &name ) {
		std::map<std::string,std::string>::const_iterator it = switches.find(name);
		if (it != switches.end()) {
			return it->second;
		} else {
			return "";
		}
	}

	virtual void shutdown( unsigned exitCode ) {
		std::vector<OnShutdown>::const_iterator it = shutdownCallbacks.begin();
		for ( ; it != shutdownCallbacks.end(); it++ ) {
			(*it)(HTMLSHELL_EXTERNAL_CODE_BASE + exitCode);
		}
	}

	std::vector<OnShutdown> shutdownCallbacks;
	std::map<std::string,std::string> switches;
};

static SystemApiMock *mockIface = NULL;

}

void *getMockInterface() {
	if (!impl::mockIface) {
		impl::mockIface = new impl::SystemApiMock();
	}
	return impl::mockIface;
}

void destroyMockInterface() {
	if (impl::mockIface) {
		delete impl::mockIface;
		impl::mockIface = NULL;
	}
}

void onShutdown( OnShutdown cb ) {
	impl::mockIface->shutdownCallbacks.push_back(cb);
}

void setCmdSwitch( const std::string &name, const std::string &value ) {
	impl::mockIface->switches.insert(std::make_pair(name,value));
}

}
}
}
