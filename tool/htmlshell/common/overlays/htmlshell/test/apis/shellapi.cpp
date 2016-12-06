#include "shellapi.h"
#include "testapi/testapi.h"
#include "../htmlshell.h"

class TestApiRegisterFail : public TestApi {
public:
	using TestApi::TestApi;

protected:
	virtual bool registerInterfaces() {
		bool check = true;
		check &= callRegisterInterface( new impl::TestApi_1_0(nullptr), 0x00010000u);
		check &= callRegisterInterface( new impl::TestApi_1_0(nullptr), 0x00010000u);
		return check;
	}
};

ShellApiTest::ShellApiTest()
{
	testApi = nullptr;
	delegate = nullptr;
}

void ShellApiTest::SetUp() {
	delegate = new test::FakeMainDelegate();
	testApi = new TestApi(delegate);
	ASSERT_TRUE( testApi->initialize() );
}

void ShellApiTest::TearDown() {
	testApi->finalize();
	delete testApi;
	testApi = nullptr;
	delete delegate;
	delegate = nullptr;
}

TEST( ShellApiFailTest, fail_to_init_twice ) {
	tvd::MainDelegate *delegate = new test::FakeMainDelegate();
	TestApi *testApi = new TestApi(delegate);
	ASSERT_TRUE( testApi->initialize() );
	ASSERT_FALSE( testApi->initialize() );
	testApi->finalize();
	delete testApi;
	delete delegate;
}

TEST( ShellApiFailTest, fail_to_init_if_register_same_interface_twice ) {
	tvd::MainDelegate *delegate = new test::FakeMainDelegate();
	TestApi *testApi = new TestApiRegisterFail(delegate);
	ASSERT_FALSE( testApi->initialize() );
	delete testApi;
	delete delegate;
}

TEST( ShellApiFailTest, fail_to_get_iface_after_finalized ) {
	tvd::MainDelegate *delegate = new test::FakeMainDelegate();
	TestApi *testApi = new TestApi(delegate);

	ASSERT_TRUE( testApi->initialize() );

	ASSERT_TRUE( testApi->hasInterface(0,0) );
	ASSERT_TRUE( testApi->hasInterface(1,0) );
	ASSERT_TRUE( testApi->hasInterface(1,1) );
	ASSERT_TRUE( testApi->getInterface(0,0) != nullptr );
	ASSERT_TRUE( testApi->getInterface(1,0) != nullptr );
	ASSERT_TRUE( testApi->getInterface(1,1) != nullptr );

	testApi->finalize();

	ASSERT_FALSE( testApi->hasInterface(0,0) );
	ASSERT_FALSE( testApi->hasInterface(1,0) );
	ASSERT_FALSE( testApi->hasInterface(1,1) );
	ASSERT_EQ( nullptr, testApi->getInterface(0,0) );
	ASSERT_EQ( nullptr, testApi->getInterface(1,0) );
	ASSERT_EQ( nullptr, testApi->getInterface(1,1) );

	delete testApi;
	delete delegate;
}

TEST_F( ShellApiTest, fail_to_register_interface_outside_registerinterfaces_call ) {
	ASSERT_FALSE( testApi->callRegisterInterface(new impl::TestApi_1_0(nullptr), 0x00010002u) );
}

TEST_F( ShellApiTest, shellapi_name_is_ok ) {
	ASSERT_STREQ( "testapi", testApi->name().c_str() );
}

TEST_F( ShellApiTest, shellapi_has_interface ) {
	ASSERT_TRUE( testApi->hasInterface(0,0) );
	ASSERT_TRUE( testApi->hasInterface(1,0) );
	ASSERT_TRUE( testApi->hasInterface(1,1) );
	ASSERT_FALSE( testApi->hasInterface(0,1) );
	ASSERT_FALSE( testApi->hasInterface(2,0) );
}

TEST_F( ShellApiTest, shellapi_get_interface ) {
	ASSERT_TRUE( testApi->getInterface(0,0) != nullptr );
	ASSERT_TRUE( testApi->getInterface(1,0) != nullptr );
	ASSERT_TRUE( testApi->getInterface(1,1) != nullptr );
	ASSERT_EQ( nullptr, testApi->getInterface(0,1) );
	ASSERT_EQ( nullptr, testApi->getInterface(2,0) );
}

TEST_F( ShellApiTest, default_iface_must_be_equal_to_last_iface ) {
	ASSERT_TRUE( testApi->getInterface(0,0) == testApi->getInterface(1,1) );
}
