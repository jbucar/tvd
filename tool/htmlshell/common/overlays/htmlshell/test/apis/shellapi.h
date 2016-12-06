#pragma once

#include "testing/gtest/include/gtest/gtest.h"

namespace tvd {
class MainDelegate;
}

class TestApi;

class ShellApiTest : public testing::Test {
public:
	ShellApiTest();

protected:
	virtual void SetUp();
	virtual void TearDown();

	tvd::MainDelegate *delegate;
	TestApi *testApi;
};
