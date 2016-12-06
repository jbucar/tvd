#include "browserapi.h"
#include "../../htmlshell.h"
#include "../../util.h"
#include "../../../src/util.h"
#include "../../../src/apis/browser/browserapi.h"
#include "../../../src/impl/browser/browserclient.h"
#include "base/logging.h"
#include "content/browser/browser_thread_impl.h"

namespace test {

tvd::BrowserApi *api=nullptr;
static std::string jsApis = "";

class BrowserClientMock : public tvd::BrowserClient {
public:
	using tvd::BrowserClient::BrowserClient;

	void closeAllSites() override {
		_sites.clear();
	}

	void onSiteClosed( int id, const std::string &error ) override {
		if (_sites.erase(id) == 1) {
			notifyBrowserClosed(id, error);
		}
	}

	bool closeSite( int id ) override {
		if (_sites.erase(id) == 1) {
			notifyBrowserClosed(id, "");
			return true;
		}
		return false;
	}

	bool showSite( int id, bool show, bool focus ) override {
		auto it = _sites.find(id);
		if (it != _sites.end()) {
			it->second.visible = show;
			it->second.focus = focus;
			return true;
		}
		return false;
	}

	void notifyBrowserClosed( int id, const std::string &error ) override {
		if (api) {
			api->browserClosed(id, error);
		}
	}

	int siteCount() {
		return _sites.size();
	}

	bool isSiteVisible( int id ) {
		auto it = _sites.find(id);
		return it != _sites.end() && it->second.visible;
	}

	bool siteHasFocus( int id ) {
		auto it = _sites.find(id);
		return it != _sites.end() && it->second.focus;
	}

protected:
	struct SiteS {
		bool visible;
		bool focus;
	};
	typedef struct SiteS Site;

	virtual void createSiteInstance( int id, const std::string &url, const tvd::browser::Options &opts ) {
		Site s = {true,true};
		_sites.insert(std::make_pair(id,s));
		jsApis = opts.initJS;
		if (api) {
			api->browserLaunched(id);
		}
	}

private:
	std::map<int, Site> _sites;
};

}

BrowserApiTest::BrowserApiTest()
{
	_api = nullptr;
	_browserClient = nullptr;
}

BrowserApiTest::~BrowserApiTest()
{
	CHECK(_api == nullptr);
	CHECK(_browserClient == nullptr);
}

void BrowserApiTest::SetUp() {
	_delegate = new test::FakeMainDelegate();
	_mainMessageLoop.reset(new base::MessageLoopForUI);
	_mainThread.reset(new content::BrowserThreadImpl(TVD_UIT, base::MessageLoop::current()));
	_browserClient = new test::BrowserClientMock(_delegate);
	_api = new tvd::BrowserApi(_browserClient);
	ASSERT_TRUE( _api != nullptr );
	test::api = _api;
}

void BrowserApiTest::TearDown() {
	_api->finalize();
	delete _api;
	test::api = _api = nullptr;

	delete _browserClient;
	_browserClient = nullptr;

	delete _delegate;
	_delegate = nullptr;
}

BrowserApiInterface_1_0_Test::BrowserApiInterface_1_0_Test()
{
	iface = nullptr;
	listener = nullptr;
}

void BrowserApiInterface_1_0_Test::SetUp() {
	BrowserApiTest::SetUp();
	ASSERT_TRUE(_api->initialize());
	iface = static_cast<tvd::browser::ApiInterface_1_0*>(_api->getInterface(1,0));
	ASSERT_TRUE( iface != nullptr );

	listener = new tvd::browser::ListenerForwarder<BrowserApiInterface_1_0_Test>(this);
	iface->addListener(listener);
}

void BrowserApiInterface_1_0_Test::TearDown() {
	iface->rmListener(listener);
	delete listener;
	listener = nullptr;

	iface = nullptr;
	BrowserApiTest::TearDown();
}

void BrowserApiInterface_1_0_Test::onLaunched( int id ) {
	_launchedBrowsers.push_back(id);
}

void BrowserApiInterface_1_0_Test::onClosed( int id, const std::string &error ) {
	_closedBrowsers.push_back(id);
}

void BrowserApiInterface_1_0_Test::onLoadProgress( int id ) {
	_loadedBrowsers.insert(id);
}

TEST_F( BrowserApiTest, constructed_successfully ) {
	ASSERT_STREQ( "browserapi", _api->name().c_str() );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, initialized_successfully ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, get_interface_version_1_0 ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_TRUE( _api->hasInterface(1,0) );
	ASSERT_TRUE( _api->getInterface(1,0) != nullptr );
}

TEST_F( BrowserApiTest, get_default_interface ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_TRUE( _api->hasInterface(0,0) );
	ASSERT_TRUE( _api->getInterface(0,0) );
}

TEST_F( BrowserApiTest, api_default_interface_is_last_interface ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( _api->getInterface(0,0), _api->getInterface(BROWSER_API_VERSION_MAJOR, BROWSER_API_VERSION_MINOR) );
}

