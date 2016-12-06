#include "../apis/testapi/interface.h"
#include "../../src/switches.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "dtv-htmlshellsdk/src/apis/fs.h"
#include "dtv-htmlshellsdk/src/apis/browser.h"
#include "dtv-htmlshellsdk/src/apis/system.h"
#include <map>

namespace tvd {

static LibWrapper *libWrapper = NULL;
static HtmlShellWrapper *htmlShellWrapper = NULL;
static TestApiInterface_1_0 *test = NULL;

namespace browser_test {

class BrowserListener : public tvd::browser::Listener {
public:
	BrowserListener() {
		_handleKey = false;
		_isUp = false;
		_id = 0;
		_keycode = 0;
		_called = 0;
	}

	void onClosed( int id, const std::string &error ) override {
		TESTAPI_ASSERT_TRUE( id > 0 );
		_browsersClosed.insert(std::make_pair(id,error));
	}

	bool onKey( int id, int keycode, bool isUp ) override {
		_called++;
		_id = id;
		_keycode = keycode;
		_isUp = isUp;
		return _handleKey;
	}

	void onWebLog( int id, const browser::WebLog &logData ) override {
		_id = id;
		_logData = logData;
	}

	int browsersClosed() {
		return _browsersClosed.size();
	}

	bool _handleKey;
	bool _isUp;
	int _id;
	int _keycode;
	int _called;
	browser::WebLog _logData;
	std::map<int, std::string> _browsersClosed;
};

}

static void testFsApi() {
	TESTAPI_ASSERT_TRUE( htmlShellWrapper->hasApi(FS_API, 1, 0) );
	fs::ApiInterface_1_0 *api = htmlShellWrapper->getApi<fs::ApiInterface_1_0>(FS_API, 1, 0);
	TESTAPI_ASSERT_TRUE( api != NULL );

	TESTAPI_ASSERT_EQ( test->getTestProfileDir("user"), api->getUserProfileDir() );
	TESTAPI_ASSERT_EQ( test->getTestProfileDir("system"), api->getSystemProfileDir() );
}

static void testBrowserApi() {
	browser_test::BrowserListener handler;
	TESTAPI_ASSERT_TRUE( htmlShellWrapper->hasApi(BROWSER_API, 1, 0) );
	browser::ApiInterface_1_0 *api = htmlShellWrapper->getApi<browser::ApiInterface_1_0>(BROWSER_API, 1, 0);
	TESTAPI_ASSERT_TRUE( api != NULL );

	api->addListener(&handler);

	TESTAPI_ASSERT_TRUE( api->launchBrowser(1, "http://google.com", tvd::browser::Options()) );
	TESTAPI_ASSERT_TRUE( api->closeBrowser(1) );
	test->signalBrowserClosed(1, "");
	TESTAPI_ASSERT_EQ( 1u, handler.browsersClosed() );
	TESTAPI_ASSERT_EQ( handler._browsersClosed[1], "" );
	TESTAPI_ASSERT_FALSE( api->closeBrowser(1) );
	TESTAPI_ASSERT_EQ( 1u, handler.browsersClosed() );

	TESTAPI_ASSERT_TRUE( api->launchBrowser(2, "http://google.com", tvd::browser::Options()) );
	test->signalBrowserClosed(2, "An error");
	TESTAPI_ASSERT_EQ( 2u, handler.browsersClosed() );
	TESTAPI_ASSERT_EQ( handler._browsersClosed[2], "An error" );

	test->emitError(15, "Error message!", "http://google.com.ar", 1);
	TESTAPI_ASSERT_EQ( 15, handler._id );
	TESTAPI_ASSERT_EQ( "Error message!", handler._logData.message );
	TESTAPI_ASSERT_EQ( "http://google.com.ar", handler._logData.source );
	TESTAPI_ASSERT_EQ( 1, handler._logData.line );
	TESTAPI_ASSERT_EQ( 2, handler._logData.level );

	TESTAPI_ASSERT_FALSE( test->emitKeyboardEvent(1, 10, false) );
	TESTAPI_ASSERT_EQ( 1, handler._id );
	TESTAPI_ASSERT_EQ( 10, handler._keycode );
	TESTAPI_ASSERT_FALSE( handler._isUp );
	TESTAPI_ASSERT_EQ( 1, handler._called );

	handler._handleKey = true;
	TESTAPI_ASSERT_TRUE( test->emitKeyboardEvent(2, 20, true) );
	TESTAPI_ASSERT_EQ( 2, handler._id );
	TESTAPI_ASSERT_EQ( 20, handler._keycode );
	TESTAPI_ASSERT_TRUE( handler._isUp );
	TESTAPI_ASSERT_EQ( 2, handler._called );

	api->rmListener(&handler);

	TESTAPI_ASSERT_FALSE( test->emitKeyboardEvent(3, 30, false) );
	TESTAPI_ASSERT_EQ( 2, handler._id );
	TESTAPI_ASSERT_EQ( 20, handler._keycode );
	TESTAPI_ASSERT_TRUE( handler._isUp );
	TESTAPI_ASSERT_EQ( 2, handler._called );
}

static void testSystemApi() {
	TESTAPI_ASSERT_TRUE( htmlShellWrapper->hasApi(SYSTEM_API, 1, 0) );
	system::ApiInterface_1_0 *api = htmlShellWrapper->getApi<system::ApiInterface_1_0>(SYSTEM_API, 1, 0);
	TESTAPI_ASSERT_TRUE( api != NULL );

	TESTAPI_ASSERT_EQ( "Test", api->getCmdSwitch("test-switch") );
}

// Library api implementation
LibWrapper *init( HtmlShellWrapper *wrapper ) {
	// Get test api
	if (wrapper->hasApi(TEST_API, 1, 0)) {
		test = wrapper->getApi<TestApiInterface_1_0>(TEST_API, 1, 0);
		if (test) {
			htmlShellWrapper = wrapper;

			test->addTest(testFsApi);
			test->addTest(testBrowserApi);
			test->addTest(testSystemApi);

			libWrapper = new LibWrapper("TestApis", "1.0");
		}
	}

	return libWrapper;
}

void fin() {
	delete libWrapper;
	libWrapper = NULL;
	htmlShellWrapper = NULL;
	test = NULL;
}

}
