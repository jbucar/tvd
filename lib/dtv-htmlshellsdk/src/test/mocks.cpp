#include "mocks.h"
#include "../apis/browser.h"
#include "../apis/fs.h"
#include "../apis/system.h"
#include "../apis/web.h"
#include <vector>

namespace tvd {

namespace fs {
namespace test {
	void *getMockInterface();
	void destroyMockInterface();
}
}

namespace browser {
namespace test {
	void *getMockInterface();
	void destroyMockInterface();
}
}

namespace web {
namespace test {
	void *getMockInterface();
	void destroyMockInterface();
}
}

namespace system {
namespace test {
	void *getMockInterface();
	void destroyMockInterface();
}
}

namespace test {

HtmlShellWrapperImpl *g_wrap = NULL;

bool HtmlShellWrapperImpl::hasApi( const std::string &name, uint32_t major, uint32_t minor ) {
	return getApiImpl(name, major, minor) != NULL;
}

void *HtmlShellWrapperImpl::getApiImpl( const std::string &name, uint32_t major, uint32_t minor ) {
	std::vector<MockApi>::const_iterator it = _apis.begin();
	while (it != _apis.end()) {
		if (it->_name == name && it->_major == major && it->_minor == minor) {
			return it->_mock;
		}
		it++;
	}
	return NULL;
}

void HtmlShellWrapperImpl::clearApis() {
	_apis.clear();
}

void HtmlShellWrapperImpl::addApi( const MockApi &api ) {
	_apis.push_back(api);
}

tvd::HtmlShellWrapper *getHtmlShellWrapperMock() {
	if (!g_wrap) {
		g_wrap = new HtmlShellWrapperImpl();
	}
	return g_wrap;
}

void createMockApis() {
	g_wrap->clearApis();
	g_wrap->addApi(MockApi(BROWSER_API, 1, 0, browser::test::getMockInterface()));
	g_wrap->addApi(MockApi(FS_API, 1, 0, fs::test::getMockInterface()));
	g_wrap->addApi(MockApi(SYSTEM_API, 1, 0, system::test::getMockInterface()));
	g_wrap->addApi(MockApi(WEB_API, 1, 0, web::test::getMockInterface()));
}

void destroyHtmlShellWrapperMock() {
	if (g_wrap) {
		g_wrap->clearApis();
		browser::test::destroyMockInterface();
		fs::test::destroyMockInterface();
		system::test::destroyMockInterface();
		web::test::destroyMockInterface();

		delete g_wrap;
		g_wrap = NULL;
	}
}

}
}
