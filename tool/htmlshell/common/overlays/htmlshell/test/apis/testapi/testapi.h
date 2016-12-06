#pragma once

#include "interface.h"
#include "../../../src/apis/shellapi.h"

class TestApi;

namespace tvd {
class MainDelegate;
}

namespace impl {

class TestApi_1_0 : public TestApiInterface_1_0 {
public:
	explicit TestApi_1_0( TestApi *api );
	virtual ~TestApi_1_0();

	void testAssert(bool condition, const std::string &file, int line) override;
	void addTest(TestFn) override;
	std::string getTestResourcePath( const std::string &dir, const std::string &file ) override;
	std::string getTestProfileDir( const std::string &name ) override;
	void stopShell( int error ) override;
	bool emitKeyboardEvent( int id, int keycode, bool isUp ) override;
	void emitError( int id, const std::string &message, const std::string &source, int line ) override;
	void signalBrowserClosed( int id, const std::string &error ) override;

private:
	TestApi *_api;
};

class TestApi_1_1 : public TestApi_1_0 {
public:
	explicit TestApi_1_1( TestApi *api ) : TestApi_1_0(api) {}
	~TestApi_1_1() override {}
};

}

class TestApi : public tvd::ShellApi {
public:
	explicit TestApi( tvd::MainDelegate *delegate );
	~TestApi() override;

	void runTests();

	// Only for interface comunication
	void addTest( TestFn test );
	bool callRegisterInterface( void *iface, uint32_t version );

	tvd::MainDelegate *delegate;

protected:
	bool init() override;
	void fin() override;

	bool registerInterfaces() override;

	uint32_t getLastVersion() const override;

private:
	std::vector<TestFn> _tests;
	impl::TestApi_1_0 *_iface_10;
	impl::TestApi_1_1 *_iface_11;
};
