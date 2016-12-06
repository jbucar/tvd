#include "webapi.h"
#include "dtv-htmlshellsdk/src/apis/web.h"
#include "../testapi/testapi.h"
#include "../../../src/errors.h"
#include "../../../src/library.h"
#include "../../../src/switches.h"
#include "../../../src/apis/web/webapi.h"
#include "base/logging.h"
#include "base/command_line.h"

namespace impl {

static int g_count = 0;
static tvd::web::ApiInterface_1_0 *g_iface = NULL;
static tvd::web::Response queryResp;

class WebApiMock : public tvd::WebApi {
public:
	WebApiMock() : tvd::WebApi(NULL) {}

	virtual bool executeCallback( const tvd::web::Response &response ) {
		queryResp = response;
		return true;
	}
};

static bool testHandleMsgCb( const tvd::web::Request &req ) {
	g_count++;
	LOG(INFO) << "Test handle message: " << req.name;
	if (req.name == "webapi1") {
		return req.params.empty();
	}
	else if (req.name == "webapi2") {
		return g_iface->executeCallback({req.browserId, req.queryId, false, "", "Hello, " + req.params});
	}
	else if (req.name == "webapi3") {
		bool check = true;
		check &= req.params == "[\"string\",1,3.14,true,null]";
		check &= g_iface->executeCallback({req.browserId, req.queryId, false, "", "[\"response\",2,2.71,false,undefined,null]"});
		return check;
	}
	return false;
}

}

WebApiTest::WebApiTest()
{
	_webApi = NULL;
}

void WebApiTest::SetUp() {
	_webApi = new impl::WebApiMock();
	ASSERT_TRUE( _webApi->initialize() );
}

void WebApiTest::TearDown() {
	_webApi->finalize();
	delete _webApi;
	_webApi = NULL;
}

WebApiInterface_1_0_Test::WebApiInterface_1_0_Test()
{
	iface = NULL;
}

void WebApiInterface_1_0_Test::SetUp() {
	WebApiTest::SetUp();
	impl::g_iface = iface = static_cast<tvd::web::ApiInterface_1_0*>(_webApi->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );
}

void WebApiInterface_1_0_Test::TearDown() {
	impl::g_iface = iface = NULL;
	WebApiTest::TearDown();
}

// Test basic code used then in setup and teardown of WebApiTest
TEST( WebApiTest0, web_api_init_fin_successfully ) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	// SetUp
	tvd::WebApi *api = new tvd::WebApi(NULL);
	ASSERT_TRUE( api != NULL );
	ASSERT_TRUE( api->initialize() );

	// TearDown
	api->finalize();
	delete api;
	api = NULL;
}

TEST_F( WebApiTest, webapi_construct_successfully ) {
	ASSERT_STREQ( "webapi", _webApi->name().c_str() );
}

TEST_F( WebApiTest, webapi_get_interface_version_1_0 ) {
	ASSERT_TRUE( _webApi->hasInterface(1,0) );
	ASSERT_TRUE( _webApi->getInterface(1,0) != NULL );
}

TEST_F( WebApiTest, dont_has_js_api ) {
	ASSERT_FALSE( _webApi->hasJsApi("unknown") );
}

TEST_F( WebApiInterface_1_0_Test, fail_to_add_js_api_with_invalid_handle_msg_callback ) {
	ASSERT_FALSE( iface->addJsApi("webapi", NULL) );
	ASSERT_FALSE( _webApi->hasJsApi("webapi") );
}

