#include <gtest/gtest.h>

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"gtestd.lib")
#	else
#		pragma comment(lib,"gtest.lib")
#	endif // _DEBUG
#endif

int main( int argc, char **argv ) {
	::testing::FLAGS_gtest_death_test_style = "threadsafe";
	int result;
	testing::InitGoogleTest( &argc, argv );
	result = RUN_ALL_TESTS();
	return result;
}
