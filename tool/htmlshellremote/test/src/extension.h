#pragma once

#include <gtest/gtest.h>

namespace remote {
	class Extension;
}

namespace tvd {
	class HtmlShellWrapper;
}

class ExtensionTest : public testing::Test {
public:
	ExtensionTest();
	virtual ~ExtensionTest();

protected:
	virtual void SetUp();
	virtual void TearDown();

	tvd::HtmlShellWrapper *_shell;
};

