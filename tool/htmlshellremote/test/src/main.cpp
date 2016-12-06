#include <gtest/gtest.h>

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"gtestd.lib")
#	else
#		pragma comment(lib,"gtest.lib")
#	endif // _DEBUG
#endif

int main( int argc, char **argv ) {
	testing::InitGoogleTest( &argc, argv );
	int result = RUN_ALL_TESTS();
	return result;
}

