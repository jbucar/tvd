#pragma once

#include "../shellapi.h"
#include "dtv-htmlshellsdk/src/apis/browser.h"
#include <map>

namespace tvd {

class BrowserClient;

namespace impl {
class BrowserApi_1_0;
}

class BrowserApi : public ShellApi {
public:
	explicit BrowserApi( BrowserClient *client );
	virtual ~BrowserApi();

	bool launchBrowser( int id, const std::string &url, const browser::Options &opts );
	bool closeBrowser( int id );
	bool showBrowser( int id, bool show, bool keyboardFocus );
	void addListener( tvd::browser::Listener *listener );
	void rmListener( tvd::browser::Listener *listener );

	// Events
	void browserLaunched( int id );
	void browserClosed( int id, const std::string &error );
	void browserLoaded( int id );
	bool onKeyEvent( int id, int keycode, bool isUp );
	void onWebLog( int id, const browser::WebLog &logData );

protected:
	bool init() override;
	void fin() override;

	bool registerInterfaces() override;
	uint32_t getLastVersion() const override;

private:
	BrowserClient *_client;
	impl::BrowserApi_1_0 *_iface;
	std::vector<tvd::browser::Listener*> _listeners;
};

}
