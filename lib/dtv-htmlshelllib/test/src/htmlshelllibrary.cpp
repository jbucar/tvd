#include "../../src/library.h"
#include <util/log.h>
#include <util/mcr.h>
#include <htmlshellsdk/libraryinterface.h>
#include <htmlshellsdk/test/mocks.h>
#include <generated/config.h>
#include <gtest/gtest.h>

MAKE_HTMLSHELL_LIBRARY(tvd::HtmlShellLibrary, HTMLSHELLLIB);

class HtmlShellLibTest : public testing::Test {
public:
	HtmlShellLibTest() {
		_shell = NULL;
	}

protected:
	virtual void SetUp() {
		_shell = tvd::test::getHtmlShellWrapperMock();
		util::log::setLevel("htmlshelllib", "all", "all");
	}
	virtual void TearDown() {
		tvd::test::destroyHtmlShellWrapperMock();
		_shell = NULL;
	}

	tvd::HtmlShellWrapper *_shell;
};

TEST_F( HtmlShellLibTest, lib_initialized_successfully ) {
	tvd::LibWrapper *wrapper = tvd::init(_shell);
	ASSERT_TRUE( wrapper != NULL );
	ASSERT_STREQ( HTMLSHELLLIB_NAME, wrapper->name().c_str() );
	ASSERT_STREQ( HTMLSHELLLIB_VERSION, wrapper->version().c_str() );
	tvd::fin();
}

TEST_F( HtmlShellLibTest, lib_fail_to_initialize_twice ) {
	ASSERT_TRUE( tvd::init(_shell) != NULL );
	ASSERT_DEATH( tvd::init(_shell), "" );
	tvd::fin();
}

TEST_F( HtmlShellLibTest, lib_fail_to_finalize_if_not_initialized ) {
	ASSERT_DEATH( tvd::fin(), "" );
}

TEST_F( HtmlShellLibTest, lib_fail_to_finalize_twice ) {
	ASSERT_TRUE( tvd::init(_shell) != NULL );
	tvd::fin();
	ASSERT_DEATH( tvd::fin(), "" );
}

TEST_F( HtmlShellLibTest, lib_init_fin_init_succedd ) {
	ASSERT_TRUE( tvd::init(_shell) != NULL );
	tvd::fin();
	ASSERT_TRUE( tvd::init(_shell) != NULL );
	tvd::fin();
}
