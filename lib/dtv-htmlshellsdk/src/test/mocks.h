#pragma once

#include "../libraryinterface.h"
#include "../apis/web.h"
#include "../apis/browser.h"
#include <string>
#include <vector>

// Keep synched with src/errors.h
#define HTMLSHELL_EXTERNAL_CODE_BASE 15

namespace tvd {
namespace test {

class MockApi {
public:
	MockApi( std::string pName, uint32_t pMajor, uint32_t pMinor, void *pMock )
		: _name(pName), _major(pMajor), _minor(pMinor), _mock(pMock) {}

	std::string _name;
	uint32_t _major;
	uint32_t _minor;
	void *_mock;
};

class HtmlShellWrapperImpl : public HtmlShellWrapper {
public:
	virtual ~HtmlShellWrapperImpl() {}

	virtual bool hasApi( const std::string &name, uint32_t major, uint32_t minor );

	void clearApis();
	void addApi( const MockApi &api );

protected:
	virtual void *getApiImpl( const std::string &name, uint32_t major, uint32_t minor );

private:
	std::vector<MockApi> _apis;
};

HtmlShellWrapper *getHtmlShellWrapperMock();
void destroyHtmlShellWrapperMock();
void createMockApis();

}

// Testing helpers
namespace system {
namespace test {
typedef void (*OnShutdown)( int );
void onShutdown( OnShutdown cb );
void setCmdSwitch( const std::string &name, const std::string &value );
}
}

namespace web {
namespace test {
typedef boost::function<bool (const web::Response &response)> ExecuteCallback;
void onExecuteCallback( ExecuteCallback cb );
bool simulateApiCall( const std::string &api, const web::Request &req );
}
}

namespace browser {
namespace test {
typedef boost::function<void (int id, const std::string &url, const Options &opts)> OnLaunchBrowserRequest;
void onLaunchBrowserRequest( OnLaunchBrowserRequest cb );
bool emitKeyboardEvent( int id, int keycode, bool isUp );
void emitWebLog( int id, const browser::WebLog &logData );
void emitLoaded( int id );
void setOnCloseError( const std::string &errorStr );
bool isHidden( int id );
}
}

}
