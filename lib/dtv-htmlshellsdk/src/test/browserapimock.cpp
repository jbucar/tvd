#include "../apis/browser.h"
#include "mocks.h"
#include <cstdio>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <boost/foreach.hpp>

namespace tvd {
namespace browser {
namespace test {

namespace impl {

static bool isScheme( const std::string &url, const std::string &scheme ) {
	return url.substr(0, scheme.length()) == scheme;
}

static bool fileExists( const std::string &filename ) {
	if (filename.empty()) {
		return false;
	}

	if (FILE *file = fopen(filename.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}
}

static bool isAllowedUrl( const std::string &url ) {
	if (url.find("://") != std::string::npos) {
		if (isScheme(url,"https://")) {
			return url.length() > 8;
		} else if (isScheme(url,"http://")) {
			return url.length() > 7;
		}
		else if (isScheme(url,"file://")) {
			return fileExists(url.substr(7));
		}
		else if (isScheme(url,"filesystem://")) {
			return fileExists(url.substr(13));
		}
	}
	return false;
}

class BrowserApiMock : public browser::ApiInterface_1_0 {
public:
	BrowserApiMock() {
		_onLaunchBrowserRequest = NULL;
		_closeError = "";
	}

	virtual ~BrowserApiMock() {}

	virtual bool launchBrowser( int id, const std::string &url, const browser::Options &opts ) {
		bool result = false;
		if (isAllowedUrl(url)) {
			result = _openBrowsers.insert(std::make_pair(id,true)).second;
			if (result) {
				BOOST_FOREACH( tvd::browser::Listener *bListener, _listeners ) {
					bListener->onLaunched(id);
				}
			}
			if (_onLaunchBrowserRequest) {
				_onLaunchBrowserRequest(id, url, opts);
			}
		}
		return result;
	}

	virtual bool closeBrowser( int id ) {
		bool result = _openBrowsers.erase(id);
		if (result) {
			BOOST_FOREACH( tvd::browser::Listener *bListener, _listeners ) {
				bListener->onClosed(id, _closeError);
			}
		}
		return result;
	}

	virtual void addListener( Listener *listener ) {
		_listeners.push_back(listener);
	}

	virtual void rmListener( Listener *listener ) {
		std::vector<tvd::browser::Listener*>::iterator it = std::find(_listeners.begin(), _listeners.end(), listener);
		if (it != _listeners.end()) {
			_listeners.erase(it);
		}
	}

	virtual bool showBrowser( int id, bool show, bool /*keyboardFocus*/ ) {
		std::map<int, bool>::iterator it = _openBrowsers.find(id);
		if (it != _openBrowsers.end()) {
			it->second = show;
			return true;
		}
		return false;
	}

	std::vector<tvd::browser::Listener*> _listeners;
	std::map<int, bool> _openBrowsers;
	OnLaunchBrowserRequest _onLaunchBrowserRequest;
	std::string _closeError;
};

static BrowserApiMock *mockIface = NULL;

}

void *getMockInterface() {
	if (!impl::mockIface) {
		impl::mockIface = new impl::BrowserApiMock();
	}
	return impl::mockIface;
}

void destroyMockInterface() {
	if (impl::mockIface) {
		delete impl::mockIface;
		impl::mockIface = NULL;
	}
}

bool emitKeyboardEvent( int id, int keycode, bool isUp ) {
	bool handled = true;
	BOOST_FOREACH( tvd::browser::Listener *bListener, impl::mockIface->_listeners ) {
		handled &= bListener->onKey(id, keycode, isUp);
	}
	return handled;
}

void emitWebLog( int id, const browser::WebLog &logData ) {
	BOOST_FOREACH( tvd::browser::Listener *bListener, impl::mockIface->_listeners ) {
		bListener->onWebLog(id, logData);
	}
}

void onLaunchBrowserRequest( OnLaunchBrowserRequest cb ) {
	impl::mockIface->_onLaunchBrowserRequest = cb;
}

void setOnCloseError( const std::string &errorStr ) {
	impl::mockIface->_closeError = errorStr;
}

bool isHidden( int id ) {
	std::map<int, bool>::const_iterator it = impl::mockIface->_openBrowsers.find(id);
	return (it != impl::mockIface->_openBrowsers.end()) && !it->second;
}

void emitLoaded( int id ) {
	BOOST_FOREACH( tvd::browser::Listener *bListener, impl::mockIface->_listeners ) {
		bListener->onLoadProgress(id);
	}
}

}
}
}
