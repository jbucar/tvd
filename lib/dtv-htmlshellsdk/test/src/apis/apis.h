#pragma once

#include <gtest/gtest.h>

namespace tvd {
class HtmlShellWrapper;
}

class SdkApiTest : public testing::Test {
public:
	SdkApiTest();

protected:
	virtual void SetUp();
	virtual void TearDown();

	tvd::HtmlShellWrapper *_shell;
};
