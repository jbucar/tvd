#pragma once

#include "dtv-htmlshellsdk/src/apis/browser.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/memory/scoped_ptr.h"

namespace base {
class MessageLoop;
}

namespace content {
class BrowserThreadImpl;
}

namespace tvd {
class BrowserApi;
class MainDelegate;
}

namespace test {
class BrowserClientMock;
}

class BrowserApiTest : public testing::Test {
public:
	BrowserApiTest();
	virtual ~BrowserApiTest();

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::MainDelegate *_delegate;
	tvd::BrowserApi *_api;
	test::BrowserClientMock *_browserClient;
	scoped_ptr<base::MessageLoop> _mainMessageLoop;
	scoped_ptr<content::BrowserThreadImpl> _mainThread;
};

class BrowserApiInterface_1_0_Test : public BrowserApiTest {
public:
	BrowserApiInterface_1_0_Test();

	// Browser listener
	void onLaunched( int id );
	void onClosed( int id, const std::string &error );
	bool onKey(int, int, bool) { return false; }
	void onWebLog(int, const tvd::browser::WebLog&) {}
	void onLoadProgress(int);

protected:
	void SetUp() override;
	void TearDown() override;

	std::vector<int> _launchedBrowsers;
	std::vector<int> _closedBrowsers;
	std::set<int> _loadedBrowsers;
	tvd::browser::ApiInterface_1_0 *iface;
	tvd::browser::Listener *listener;
};
