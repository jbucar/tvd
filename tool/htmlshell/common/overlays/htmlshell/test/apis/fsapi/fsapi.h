#pragma once

#include "testing/gtest/include/gtest/gtest.h"

namespace tvd {
class FsApi;
}

class FsApiTest : public testing::Test {
public:
	FsApiTest();
	~FsApiTest() override;

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::FsApi *fsApi;
};
