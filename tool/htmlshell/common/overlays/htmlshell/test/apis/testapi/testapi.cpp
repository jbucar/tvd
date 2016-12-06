#include "testapi.h"
#include "../../util.h"
#include "../../../src/apis/browser/browserapi.h"
#include "../../../src/impl/app/maindelegate.h"
#include "../../../src/impl/browser/browserclient.h"
#include "base/logging.h"
#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace impl {

TestApi_1_0::TestApi_1_0( TestApi *api )
	: _api(api)
{}

TestApi_1_0::~TestApi_1_0()
{
}

void TestApi_1_0::testAssert(bool condition, const std::string &file, int line) {
	if (!condition) {
		LOG(ERROR) << "Assertion failure in library: " << file << ":" << line;
	}
	ASSERT_TRUE( condition );
}

void TestApi_1_0::addTest( TestFn test ) {
	_api->addTest(test);
}

std::string TestApi_1_0::getTestResourcePath( const std::string &dir, const std::string &file ) {
	return util::getTestResourceDir(dir).Append(file).value();
}

std::string TestApi_1_0::getTestProfileDir( const std::string &name ) {
	return util::getTestProfileDir(name).value();
}

void TestApi_1_0::stopShell( int error ) {
	_api->delegate->stop(error);
}

bool TestApi_1_0::emitKeyboardEvent( int id, int keycode, bool isUp ) {
	return static_cast<tvd::BrowserApi*>(_api->delegate->getShellApi(BROWSER_API))->onKeyEvent(id, keycode, isUp);
}

void TestApi_1_0::emitError( int id, const std::string &message, const std::string &source, int line ) {
	static_cast<tvd::BrowserApi*>(_api->delegate->getShellApi(BROWSER_API))->onWebLog(id, {2, line, message, source});
}

void TestApi_1_0::signalBrowserClosed( int id, const std::string &error ) {
	_api->delegate->browserClient()->onSiteClosed(id, error);
}

}

TestApi::TestApi( tvd::MainDelegate *mainDelegate )
	: ShellApi(TEST_API), delegate(mainDelegate)
{}

TestApi::~TestApi()
{}

void TestApi::addTest( TestFn test ) {
	_tests.push_back(test);
}

bool TestApi::init() {
	_iface_10 = new impl::TestApi_1_0(this);
	_iface_11 = new impl::TestApi_1_1(this);
	return true;
}

void TestApi::fin() {
	_tests.clear();
	delete _iface_10;
	delete _iface_11;
	_iface_10 = NULL;
	_iface_11 = NULL;
}

void TestApi::runTests() {
	for (TestFn test : _tests) {
		test();
	}
}

bool TestApi::registerInterfaces() {
	bool result = true;
	result &= callRegisterInterface(_iface_10, 0x00010000u);
	result &= callRegisterInterface(_iface_11, 0x00010001u);
	return result;
}

bool TestApi::callRegisterInterface( void *iface, uint32_t version ) {
	if (!registerInterface(version, iface)) {
		return false;
	}
	return true;
}

uint32_t TestApi::getLastVersion() const {
	return MAKE_API_VERSION(TEST_API_VERSION_MAJOR, TEST_API_VERSION_MINOR);
}
