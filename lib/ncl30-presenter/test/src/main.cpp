#include "ginga.h"
#include <util/main.h>
#include <gtest/gtest.h>
#include <boost/thread.hpp>

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib,"gtestd.lib")
#	else
#		pragma comment(lib,"gtest.lib")
#	endif // _DEBUG
#endif

void runAllTests( int *result ) {
	*result = RUN_ALL_TESTS();
	ginga::stop();
}

int main( int argc, char **argv ) {
	int result;
	ginga::init();

	testing::InitGoogleTest( &argc, argv );
	boost::thread thread = boost::thread( boost::bind( &runAllTests, &result ) );

	ginga::run();
	thread.join();
	ginga::fin();
	return result;
}
