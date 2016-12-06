#include "apis.h"
#include "../../../src/libraryinterface.h"
#include "../../../src/apis/browser.h"
#include "../../../src/test/mocks.h"
#include <boost/bind.hpp>
#include <map>

namespace test {

struct KeyData {
	int code;
	bool isUp;
};

struct LaunchData {
	std::string url;
	tvd::browser::Options opts;
};

}

class BasicBrowserTest : public SdkApiTest {
};

class BrowserTest : public SdkApiTest {
public:
	BrowserTest() {
		_api = NULL;
		_handleKey = false;
		_listener = NULL;
	}

	void onLaunched( int id ) {
		_launched.push_back(id);
	}

	void onClosed( int id, const std::string &error ) {
		_closed.insert(std::make_pair(id, error));
	}

	void onLoadProgress( int id ) {
		_progress.insert(id);
	}

	bool onKey( int id, int keycode, bool isUp ) {
		_keys.insert(std::make_pair(id, (test::KeyData) {keycode, isUp}));
		return _handleKey;
	}

	void onWebLog( int id, const tvd::browser::WebLog &logData ) {
		_errors.insert(std::make_pair(id, logData));
	}

	void onLaunchBrowserRequest( int id, const std::string &url, const tvd::browser::Options &opts ) {
		_launchData.insert(std::make_pair(id, (test::LaunchData) {url, opts}));
	}

protected:
	virtual void SetUp() {
		SdkApiTest::SetUp();
		tvd::test::createMockApis();
		_api = _shell->getApi<tvd::browser::ApiInterface_1_0>(BROWSER_API, 1, 0);
		ASSERT_TRUE( _api != NULL );
		_listener = new tvd::browser::ListenerForwarder<BrowserTest>(this);
		_api->addListener(_listener);
	}

	virtual void TearDown() {
		_api->rmListener(_listener);
		_api = NULL;
		delete _listener;
		_listener = NULL;
		SdkApiTest::TearDown();
	}

	tvd::browser::ApiInterface_1_0 *_api;
	bool _handleKey;

	std::vector<int> _launched;
	std::map<int, std::string> _closed;
	std::map<int, test::KeyData> _keys;
	std::map<int, tvd::browser::WebLog> _errors;
	std::map<int, test::LaunchData> _launchData;
	std::set<int> _progress;
	tvd::browser::Listener *_listener;
};

TEST_F( BasicBrowserTest, fail_to_get_api_if_not_exist ) {
	ASSERT_TRUE( _shell->getApi<tvd::browser::ApiInterface_1_0>(BROWSER_API, 1, 0) == NULL);
}

TEST_F( BasicBrowserTest, get_api_1_0_successfully ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::browser::ApiInterface_1_0>(BROWSER_API, 1, 0) != NULL );
}

TEST_F( BasicBrowserTest, fail_to_get_api_with_wrong_version ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::browser::ApiInterface_1_0>(BROWSER_API, 1, 1) == NULL);
	ASSERT_TRUE( _shell->getApi<tvd::browser::ApiInterface_1_0>(BROWSER_API, 2, 0) == NULL);
}

TEST_F( BrowserTest, failt_to_launch_browser_with_invalid_urls ) {
	ASSERT_FALSE( _api->launchBrowser(0, "invalid", (tvd::browser::Options) {}) );
	ASSERT_FALSE( _api->launchBrowser(0, "file://unexistent", (tvd::browser::Options) {}) );
	ASSERT_FALSE( _api->launchBrowser(0, "ftp://not_permited", (tvd::browser::Options) {}) );
	ASSERT_TRUE( _launched.empty() );
}

TEST_F( BrowserTest, launch_browser_successfully ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
}

TEST_F( BrowserTest, browser_load_progress ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
	ASSERT_TRUE( _progress.empty() );
	tvd::browser::test::emitLoaded(0);
	ASSERT_FALSE( _progress.empty() );
	ASSERT_EQ( 1, _progress.count(0) );
}

TEST_F( BrowserTest, fail_to_launch_browser_with_same_ids ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
	ASSERT_FALSE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
}

TEST_F( BrowserTest, launch_two_browsers_successfully ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
	ASSERT_TRUE( _api->launchBrowser(1, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 2, _launched.size() );
	ASSERT_EQ( 1, _launched[1] );
}

TEST_F( BrowserTest, fail_to_close_browser_with_invalid_id ) {
	ASSERT_FALSE( _api->closeBrowser(0) );
	ASSERT_TRUE( _closed.empty() );
}

TEST_F( BrowserTest, close_browser_successfully ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );

	ASSERT_TRUE( _api->closeBrowser(0) );
	ASSERT_EQ( 1, _closed.size() );
	ASSERT_TRUE( _closed.find(0) != _closed.end() );
	ASSERT_STREQ( "", _closed[0].c_str() );
}

