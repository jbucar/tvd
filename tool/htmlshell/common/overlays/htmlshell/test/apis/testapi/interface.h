#pragma once

#define TEST_API "testapi"
#define TEST_API_VERSION_MAJOR 1u
#define TEST_API_VERSION_MINOR 1u

#include <string>

// To use this macros must exist a pointer to an instance of TestApiInterface named test
#define TESTAPI_ASSERT_TRUE( c ) test->testAssert(c, __FILE__, __LINE__)
#define TESTAPI_ASSERT_FALSE( c ) TESTAPI_ASSERT_TRUE( !c )
#define TESTAPI_ASSERT_EQ( a, b ) TESTAPI_ASSERT_TRUE( a == b )

typedef void (*TestFn)();

class TestApiInterface_1_0 {
public:
	virtual void testAssert(bool condition, const std::string &file, int line)=0;
	virtual void addTest(TestFn)=0;
	virtual std::string getTestResourcePath( const std::string &dir, const std::string &file )=0;
	virtual std::string getTestProfileDir( const std::string &name )=0;
	virtual void stopShell( int error )=0;
	virtual bool emitKeyboardEvent( int id, int keycode, bool isUp )=0;
	virtual void emitError( int id, const std::string &message, const std::string &source, int line )=0;
	virtual void signalBrowserClosed( int id, const std::string &error )=0;
};
