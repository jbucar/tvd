#include "systemapi.h"
#include "../../../src/errors.h"
#include "../../../src/apis/system/systemapi.h"
#include "../../../src/impl/app/maindelegate.h"
#include "dtv-htmlshellsdk/src/apis/system.h"
#include "base/logging.h"

#define SYSTEM_API_TEST_NO_ERROR 0
#define SYSTEM_API_TEST_ERROR_1  1

namespace test {

static int exit_code = HTMLSHELL_NO_ERROR;

class HtmlShellMock : public tvd::MainDelegate {
public:
	virtual void stop( int error=HTMLSHELL_NO_ERROR ) {
		exit_code = error;
	}
};

}

SystemApiTest::SystemApiTest()
{
	_api = NULL;
	_shell = NULL;	
}

SystemApiTest::~SystemApiTest()
{
	CHECK(_api == NULL);
	CHECK(_shell == NULL);
}

void SystemApiTest::SetUp() {
	_shell = new test::HtmlShellMock();
	_api = new tvd::SystemApi(_shell);
	ASSERT_TRUE( _api != NULL );
}

void SystemApiTest::TearDown() {
	_api->finalize();
	delete _api;
	_api = NULL;

	delete _shell;
	_shell = NULL;
}

SystemApiInterface_1_0_Test::SystemApiInterface_1_0_Test()
{
	iface = NULL;
}

void SystemApiInterface_1_0_Test::SetUp() {
	SystemApiTest::SetUp();
	ASSERT_TRUE(_api->initialize());
	iface = static_cast<tvd::system::ApiInterface_1_0*>(_api->getInterface(1,0));
	ASSERT_TRUE( iface != NULL );
}

void SystemApiInterface_1_0_Test::TearDown() {
	iface = NULL;
	SystemApiTest::TearDown();
}

TEST_F( SystemApiTest, constructed_successfully ) {
	ASSERT_STREQ( "systemapi", _api->name().c_str() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, test::exit_code );
}

TEST_F( SystemApiTest, initialized_successfully ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, test::exit_code );
}

TEST_F( SystemApiTest, get_interface_version_1_0 ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_TRUE( _api->hasInterface(1,0) );
	ASSERT_TRUE( _api->getInterface(1,0) != NULL );
}

TEST_F( SystemApiTest, get_default_interface ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_TRUE( _api->hasInterface(0,0) );
	ASSERT_TRUE( _api->getInterface(0,0) );
}

TEST_F( SystemApiTest, api_default_interface_is_last_interface ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( _api->getInterface(0,0), _api->getInterface(SYSTEM_API_VERSION_MAJOR, SYSTEM_API_VERSION_MINOR) );
}

TEST_F( SystemApiTest, shutdown_with_no_error ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, test::exit_code );

	static_cast<tvd::system::ApiInterface_1_0*>(_api->getInterface(1, 0))->shutdown(SYSTEM_API_TEST_NO_ERROR);
	ASSERT_EQ( HTMLSHELL_EXTERNAL_CODE_NO_ERROR, test::exit_code );
	ASSERT_EQ( HTMLSHELL_EXTERNAL_CODE_BASE + SYSTEM_API_TEST_NO_ERROR, test::exit_code );
}

TEST_F( SystemApiTest, shutdown_with_error ) {
	ASSERT_TRUE( _api->initialize() );
	ASSERT_EQ( HTMLSHELL_NO_ERROR, test::exit_code );

	static_cast<tvd::system::ApiInterface_1_0*>(_api->getInterface(1, 0))->shutdown(SYSTEM_API_TEST_ERROR_1);
	ASSERT_EQ( HTMLSHELL_EXTERNAL_CODE_BASE + SYSTEM_API_TEST_ERROR_1, test::exit_code );
	ASSERT_GT( test::exit_code, HTMLSHELL_EXTERNAL_CODE_NO_ERROR );
}
