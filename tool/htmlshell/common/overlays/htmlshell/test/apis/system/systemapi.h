#pragma once

#include "dtv-htmlshellsdk/src/apis/system.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace tvd {
class SystemApi;
}

namespace test {
class HtmlShellMock;
}

class SystemApiTest : public testing::Test {
public:
	SystemApiTest();
	~SystemApiTest() override;

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::SystemApi *_api;
	test::HtmlShellMock *_shell;
};

class SystemApiInterface_1_0_Test : public SystemApiTest {
public:
	SystemApiInterface_1_0_Test();

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::system::ApiInterface_1_0 *iface;
};
