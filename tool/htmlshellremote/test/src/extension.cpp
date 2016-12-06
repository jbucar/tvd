#include "extension.h"
#include "../../src/extension.h"
#include "generated/config.h"
#include <htmlshellsdk/test/mocks.h>

MAKE_HTMLSHELL_LIBRARY(remote::Extension, HTMLSHELLREMOTE);

ExtensionTest::ExtensionTest()
{
}

ExtensionTest::~ExtensionTest()
{
}

void ExtensionTest::SetUp() {
	_shell = tvd::test::getHtmlShellWrapperMock();
	ASSERT_TRUE(_shell);
}

void ExtensionTest::TearDown() {
	tvd::test::destroyHtmlShellWrapperMock();
	_shell = NULL;
}

TEST_F( ExtensionTest, constructor ) {
	remote::Extension extension;
}

TEST_F( ExtensionTest, fail_to_initialize_if_shell_dont_has_api ) {
	tvd::LibWrapper *lib = tvd::init(_shell);
	ASSERT_TRUE(lib == NULL);
}

TEST_F( ExtensionTest, fail_to_initialize_if_no_parameters ) {
	tvd::test::createMockApis();
	tvd::LibWrapper *lib = tvd::init(_shell);
	ASSERT_TRUE(lib == NULL);
}

TEST_F( ExtensionTest, initialize_ok ) {
	tvd::test::createMockApis();
	tvd::system::test::setCmdSwitch("tvd-remote-server-url", "pepe");
	tvd::LibWrapper *lib = tvd::init(_shell);
	ASSERT_TRUE(lib != NULL);
	tvd::fin();
}


