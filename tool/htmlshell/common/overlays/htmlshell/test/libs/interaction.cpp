#include "../apis/testapi/interface.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"

namespace tvd {

// Test lib that follow the api
static TestApiInterface_1_0 *test = NULL;
static LibWrapper *libWrapper = NULL;
static HtmlShellWrapper *shellWrapper = NULL;

static void testInteraction() {
	// Test get api default mayor, minor
	TESTAPI_ASSERT_TRUE( shellWrapper->hasApi(TEST_API, 0, 0) );
	TESTAPI_ASSERT_EQ( test, shellWrapper->getApi<TestApiInterface_1_0>(TEST_API, 0, 0) );

	// Test get api bad major version
	TESTAPI_ASSERT_FALSE( shellWrapper->hasApi(TEST_API, 2, 0) );
	TESTAPI_ASSERT_EQ( NULL, shellWrapper->getApi<TestApiInterface_1_0>(TEST_API, 2, 0) );

	// Test get api bad minor version
	TESTAPI_ASSERT_FALSE( shellWrapper->hasApi(TEST_API, 1, 2) );
	TESTAPI_ASSERT_EQ( NULL, shellWrapper->getApi<TestApiInterface_1_0>(TEST_API, 1, 2) );

	// Test get invalid api
	TESTAPI_ASSERT_FALSE( shellWrapper->hasApi("invalid", 0, 0) );
	TESTAPI_ASSERT_EQ( NULL, shellWrapper->getApi<TestApiInterface_1_0>("invalid", 0, 0) );

	TESTAPI_ASSERT_TRUE( libWrapper != NULL );
}

// Library api implementation
LibWrapper *init( HtmlShellWrapper *wrapper ) {
	// Test get valid api
	if (wrapper->hasApi(TEST_API, 1, 1)) {
		test = (TestApiInterface_1_0*) wrapper->getApi<TestApiInterface_1_0>(TEST_API, 1, 1);
		if (test) {
			shellWrapper = wrapper;
			test->addTest(testInteraction);

			libWrapper = new LibWrapper("Interaction", "1.0");
		}
	}

	return libWrapper;
}

void fin() {
	delete libWrapper;
	libWrapper = NULL;
}

}
