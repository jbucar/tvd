#include "browserapi.h"
#include "util.h"
#include "../../impl/browser/browserclient.h"
#include "dtv-htmlshellsdk/src/apis/browser.h"
#include "dtv-htmlshellsdk/src/libraryinterface.h"

namespace tvd {

namespace impl {

class BrowserApi_1_0 : public browser::ApiInterface_1_0 {
public:
	explicit BrowserApi_1_0( BrowserApi *api ) : _api(api) {}

	bool launchBrowser( int id, const std::string &url, const browser::Options &opts ) override {
		return _api->launchBrowser(id, url, opts);
	}

	bool closeBrowser( int id ) override {
		return _api->closeBrowser(id);
	}

	bool showBrowser( int id, bool show, bool keyboardFocus ) override {
		return _api->showBrowser(id, show, keyboardFocus);
	}

	void addListener( tvd::browser::Listener *listener ) override {
		_api->addListener(listener);
	}

	void rmListener( tvd::browser::Listener *listener ) override {
		_api->rmListener(listener);
	}

private:
	BrowserApi *_api;
};

}

BrowserApi::BrowserApi( BrowserClient *client )
	: ShellApi(BROWSER_API)
{
	_client = client;
	_iface = NULL;
}

BrowserApi::~BrowserApi()
{
	_client = NULL;
}

bool BrowserApi::init() {
	_iface = new impl::BrowserApi_1_0(this);
	return true;
}

void BrowserApi::fin() {
	_client->closeAllSites();

	delete _iface;
	_iface = NULL;

	if (!_listeners.empty()) {
		LOG(WARNING) << "Not all browser listeners removed! size=" << _listeners.size();
		_listeners.clear();
	}
}

bool BrowserApi::registerInterfaces() {
	// Register interface version 1.0
	if (!registerInterface(MAKE_API_VERSION(1u, 0u), _iface)) {
		return false;
	}
	return true;
}

bool BrowserApi::launchBrowser( int id, const std::string &url, const browser::Options &opts ) {
	if (util::isAllowedUrl(url)) {
		return _client->createAndLoadSite(id, url, opts);
	}

	LOG(ERROR) << "Fail to launch browser! Invalid url: " << url;
	return false;
}

bool BrowserApi::closeBrowser( int id ) {
	return _client->closeSite(id);
}

bool BrowserApi::showBrowser( int id, bool show, bool keyboardFocus ) {
	return _client->showSite(id, show, keyboardFocus);
}

void BrowserApi::addListener( tvd::browser::Listener *listener ) {
	_listeners.push_back(listener);
}

void BrowserApi::rmListener( tvd::browser::Listener *listener ) {
	std::vector<tvd::browser::Listener*>::iterator it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it != _listeners.end()) {
		_listeners.erase(it);
	} else {
		LOG(WARNING) << "Fail to remove browser event listener";
	}
}

uint32_t BrowserApi::getLastVersion() const {
	return MAKE_API_VERSION(BROWSER_API_VERSION_MAJOR, BROWSER_API_VERSION_MINOR);
}

bool BrowserApi::onKeyEvent( int id, int keycode, bool isUp ) {
	bool handled = !_listeners.empty();
	for (tvd::browser::Listener *l : _listeners) {
		handled &= l->onKey(id, keycode, isUp);
	}
	return handled;
}

void BrowserApi::onWebLog( int id, const browser::WebLog &logData ) {
	for (tvd::browser::Listener *l : _listeners) {
		l->onWebLog(id, logData);
	}
}

void BrowserApi::browserLaunched( int id ) {
	for (tvd::browser::Listener *l : _listeners) {
		l->onLaunched(id);
	}
}

void BrowserApi::browserClosed( int id, const std::string &error ) {
	for (tvd::browser::Listener *l : _listeners) {
		l->onClosed(id, error);
	}
}

void BrowserApi::browserLoaded( int id ) {
	for (tvd::browser::Listener *l : _listeners) {
		l->onLoadProgress(id);
	}
}

}
