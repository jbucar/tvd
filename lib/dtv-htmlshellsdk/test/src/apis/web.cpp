#include "apis.h"
#include "../../../src/libraryinterface.h"
#include "../../../src/apis/web.h"
#include "../../../src/test/mocks.h"
#include <boost/bind.hpp>

class WebTest : public SdkApiTest {
public:
	WebTest() {
		_handleApiCall = true;
		_handleApiResponse = true;
		_request = (tvd::web::Request) {0, 0, "", ""};
		_response = (tvd::web::Response) {0, 0, false, "", ""};
	}

	bool onApiCall( const tvd::web::Request &req ) {
		_request = req;
		return _handleApiCall;
	}

	bool onApiResponse( const tvd::web::Response &resp ) {
		_response = resp;
		return _handleApiResponse;
	}

protected:
	bool _handleApiCall;
	bool _handleApiResponse;
	tvd::web::Request _request;
	tvd::web::Response _response;
};

TEST_F( WebTest, fail_to_get_api_if_not_exist ) {
	ASSERT_TRUE( _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0) == NULL);
}

TEST_F( WebTest, get_api_1_0_successfully ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0) != NULL );
}

TEST_F( WebTest, fail_to_get_api_with_wrong_version ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 1) == NULL);
	ASSERT_TRUE( _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 2, 0) == NULL);
}

TEST_F( WebTest, fail_to_add_jsapi_with_invalid_params ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_FALSE( api->addJsApi("TestJsApi", NULL) );
	ASSERT_FALSE( api->addJsApi("", boost::bind(&WebTest::onApiCall, this, _1)) );
	ASSERT_FALSE( api->addJsApi("", NULL) );

	ASSERT_FALSE( tvd::web::test::simulateApiCall("TestJsApi", (tvd::web::Request) {1, 2, "aRequest", "params"}) );
	ASSERT_EQ( 0, _request.browserId );
	ASSERT_EQ( 0, _request.queryId );
	ASSERT_TRUE( _request.name.empty() );
	ASSERT_TRUE( _request.params.empty() );
}

TEST_F( WebTest, add_jsapi_successfully ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_TRUE( api->addJsApi("TestJsApi", boost::bind(&WebTest::onApiCall, this, _1)) );

	ASSERT_TRUE( tvd::web::test::simulateApiCall("TestJsApi", (tvd::web::Request) {1, 2, "aRequest", "params"}) );
	ASSERT_EQ( 1, _request.browserId );
	ASSERT_EQ( 2, _request.queryId );
	ASSERT_STREQ( "aRequest", _request.name.c_str() );
	ASSERT_STREQ( "params", _request.params.c_str() );
}

TEST_F( WebTest, jsapi_not_handle_apicall ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_TRUE( api->addJsApi("TestJsApi", boost::bind(&WebTest::onApiCall, this, _1)) );

	_handleApiCall = false;
	ASSERT_FALSE( tvd::web::test::simulateApiCall("TestJsApi", (tvd::web::Request) {1, 2, "aRequest", "params"}) );
	ASSERT_EQ( 1, _request.browserId );
	ASSERT_EQ( 2, _request.queryId );
	ASSERT_STREQ( "aRequest", _request.name.c_str() );
	ASSERT_STREQ( "params", _request.params.c_str() );
}

TEST_F( WebTest, fail_to_remove_inexitent_api ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );

	ASSERT_FALSE( api->removeJsApi("TestJsApi") );
}

TEST_F( WebTest, remove_api_successfully ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_TRUE( api->addJsApi("TestJsApi", boost::bind(&WebTest::onApiCall, this, _1)) );

	ASSERT_TRUE( api->removeJsApi("TestJsApi") );

	ASSERT_FALSE( tvd::web::test::simulateApiCall("TestJsApi", (tvd::web::Request) {1, 2, "aRequest", "params"}) );
	ASSERT_EQ( 0, _request.browserId );
	ASSERT_EQ( 0, _request.queryId );
	ASSERT_TRUE( _request.params.empty() );
}

TEST_F( WebTest, execute_callback_successfully ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );

	tvd::web::test::onExecuteCallback(boost::bind(&WebTest::onApiResponse, this, _1));
	ASSERT_TRUE( api->executeCallback((tvd::web::Response) {1, 2, true, "An error", "response_params"}) );
	ASSERT_EQ( 1, _response.browserId );
	ASSERT_EQ( 2, _response.queryId );
	ASSERT_TRUE( _response.isSignal );
	ASSERT_STREQ( "An error", _response.error.c_str() );
	ASSERT_STREQ( "response_params", _response.params.c_str() );
}

TEST_F( WebTest, fail_to_execute_callback_successfully ) {
	tvd::test::createMockApis();
	tvd::web::ApiInterface_1_0 *api = _shell->getApi<tvd::web::ApiInterface_1_0>(WEB_API, 1, 0);
	ASSERT_TRUE( api != NULL );

	tvd::web::test::onExecuteCallback(boost::bind(&WebTest::onApiResponse, this, _1));
	_handleApiResponse = false;
	ASSERT_FALSE( api->executeCallback((tvd::web::Response) {1, 2, true, "An error", "response_params"}) );
	ASSERT_EQ( 1, _response.browserId );
	ASSERT_EQ( 2, _response.queryId );
	ASSERT_TRUE( _response.isSignal );
	ASSERT_STREQ( "An error", _response.error.c_str() );
	ASSERT_STREQ( "response_params", _response.params.c_str() );
}
