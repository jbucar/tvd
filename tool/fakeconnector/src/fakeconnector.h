#pragma once

#include <util/keydefs.h>
#include <util/tool.h>
#include <canvas/types.h>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/signals2.hpp>
#include <set>
#include <string>

namespace fs = boost::filesystem;

typedef struct lua_State lua_State;

namespace util {
	class Buffer;
}

namespace canvas {
	class System;
	namespace remote {
		class ServerImpl;
	}
	namespace event {
		union NotifyStruct;
		typedef union NotifyStruct Notify;
	}
}

namespace connector {
	class Connector;

	template<typename T> class AVDescriptor;
	typedef AVDescriptor<int> NCLVideoDescriptor;
	typedef boost::shared_ptr<NCLVideoDescriptor> NCLVideoDescriptorPtr;

	typedef std::set<util::key::type> KeyRegisterData;
	typedef boost::shared_ptr<KeyRegisterData> KeyRegisterDataPtr;
}

class FakeConnector : public util::Tool {
public:
	FakeConnector();
	virtual ~FakeConnector();

	//	Lua methods
	void loop();
	void exit();
	void sendEditingCommand(int commandtag, int eventId, const unsigned char *payload, const int payloadSize);
	void sendKey( util::key::type key );

protected:
	virtual int run( util::cfg::cmd::CommandLine &cmd );
	void run( const std::string &script, const std::string &pipe );

	//Configuration
	virtual void registerProperties( util::cfg::cmd::CommandLine &cmd );

	//	Connector callbacks
	void onConnect( bool isConnected );
	void onGingaStarted( bool isRunning );
	void onPlayVideoEvent( size_t videoID, const std::string &url );
	void onStopVideoEvent( size_t videoID );
	void onResizeVideoEvent( size_t videoID, const connector::NCLVideoDescriptorPtr &av );
	void onKeyEvent( const connector::KeyRegisterDataPtr &data );
	void sendKey( util::key::type key, bool isUP );
	void sendBtn( canvas::input::ButtonEvent *evt );

	//	Initialization
	bool doInit( const std::string &pipe );
	void finalize();
	void runScript( const std::string &script );
	void onKey( util::key::type value, bool isUp );

private:
	canvas::System *_sys;
	canvas::remote::ServerImpl *_remote;
	lua_State *_lua;
	connector::Connector *_connector;
	std::set<util::key::type> _reservedKeys;
	boost::signals2::connection _onButton;
};