TEST_F( WebApiInterface_1_0_Test, add_valid_js_api_successfully ) {
	impl::g_count = 0;
	ASSERT_TRUE( iface->addJsApi("webapi1", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_TRUE( _webApi->hasJsApi("webapi1") );
	ASSERT_EQ( 0, impl::g_count );
	ASSERT_TRUE( _webApi->onMessageReceived({1, 2, "webapi1"}) );

	ASSERT_EQ( 1, impl::g_count );
}

TEST_F( WebApiInterface_1_0_Test, remove_js_api_successfully ) {
	impl::g_count = 0;
	ASSERT_TRUE( iface->addJsApi("webapi1", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_TRUE( _webApi->hasJsApi("webapi1") );
	ASSERT_EQ( 0, impl::g_count );
	ASSERT_TRUE( _webApi->onMessageReceived({1, 2, "webapi1"}) );
	ASSERT_EQ( 1, impl::g_count );

	ASSERT_TRUE( iface->removeJsApi("webapi1") );
	ASSERT_FALSE( _webApi->hasJsApi("webapi1") );
	ASSERT_EQ( 1, impl::g_count );
	ASSERT_FALSE( _webApi->onMessageReceived({1, 2, "webapi1"}) );
	ASSERT_EQ( 1, impl::g_count );
}

TEST_F( WebApiInterface_1_0_Test, fail_to_add_same_api_twice ) {
	ASSERT_TRUE( iface->addJsApi("webapi", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_FALSE( iface->addJsApi("webapi", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_TRUE( _webApi->hasJsApi("webapi") );
}

TEST_F( WebApiInterface_1_0_Test, webapi_dont_handle_unknown_msg ) {
	ASSERT_TRUE( iface->addJsApi("webapi", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );

	ASSERT_EQ( 0, impl::g_count );
	ASSERT_FALSE( _webApi->onMessageReceived({1, 2, "webapi", "unknown"}) );

	ASSERT_EQ( 1, impl::g_count );
}

TEST_F( WebApiInterface_1_0_Test, webapi_handle_msg_return_value_successfully ) {
	impl::g_count = 0;
	ASSERT_TRUE( iface->addJsApi("webapi2", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );

	ASSERT_EQ( 0, impl::g_count );
	ASSERT_TRUE( _webApi->onMessageReceived({1, 2, "webapi2", "World"}) );

	ASSERT_EQ( 1, impl::g_count );
	ASSERT_FALSE( impl::queryResp.isSignal );
	ASSERT_EQ( 1, impl::queryResp.browserId );
	ASSERT_EQ( 2, impl::queryResp.queryId );
	ASSERT_STREQ( "Hello, World", impl::queryResp.params.c_str() );
}

TEST_F( WebApiInterface_1_0_Test, webapi_not_handle_msg_to_unknown_js_api ) {
	ASSERT_TRUE( iface->addJsApi("webapi", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_FALSE( _webApi->onMessageReceived({1, 2, "webapi4", ""}) );
	ASSERT_EQ( 0, impl::g_count );
}

TEST_F( WebApiInterface_1_0_Test, webapi_handle_simple_message ) {
	ASSERT_TRUE( iface->executeCallback({1, 2, false, "", "\"Response\""}) );
	ASSERT_TRUE( impl::queryResp.error.empty() );
	ASSERT_FALSE( impl::queryResp.isSignal );
	ASSERT_EQ( 1, impl::queryResp.browserId );
	ASSERT_EQ( 2, impl::queryResp.queryId );
	ASSERT_STREQ( "\"Response\"", impl::queryResp.params.c_str() );
}

TEST_F( WebApiInterface_1_0_Test, webapi_handle_query_error ) {
	iface->executeCallback({1, 2, false, "\"Internal error\"", ""});
	ASSERT_TRUE( impl::queryResp.params.empty() );
	ASSERT_FALSE( impl::queryResp.isSignal );
	ASSERT_EQ( 1, impl::queryResp.browserId );
	ASSERT_EQ( 2, impl::queryResp.queryId );
	ASSERT_STREQ( "\"Internal error\"", impl::queryResp.error.c_str() );
}

TEST_F( WebApiInterface_1_0_Test, webapi_handle_query_signal ) {
	iface->executeCallback({1, 2, true, "", "\"A test event\""});
	ASSERT_TRUE( impl::queryResp.error.empty() );
	ASSERT_TRUE( impl::queryResp.isSignal );
	ASSERT_EQ( 1, impl::queryResp.browserId );
	ASSERT_EQ( 2, impl::queryResp.queryId );
	ASSERT_STREQ( "\"A test event\"", impl::queryResp.params.c_str() );
}

TEST_F( WebApiInterface_1_0_Test, webapi_handle_complex_message ) {
	ASSERT_TRUE( iface->addJsApi("webapi3", std::bind(&impl::testHandleMsgCb, std::placeholders::_1)) );
	ASSERT_TRUE( _webApi->onMessageReceived({1, 2, "webapi3", "[\"string\",1,3.14,true,null]"}) );
	ASSERT_FALSE( impl::queryResp.isSignal );
	ASSERT_EQ( 1, impl::queryResp.browserId );
	ASSERT_EQ( 2, impl::queryResp.queryId );
	ASSERT_STREQ( "[\"response\",2,2.71,false,undefined,null]", impl::queryResp.params.c_str() );
}
