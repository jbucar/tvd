#pragma once

#define BROWSER_API "browserapi"
#define BROWSER_API_VERSION_MAJOR 1u
#define BROWSER_API_VERSION_MINOR 0u

#include <string>
#include <vector>

namespace tvd {
namespace browser {

struct BoundsS {
	int x, y, w, h;
};
typedef struct BoundsS Bounds;

struct OptionsS {
	bool transparentBg;
	bool needFocus;
	bool visible;
	bool enableMouse;
	bool enableGeolocation;
	int zIndex;
	Bounds bounds;
	std::string initJS;
	std::string extraUA;
	std::vector<std::string> plugins;
};
typedef struct OptionsS Options;

struct WebLogS {
	int level;
	int line;
	std::string message;
	std::string source;
};
typedef struct WebLogS WebLog;

class Listener {
public:
	virtual ~Listener() {}

	virtual void onLaunched( int /*id*/ ) {}
	virtual void onClosed( int /*id*/, const std::string &/*error*/ ) {}
	virtual bool onKey( int /*id*/, int /*keycode*/, bool /*isUp*/ ) { return false; }
	virtual void onWebLog( int /*id*/, const WebLog &/*logData*/ ) {}
	virtual void onLoadProgress( int /*id*/ ) {}
};

template<typename T>
class ListenerForwarder : public Listener {
public:
	explicit ListenerForwarder( T *obj ) : _obj(obj) {}

	virtual void onLaunched( int id ) {
		_obj->onLaunched(id);
	}

	virtual void onClosed( int id, const std::string &error ) {
		_obj->onClosed(id, error);
	}

	virtual bool onKey( int id, int keycode, bool isUp ) {
		return _obj->onKey(id, keycode, isUp);
	}

	virtual void onWebLog( int id, const WebLog &logData ) {
		_obj->onWebLog(id, logData);
	}

	virtual void onLoadProgress( int id ) {
		_obj->onLoadProgress(id);
	}

private:
	T *_obj;
};

class ApiInterface_1_0 {
public:
	virtual ~ApiInterface_1_0() {}

	virtual bool launchBrowser( int id, const std::string &url, const Options &opts )=0;
	virtual bool closeBrowser( int id )=0;
	virtual bool showBrowser( int id, bool show, bool keyboardFocus )=0;

	// Browser events
	virtual void addListener( Listener *listener )=0;
	virtual void rmListener( Listener *listener )=0;
};

}
}