TEST_F( BrowserTest, close_browser_successfully_with_error ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );

	tvd::browser::test::setOnCloseError("An error");
	ASSERT_TRUE( _api->closeBrowser(0) );
	ASSERT_EQ( 1, _closed.size() );
	ASSERT_TRUE( _closed.find(0) != _closed.end() );
	ASSERT_STREQ( "An error", _closed[0].c_str() );
}

TEST_F( BrowserTest, fail_to_close_browser_twice ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );

	ASSERT_TRUE( _api->closeBrowser(0) );
	ASSERT_EQ( 1, _closed.size() );
	ASSERT_TRUE( _closed.find(0) != _closed.end() );
	ASSERT_STREQ( "", _closed[0].c_str() );

	ASSERT_FALSE( _api->closeBrowser(0) );
	ASSERT_EQ( 1, _closed.size() );
}

TEST_F( BrowserTest, check_launch_browser_options ) {
	tvd::browser::test::onLaunchBrowserRequest(boost::bind(&BrowserTest::onLaunchBrowserRequest, this, _1, _2, _3));
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {false, true, true, false, true, 1, (tvd::browser::Bounds) {10, 20, 800, 600}, "use strict;", "Extra UA", std::vector<std::string>(1, "Test PlugIn")}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
	ASSERT_EQ( 1, _launchData.size() );
	ASSERT_TRUE( _launchData.find(0) != _launchData.end() );
	ASSERT_STREQ( "http://google.com", _launchData[0].url.c_str() );
	ASSERT_FALSE( _launchData[0].opts.transparentBg );
	ASSERT_TRUE( _launchData[0].opts.needFocus );
	ASSERT_TRUE( _launchData[0].opts.visible );
	ASSERT_FALSE( _launchData[0].opts.enableMouse );
	ASSERT_TRUE( _launchData[0].opts.enableGeolocation );
	ASSERT_EQ( 10, _launchData[0].opts.bounds.x );
	ASSERT_EQ( 20, _launchData[0].opts.bounds.y );
	ASSERT_EQ( 800, _launchData[0].opts.bounds.w );
	ASSERT_EQ( 600, _launchData[0].opts.bounds.h );
	ASSERT_EQ( 1, _launchData[0].opts.zIndex );
	ASSERT_EQ( 1, _launchData[0].opts.plugins.size() );
	ASSERT_EQ( "Test PlugIn", _launchData[0].opts.plugins[0] );
	ASSERT_STREQ( "use strict;", _launchData[0].opts.initJS.c_str() );
	ASSERT_STREQ( "Extra UA", _launchData[0].opts.extraUA.c_str() );
}

TEST_F( BrowserTest, emit_keyboard_event_not_handled ) {
	ASSERT_FALSE( tvd::browser::test::emitKeyboardEvent(0, 10, true) );
	ASSERT_EQ( 1, _keys.size());
	ASSERT_TRUE( _keys.find(0) != _keys.end() );
	ASSERT_EQ( 10, _keys[0].code );
	ASSERT_TRUE( _keys[0].isUp );
}

TEST_F( BrowserTest, emit_keyboard_event_handled ) {
	_handleKey = true;
	ASSERT_TRUE( tvd::browser::test::emitKeyboardEvent(0, 10, true) );
	ASSERT_EQ( 1, _keys.size());
	ASSERT_TRUE( _keys.find(0) != _keys.end() );
	ASSERT_EQ( 10, _keys[0].code );
	ASSERT_TRUE( _keys[0].isUp );
}

TEST_F( BrowserTest, javascript_error_are_forwarded_successfully ) {
	tvd::browser::test::emitWebLog(0, (tvd::browser::WebLog) {2, 10, "Error msg", "scripts/main.js"});
	ASSERT_EQ( 1, _errors.size());
	ASSERT_TRUE( _errors.find(0) != _errors.end() );
	ASSERT_EQ( 10, _errors[0].line );
	ASSERT_STREQ( "Error msg", _errors[0].message.c_str() );
	ASSERT_STREQ( "scripts/main.js", _errors[0].source.c_str() );
}

TEST_F( BrowserTest, hide_show_browser_successfully ) {
	ASSERT_TRUE( _api->launchBrowser(0, "http://google.com", (tvd::browser::Options) {}) );
	ASSERT_EQ( 1, _launched.size() );
	ASSERT_EQ( 0, _launched[0] );
	ASSERT_FALSE( tvd::browser::test::isHidden(0) );
	ASSERT_TRUE( _api->showBrowser(0, false, true) );
	ASSERT_TRUE( tvd::browser::test::isHidden(0) );
	ASSERT_TRUE( _api->showBrowser(0, true, true) );
	ASSERT_FALSE( tvd::browser::test::isHidden(0) );
}

TEST_F( BrowserTest, fail_to_hide_show_inexistent_browser ) {
	ASSERT_FALSE( _api->showBrowser(0, false, true) );
	ASSERT_FALSE( _api->showBrowser(0, true, true) );
}