TEST_F( BrowserApiTest, launch_and_close_browser ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com.ar", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( _api->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, close_inexistent_browser ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_FALSE( _api->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, launch_and_close_browser_twice ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com.ar", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( _api->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_FALSE( _api->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, launch_and_close_two_browsers ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com.ar", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	base::FilePath html = util::getTestResourceDir("apps").Append("index.html");
	ASSERT_TRUE( _api->launchBrowser(2, "file://" + html.value(), tvd::browser::Options()) );
	ASSERT_EQ( 2, _browserClient->siteCount() );

	ASSERT_TRUE( _api->closeBrowser(1) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
	ASSERT_FALSE( _api->closeBrowser(1) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( _api->closeBrowser(2) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
	ASSERT_FALSE( _api->closeBrowser(2) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, finalize_api_without_closing_all_browsers ) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com.ar", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	_api->finalize();
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiTest, javascript_code_is_sended_correctly ) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	tvd::browser::Options opts;
	opts.initJS = "alert(\"Hello, World!!!\")";
	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com.ar", opts) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_STREQ( "alert(\"Hello, World!!!\")", test::jsApis.c_str() );

	ASSERT_TRUE( _api->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
	_api->finalize();
}

TEST_F( BrowserApiInterface_1_0_Test, launch_browser ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
}

TEST_F( BrowserApiInterface_1_0_Test, browser_load_progress ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( _loadedBrowsers.empty() );
	_api->browserLoaded(1);
	ASSERT_FALSE( _loadedBrowsers.empty() );
	ASSERT_EQ( 1u, _loadedBrowsers.count(1) );
}

TEST_F( BrowserApiInterface_1_0_Test, launch_browser_wiht_file_url ) {
	std::string html = util::getTestResourceDir("apps").Append("index.html").value();

	ASSERT_TRUE( iface->launchBrowser(1, "file://" + html , tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
}

TEST_F( BrowserApiInterface_1_0_Test, launch_browser_wiht_file_url_and_params ) {
	std::string html = util::getTestResourceDir("apps").Append("index.html").value();

	ASSERT_TRUE( iface->launchBrowser(1, "file://" + html + "?param=value", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
}

TEST_F( BrowserApiInterface_1_0_Test, launch_two_browser ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( iface->launchBrowser(2, "http://test2.html", tvd::browser::Options()) );
	ASSERT_EQ( 2, _browserClient->siteCount() );
}

TEST_F( BrowserApiInterface_1_0_Test, show_hide_browser_successfully ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
	ASSERT_TRUE( _browserClient->isSiteVisible(1) );
	ASSERT_TRUE( iface->showBrowser(1, false, true) );
	ASSERT_FALSE( _browserClient->isSiteVisible(1) );
	ASSERT_TRUE( _browserClient->siteHasFocus(1) );
	ASSERT_TRUE( iface->showBrowser(1, true, false) );
	ASSERT_TRUE( _browserClient->isSiteVisible(1) );
	ASSERT_FALSE( _browserClient->siteHasFocus(1) );
}

TEST_F( BrowserApiInterface_1_0_Test, fail_to_show_hide_invalid_browser ) {
	ASSERT_FALSE( iface->showBrowser(1, false, true) );
	ASSERT_FALSE( iface->showBrowser(1, true, true) );
}

TEST_F( BrowserApiInterface_1_0_Test, close_browsers ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( iface->launchBrowser(2, "http://test2.html", tvd::browser::Options()) );
	ASSERT_EQ( 2, _browserClient->siteCount() );

	ASSERT_TRUE( iface->closeBrowser(1) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
	ASSERT_TRUE( iface->closeBrowser(2) );
	ASSERT_EQ( 0, _browserClient->siteCount() );
}

TEST_F( BrowserApiInterface_1_0_Test, on_browser_closed ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( _closedBrowsers.empty() );
	ASSERT_TRUE( iface->closeBrowser(1) );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_EQ( 1u, _closedBrowsers.size() );
	ASSERT_EQ( 1, _closedBrowsers[0] );
}

TEST_F( BrowserApiInterface_1_0_Test, on_browser_closed_with_two_browsers ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_TRUE( iface->launchBrowser(2, "http://test2.html", tvd::browser::Options()) );
	ASSERT_EQ( 2, _browserClient->siteCount() );

	ASSERT_TRUE( _closedBrowsers.empty() );

	ASSERT_TRUE( iface->closeBrowser(1) );
	ASSERT_EQ( 1, _browserClient->siteCount() );

	ASSERT_EQ( 1u, _closedBrowsers.size() );
	ASSERT_EQ( 1, _closedBrowsers[0] );

	ASSERT_TRUE( iface->closeBrowser(2) );
	ASSERT_EQ( 0, _browserClient->siteCount() );

	ASSERT_EQ( 2u, _closedBrowsers.size() );
	ASSERT_EQ( 1, _closedBrowsers[0] );
	ASSERT_EQ( 2, _closedBrowsers[1] );
}

TEST_F( BrowserApiInterface_1_0_Test, on_browser_launched ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_EQ( 1, _browserClient->siteCount() );
	ASSERT_EQ( 1u, _launchedBrowsers.size() );
	ASSERT_EQ( 1, _launchedBrowsers[0] );
	ASSERT_TRUE( iface->closeBrowser(1) );
}

TEST_F( BrowserApiInterface_1_0_Test, on_browser_launched_with_two_browsers ) {
	ASSERT_TRUE( iface->launchBrowser(1, "http://test1.html", tvd::browser::Options()) );
	ASSERT_TRUE( iface->launchBrowser(2, "http://test2.html", tvd::browser::Options()) );

	ASSERT_EQ( 2, _browserClient->siteCount() );
	ASSERT_EQ( 2u, _launchedBrowsers.size() );
	ASSERT_EQ( 1, _launchedBrowsers[0] );
	ASSERT_EQ( 2, _launchedBrowsers[1] );
	ASSERT_TRUE( iface->closeBrowser(1) );
	ASSERT_TRUE( iface->closeBrowser(2) );
}
