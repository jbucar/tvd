#include "fakeconnector.h"
#include "lua/connector.h"
#include "generated/config.h"
#include <canvas/remote/serverimpl.h>
#include <canvas/inputmanager.h>
#include <canvas/canvas.h>
#include <canvas/window.h>
#include <canvas/system.h>
#include <canvas/factory.h>
#include <connector/connector.h>
#include <connector/handler/editingcommandhandler.h>
#include <connector/handler/keyeventhandler.h>
#include <connector/handler/keyregisterhandler.h>
#include <connector/handler/buttoneventhandler.h>
#include <connector/handler/videoresizehandler.h>
#include <connector/handler/playvideohandler.h>
#include <connector/handler/stopvideohandler.h>
#include <connector/handler/exitcommandhandler.h>
#include <connector/handler/startcommandhandler.h>
#include <util/task/dispatcherimpl.h>
#include <util/cfg/cfg.h>
#include <util/cfg/cmd/commandline.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

CREATE_TOOL(FakeConnector,FAKECONNECTOR);

namespace fs = boost::filesystem;

FakeConnector::FakeConnector() {
	_sys = NULL;
	_remote = NULL;
	_lua = NULL;
}

FakeConnector::~FakeConnector()
{
}

int FakeConnector::run( util::cfg::cmd::CommandLine & /*cmd*/ ) {
	fs::path url( getCfg<std::string> ("luascript") );

	//	Set windows title
	util::cfg::setValue("gui.window.title", "FakeConnector");

	if (!url.is_absolute()) {
		url = fs::current_path() / url;
	}

	if (exists(url) && is_regular_file(url)) {
		run( getCfg<std::string> ("luascript"), getCfg<std::string> ("connector") );
	} else {
		LWARN("FakeConnector", "invalid lua script file path: url=%s", url.string().c_str());
	}
	return 0;
}

void FakeConnector::registerProperties( util::cfg::cmd::CommandLine &cmd ) {
	Tool::registerProperties( cmd );
	registerProp("luascript", 'l', "Start lua script", std::string(""), cmd);
	registerProp("connector", 'c', "Start connector using arg as pipe.", std::string(DEFAULT_CONNECTOR_FILE), cmd);
}

void FakeConnector::run( const std::string &script, const std::string &pipe ) {
	if (doInit( pipe )) {
		//	Run lua script
		runScript( script );
		finalize();
	}
}

bool FakeConnector::doInit( const std::string &pipe ) {
	LINFO("FakeConnector", "doInit: pipe=%s\n", pipe.c_str());

	//	Create canvas factory
	canvas::Factory *factory = new canvas::Factory();

	//	Setup local system
	_sys = factory->createSystem();
	if (!_sys) {
		delete factory;
		LERROR("FakeConnector", "cannot create system");
		return false;
	}

	//	Setup system
	util::cfg::setValue("gui.window.title", "FakeConnector");
	if (!_sys->initialize( factory )) {
		LERROR("FakeConnector", "cannot initialize system");
		return false;
	}

	//	Setup keys
	_sys->input()->onKey().connect( boost::bind(&FakeConnector::onKey,this,_1,_2) );

	//	Setup Btn
	_onButton = _sys->input()->onButton().connect( boost::bind(&FakeConnector::sendBtn,this,_1) );

	//	Setup remote canvas
	_remote = new canvas::remote::ServerImpl(_sys);
	if (!_remote->initialize( canvas::Size(720,576), 2 )) {
		LERROR("FakeConnector", "cannot initialize system");
		return false;
	}
	_remote->enable(true);

	//	Add into dispatcher
	_sys->dispatcher()->registerTarget( this, "Fakeconnector" );

	//	Setup connector
	_connector = new connector::Connector( pipe, true );
	_connector->setDispatcher( boost::bind(&util::task::Dispatcher::post,_sys->dispatcher(),this,_1) );

	//	Handle connect
	_connector->onConnect( boost::bind(&FakeConnector::onConnect,this,true) );

	//	Handle error cases
	boost::function<void (void)> fnc = boost::bind(&FakeConnector::onConnect,this,false);
	_connector->onConnectTimeout( fnc );
	_connector->onDisconnect( fnc );
	_connector->onTimeout( fnc );

	{       //	Reserve/Release keys messages
		connector::KeyRegisterHandler *keyHandler = new connector::KeyRegisterHandler();
		keyHandler->onKeyEvent( boost::bind( &FakeConnector::onKeyEvent,this,_1) );
		_connector->addHandler( keyHandler );
	}

	{	//	Play video
		connector::PlayVideoHandler *handler = new connector::PlayVideoHandler();
		handler->onPlayVideoEvent( boost::bind(&FakeConnector::onPlayVideoEvent,this,_1,_2) );
		_connector->addHandler( handler );
	}

	{	//	Stop video
		connector::StopVideoHandler *handler = new connector::StopVideoHandler();
		handler->onStopVideoEvent( boost::bind(&FakeConnector::onStopVideoEvent,this,_1) );
		_connector->addHandler( handler );
	}

	{	//	Video resize
		connector::VideoResizeHandler<int> *videoResizeHandler = new connector::VideoResizeHandler<int>();
		videoResizeHandler->onVideoResizeEvent( boost::bind(&FakeConnector::onResizeVideoEvent,this,_1,_2) );
		_connector->addHandler( videoResizeHandler );
	}

	{       //	Setup start presentation
		connector::StartCommandHandler *handler = new connector::StartCommandHandler();
		handler->onStartPresentationEvent( boost::bind( &FakeConnector::onGingaStarted, this, _1 ) );
		_connector->addHandler( handler );
	}

	return true;
}

