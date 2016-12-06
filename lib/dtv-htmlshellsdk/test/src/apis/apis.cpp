#include "apis.h"
#include "../../../src/test/mocks.h"

SdkApiTest::SdkApiTest()
{
	_shell = NULL;
}

void SdkApiTest::SetUp() {
	_shell = tvd::test::getHtmlShellWrapperMock();
}

void SdkApiTest::TearDown() {
	tvd::test::destroyHtmlShellWrapperMock();
	_shell = NULL;
}
