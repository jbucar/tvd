#pragma once

#include "../../htmlshell.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace tvd {
class WebApi;
namespace web {
class ApiInterface_1_0;
}
}

class WebApiTest : public testing::Test {
public:
	WebApiTest();

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::WebApi *_webApi;
};

class WebApiInterface_1_0_Test : public WebApiTest {
public:
	WebApiInterface_1_0_Test();

protected:
	void SetUp() override;
	void TearDown() override;

	tvd::web::ApiInterface_1_0 *iface;
};