void FakeConnector::finalize() {
	connector::ExitCommandHandler exit;
	exit.send( _connector );

	//	Remove from dispatcher
	_sys->dispatcher()->unregisterTarget( this );
	_onButton.disconnect();

	lua::fin();
	DEL(_connector);

	lua_close(_lua);
	_lua = NULL;

	if (_remote) {
		_remote->finalize();
		DEL(_remote);
	}

	if (_sys) {
		_sys->finalize();
		DEL(_sys);
	}
}

void FakeConnector::runScript( const std::string &script ) {
	LINFO("FakeConnector", "Running script %s", script.c_str());
	_lua = lua_open();
	lua::init( _lua, this );
	if (luaL_dofile( _lua, script.c_str() )) {
		int top = lua_gettop(_lua);
		LINFO("FakeConnector", "LUA: stack dump (%d): %s", top, lua_tostring(_lua, -1));
	}
	lua::fin();
}

void FakeConnector::loop() {
	_sys->run();
}

void FakeConnector::exit() {
	_sys->exit();
}

void FakeConnector::sendKey( util::key::type key ) {
	if (key > util::key::null && key < util::key::LAST) {
		sendKey( key, false );
		sendKey( key, true );
	}
}

void FakeConnector::sendEditingCommand(int commandtag, int eventId, const unsigned char *payload, const int payloadSize) {
	connector::EditingCommandHandler ecdh;
	connector::EditingCommandData ecd((unsigned char)commandtag);
	ecd.setEventID(eventId);
	ecd.addPayload(payload, payloadSize);
	ecdh.send(_connector, &ecd);
}

void FakeConnector::onConnect( bool isConnected ) {
	LINFO("FakeConnector", "Connector state changed: isConnected=%d", isConnected);
	lua_getfield( _lua, LUA_GLOBALSINDEX, "OnConnected" );
	lua_pushnumber( _lua, isConnected ? 1 : 0 );
	lua_call( _lua, 1, 0 );
}

void FakeConnector::onGingaStarted( bool isRunning ) {
	LINFO("FakeConnector", "Ginga presentation changed: isRunning=%d", isRunning);
	lua_getfield( _lua, LUA_GLOBALSINDEX, "OnGinga" );
	lua_pushnumber( _lua, isRunning ? 1 : 0 );
	lua_call( _lua, 1, 0 );
}

void FakeConnector::onPlayVideoEvent( size_t videoID, const std::string &url ) {
	LINFO("FakeConnector", "Play video: videoID=%d, url=%s", videoID, url.c_str());
}

void FakeConnector::onStopVideoEvent( size_t videoID ) {
	LINFO("FakeConnector", "Stop video: videoID=%d", videoID);
}

void FakeConnector::onResizeVideoEvent( size_t videoID, const connector::NCLVideoDescriptorPtr & /*av*/ ) {
	LINFO("FakeConnector", "Resize video: videoID=%d", videoID);
}

void FakeConnector::onKeyEvent( const connector::KeyRegisterDataPtr &data ) {
	LINFO("FakeConnector", "On reserve keys: keys=%d", data->size());
	BOOST_FOREACH( util::key::type key, *data.get() ) {
		_reservedKeys.insert( key );
	}
}

void FakeConnector::sendKey( util::key::type key, bool isUp ) {
	connector::KeyEventHandler handler;
	handler.send( _connector, key, isUp );
}

void FakeConnector::sendBtn( ::canvas::input::ButtonEvent *evt ) {
	connector::ButtonEventHandler handler;
	handler.send( _connector, evt->button, evt->x, evt->y, evt->isPress );
}

//	Canvas keys
void FakeConnector::onKey( util::key::type value, bool isUp ) {
	if (_reservedKeys.count( value ) > 0) {
		sendKey( value, isUp );
	} else if (value != util::key::null && !isUp) {
		lua_getfield( _lua, LUA_GLOBALSINDEX, "OnKeyEvent" );
		lua_pushnumber( _lua, value );
		lua_call( _lua, 1, 0 );
	}
}

