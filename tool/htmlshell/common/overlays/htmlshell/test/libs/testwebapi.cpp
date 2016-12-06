#include "../apis/testapi/interface.h"
#include "../../src/errors.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"
#include "dtv-htmlshellsdk/src/apis/web.h"
#include "base/strings/string_number_conversions.h"

namespace tvd {

// Test lib that follow the api
static HtmlShellWrapper *wrapper = NULL;
static LibWrapper *libWrapper = NULL;
static TestApiInterface_1_0 *test = NULL;
static web::ApiInterface_1_0 *webIface = NULL;

static bool testHandleMessage( const web::Request &req ) {
	if (req.name == "JsApiTestSimple") {
		return webIface->executeCallback({req.browserId, req.queryId, false, "", "[response]"});
	}
	else if (req.name == "JsApiTestComplex") {
		TESTAPI_ASSERT_EQ( std::string("['string',1,3.14,true,null,undefined]"), req.params );
		return webIface->executeCallback({req.browserId, req.queryId, false, "", "[\"response\",2,2.71,false,undefined,null]"});
	}
	else if (req.name == "JsApiTestStop") {
		TESTAPI_ASSERT_TRUE( !req.params.empty() );
		std::string numberStr(req.params, 1, req.params.length()-2 );
		int exitCode = -1;
		base::StringToInt(numberStr, &exitCode);
		test->stopShell( exitCode );
		return true;
	}
	else if (req.name == "JsApiTestParams") {
		TESTAPI_ASSERT_EQ( std::string("[\"string\",1,1.2,true,false,null,[\"array\"]]"), req.params );
		return webIface->executeCallback({req.browserId, req.queryId, false, "", "[0]"});
	}
	else if (req.name == "JsApiTestError") {
		webIface->executeCallback({req.browserId, req.queryId, false, "Error description", ""});
		return true;
	}
	else if (req.name == "JsApiTestSignals") {
		std::string resp("[\"TestEvent\",0]");
		webIface->executeCallback({req.browserId, req.queryId, true, "", resp});
		webIface->executeCallback({req.browserId, req.queryId, true, "", resp});
		webIface->executeCallback({req.browserId, req.queryId, true, "", resp});
		return true;
	}
	return false;
}

// Library api implementation
LibWrapper *init( HtmlShellWrapper *shellWrapper ) {
	// Get test api
	wrapper = shellWrapper;
	if (wrapper->hasApi(TEST_API, 1, 0)) {
		test = wrapper->getApi<TestApiInterface_1_0>(TEST_API, 1, 0);
		if (test) {
			TESTAPI_ASSERT_TRUE( wrapper->hasApi(WEB_API, 1, 0) );
			webIface = wrapper->getApi<web::ApiInterface_1_0>(WEB_API, 1, 0);
			TESTAPI_ASSERT_TRUE( webIface != NULL );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestSimple", std::bind(&testHandleMessage, std::placeholders::_1)) );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestComplex", std::bind(&testHandleMessage, std::placeholders::_1)) );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestStop", std::bind(&testHandleMessage, std::placeholders::_1)) );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestParams", std::bind(&testHandleMessage, std::placeholders::_1)) );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestError", std::bind(&testHandleMessage, std::placeholders::_1)) );
			TESTAPI_ASSERT_TRUE( webIface->addJsApi("JsApiTestSignals", std::bind(&testHandleMessage, std::placeholders::_1)) );

			libWrapper = new LibWrapper("TestWebApi", "1.0");
		}
	}

	return libWrapper;
}

void fin() {
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestSimple") );
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestComplex") );
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestStop") );
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestParams") );
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestError") );
	TESTAPI_ASSERT_TRUE( webIface->removeJsApi("JsApiTestSignals") );

	delete libWrapper;
	libWrapper = NULL;
	wrapper = NULL;
	webIface = NULL;
	test = NULL;
}

}
