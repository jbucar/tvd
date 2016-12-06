#include "htmlshell.h"
#include "util.h"
#include "apis/testapi/testapi.h"
#include "../src/library.h"

TEST( LibraryTest, make_version_test ) {
	ASSERT_EQ( 0x00000000u, MAKE_API_VERSION(0,0) );
	ASSERT_EQ( 0x00000001u, MAKE_API_VERSION(0,1) );
	ASSERT_EQ( 0x0000FFFFu, MAKE_API_VERSION(0,65535) );
	ASSERT_EQ( 0x00010000u, MAKE_API_VERSION(1,0) );
	ASSERT_EQ( 0x00010001u, MAKE_API_VERSION(1,1) );
	ASSERT_EQ( 0x0001FFFFu, MAKE_API_VERSION(1,65535) );
	ASSERT_EQ( 0xFFFF0000u, MAKE_API_VERSION(65535,0) );
	ASSERT_EQ( 0xFFFF0001u, MAKE_API_VERSION(65535,1) );
	ASSERT_EQ( 0xFFFFFFFFu, MAKE_API_VERSION(65535,65535) );
}

TEST( LibraryTest, get_major_version ) {
	ASSERT_EQ( 0x00000000u, GET_MAJOR_VERSION(0x0000ABCDu) );
	ASSERT_EQ( 0x00000001u, GET_MAJOR_VERSION(0x0001ABCDu) );
	ASSERT_EQ( 0x0000FFFFu, GET_MAJOR_VERSION(0xFFFFABCDu) );
}

TEST( LibraryTest, get_minor_version ) {
	ASSERT_EQ( 0x00000000u, GET_MINOR_VERSION(0xABCD0000u) );
	ASSERT_EQ( 0x00000001u, GET_MINOR_VERSION(0xABCD0001u) );
	ASSERT_EQ( 0x0000FFFFu, GET_MINOR_VERSION(0xABCDFFFFu) );
}

TEST( LibraryTest, create_library_from_invalid_path_should_fail ) {
	tvd::Library *lib = tvd::Library::createFromPath(util::getTestDir("launcher").Append("libs").Append("libinvalid.so"));
	ASSERT_EQ( nullptr, lib );
}

TEST( LibraryTest, library_init_successfully ) {
	tvd::Library *lib = tvd::Library::createFromPath(util::getTestDir("launcher").Append("libs").Append("libtestinitsucceed.so"));
	ASSERT_TRUE( lib != nullptr );
	tvd::HtmlShellWrapper wrapper;
	ASSERT_TRUE(lib->init(&wrapper));
	lib->fin();
	delete lib;
}

TEST( LibraryTest, library_init_fail ) {
	tvd::Library *lib = tvd::Library::createFromPath(util::getTestDir("launcher").Append("libs").Append("libtestinitfail.so"));
	ASSERT_TRUE( lib != nullptr );
	tvd::HtmlShellWrapper wrapper;
	ASSERT_FALSE(lib->init(&wrapper));
	delete lib;
}

TEST( LibraryTest, shell_and_library_comunicate_succesfully_throw_api ) {
	test::FakeMainDelegate delegate;
	TestApi testApi(&delegate);
	ASSERT_TRUE(delegate.addApi(&testApi));

	tvd::Library *lib = tvd::Library::createFromPath(util::getTestDir("launcher").Append("libs").Append("libtestinteraction.so"));
	ASSERT_TRUE( lib != nullptr );
	ASSERT_TRUE( lib->init(&delegate) );

	ASSERT_STREQ( "Interaction", lib->name().c_str() );
	ASSERT_STREQ( "1.0", lib->version().c_str() );

	testApi.runTests();

	lib->fin();
	delete lib;

	testApi.finalize();
}
