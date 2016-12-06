#include "apis.h"
#include "../../../src/libraryinterface.h"
#include "../../../src/apis/system.h"
#include "../../../src/test/mocks.h"

namespace test {
static int exitCode = -1;
static void onShutdownCalled( int code ) {
	exitCode = code;
}
}

class SystemTest : public SdkApiTest {
};

TEST_F( SystemTest, fail_to_get_api_if_not_exist ) {
	ASSERT_TRUE( _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 0) == NULL);
}

TEST_F( SystemTest, get_api_1_0_successfully ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 0) != NULL );
}

TEST_F( SystemTest, fail_to_get_api_with_wrong_version ) {
	tvd::test::createMockApis();
	ASSERT_TRUE( _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 1) == NULL);
	ASSERT_TRUE( _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 2, 0) == NULL);
}

TEST_F( SystemTest, get_unexistent_cmd_switch ) {
	tvd::test::createMockApis();
	tvd::system::ApiInterface_1_0 *api = _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_TRUE( api->getCmdSwitch("a-switch").empty() );
}

TEST_F( SystemTest, get_valid_cmd_switch ) {
	tvd::test::createMockApis();
	tvd::system::test::setCmdSwitch("a-switch", "switch_value");
	tvd::system::ApiInterface_1_0 *api = _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_STREQ( "switch_value", api->getCmdSwitch("a-switch").c_str() );
}

TEST_F( SystemTest, shutdown_called_successfully ) {
	tvd::test::createMockApis();
	tvd::system::test::onShutdown(&test::onShutdownCalled);

	tvd::system::ApiInterface_1_0 *api = _shell->getApi<tvd::system::ApiInterface_1_0>(SYSTEM_API, 1, 0);
	ASSERT_TRUE( api != NULL );
	ASSERT_EQ( test::exitCode, -1 );
	api->shutdown(0);
	ASSERT_EQ( test::exitCode, HTMLSHELL_EXTERNAL_CODE_BASE );
	api->shutdown(1);
	ASSERT_EQ( test::exitCode, HTMLSHELL_EXTERNAL_CODE_BASE + 1 );
}
