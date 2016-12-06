/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-gingaplayer implementation.

    DTV-gingaplayer is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-gingaplayer is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-gingaplayer.

    DTV-gingaplayer es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-gingaplayer se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "../lua.h"
#include "../../luaplayer.h"
#include "../../../system.h"
#include "class/key.h"
#include "class/pointer.h"
#include "class/ncl.h"
#include "class/user.h"
#include "class/tcp.h"
#include "class/si.h"
#include <canvas/system.h>
#include <util/net/socket.h>
#include <util/net/ipv4/sockaddr.h>
#include <util/net/resolver.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#define LUA_EVENT "lua_module_event"

namespace player {
namespace event {

struct FindTimerByID {
	FindTimerByID( util::id::Ident &timerID ) : _timerID(timerID) {}

	bool operator()( const Module::TimerRef &timer ) const {
		return timer.first == _timerID;
	}

	util::id::Ident &_timerID;

private:
	// Avoid VisualStudio warning C4512
	FindTimerByID &operator=( FindTimerByID & /*ft*/ ) { return *this; }
};

struct FindTimerByRef {
	FindTimerByRef( int id ) : _id(id) {}

	bool operator()( const Module::TimerRef &timer ) const {
		return timer.second == _id;
	}

	int _id;
};

struct FindSocketByID {
	FindSocketByID( util::id::ID_TYPE id ) : _id(id) {}

	bool operator()( const Module::SocketList::value_type &pair ) const {
		util::id::Ident id = pair.first;
		return util::id::isValid(id) ? (id->getID() == _id) : false;
	}

	util::id::ID_TYPE _id;

private:
	// Avoid VisualStudio warning C4512
	FindSocketByID &operator=( FindSocketByID & /*ft*/ ) { return *this; }
};

class Handler {
public:
	Handler( lua_State *lua, int ref, const std::string &className="any" )
		: _lua(lua), _method( ref ), _className(className) {}

	virtual ~Handler() {
		luaL_unref(_lua, LUA_REGISTRYINDEX, _method);
	}

	bool isEqual( int index ) {
		//	Get stored function
		lua_rawgeti(_lua, LUA_REGISTRYINDEX, _method);
		//	Compare with parameter
		return lua_equal( _lua, index, -1 ) == 1;
	}

	bool dispatch( EventImpl *table ) {
		bool result=false;
		const std::string &className = (*table)["class"];
		if (_className == "any" || _className == className) {
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, _method);

			lua_newtable(_lua);
			BOOST_FOREACH( const EventImpl::value_type &value, *table ) {
				lua_pushstring( _lua, value.second.c_str() );
				lua_setfield( _lua, -2, value.first.c_str() );
			}

			lua_call( _lua, 1, 1 );
			result=lua_toboolean( _lua, -1 ) == 1;
		}
		return result;
	}

private:
	lua_State *_lua;
	int _method;
	std::string _className;
};

/*******************************************************************************
*	Event module
*******************************************************************************/
static int l_post( lua_State *L ) {
	//	(a) evt: event -> sent: boolean; err_msg: string
	//	(b) dst:string, evt: event -> sent: boolean; err_msg: string
	//	dst = [in | out]; default=out

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check dst
	bool dstIsOut=true;
	int eventPos=1;
	if (lua_gettop(L) == 2) {
		const char *dst = luaL_checkstring(L, 1);
		if (!dst) {
			return luaL_error(L, "[player::event] Invalid dst");
		}

		if (!strcmp("out",dst)) {
			dstIsOut=true;
		}
		else if (!strcmp( "in", dst )) {
			dstIsOut=false;
		}
		else {
			return luaL_error(L, "[player::event] Invalid dst");
		}

		eventPos=2;
	}

	//	Get class event
	luaL_checktype(L, eventPos, LUA_TTABLE);
	const char *value = lua::getField( L, eventPos, "class" );
	if (!value) {
		return luaL_error(L, "[player::event] Event class nil" );
	}

	return module->postEvent( value, dstIsOut, eventPos );
}

static int l_cancel( lua_State *L ) {
	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return (int) luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Get timerID
	int timerRef = (int) lua_tonumber(L,lua_upvalueindex(1));

	module->cancelTimer( timerRef );
	return 0;
}

static int l_timer( lua_State *L ) {
	// event.timer( time: number, f: function ) -> cancel: function

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check ms value
	util::DWORD ms = (util::DWORD) luaL_checknumber( L, 1 );

	//	Check function and save in the registry
	luaL_checktype( L, 2, LUA_TFUNCTION );	//	(f)

	//	Register system timer
	module->registerTimer( ms );
	return 1;
}

static int l_uptime( lua_State *L ) {	//	TODO: Get uptime from player
	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	util::DWORD ms = module->uptime();
	lua_pushnumber(L, ms);
	return 1;
}

static int l_register( lua_State *L ) {
	//	event.register( [pos:number]; f:function; [class:string; [...:any]] )

	std::string classFilter="any";
	int index=1;

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Get pos
	int pos=-1;
	if (lua_type(L, index) == LUA_TNUMBER) {
		pos = (int) luaL_checknumber( L, index );
		if (pos <= 0) {
			return luaL_argerror(L, 1, NULL);
		}
		index++;
	}

	//	Get function
	luaL_checktype(L, index, LUA_TFUNCTION);
	int fncIndex=index;
	index++;

	//	Have class filter?
	if (lua_gettop(L) >= index)	{
		//	Get class filter
		classFilter=luaL_checkstring( L, index );
		index++;
		//	Have class dependent filter?
		if (lua_gettop(L) >= index) {
			return luaL_error(L, "Class dependent filter not supported");
		}
	}

	//	Add event handler
	return module->addHandler( pos, fncIndex, classFilter );
}

static int l_unregister( lua_State *L ) {
	//	event.unregister( f:function )

	//	Get event module from stack
	Module *module = Module::get( L );
	if (!module) {
		return luaL_error( L, "[player::event] Invalid event module" );
	}

	//	Check function
	luaL_checktype( L, 1, LUA_TFUNCTION );	//	(f)

	//	Remove event handler
	module->removeHandler( 1 );
	return 0;
}

static const struct luaL_Reg event_methods[] = {
	{ "post",       l_post       },
	{ "timer",      l_timer      },
	{ "uptime",     l_uptime     },
	{ "register",   l_register   },
	{ "unregister", l_unregister },
	{ NULL,         NULL         }
};


/*******************************************************************************
*	Class Module
*******************************************************************************/
Module::Module( System *sys, LuaPlayer *player, lua_State *lua )
	: _sys(sys), _player(player), _lua( lua )
{
	//	Store module into stack
	lua::storeObject( lua, this, LUA_EVENT );

	//	Register classes
	_classes["ncl"]  = &ncl::postEvent;
	_classes["key"]  = &key::postEvent;
	_classes["pointer"]  = &pointer::postEvent;
	_classes["user"] = &user::postEvent;
	_classes["tcp"]  = &tcp::postEvent;
	_classes["si"]   = &si::postEvent;

	//	Register event methods
	luaL_register( _lua, "event", event_methods );
}

Module::~Module()
{
	{	//	Cleanup timers
		TimerList::iterator it=_timers.begin();
		while (it != _timers.end()) {
			cancelTimer( (*it).first, true );
			it = _timers.begin();
		}
	}

	{	//	Cleanup sockets
		SocketList::iterator it=_sockets.begin();
		while (it != _sockets.end()) {
			disconnect( it );
			it=_sockets.begin();
		}
	}
}

//	Initialization
void Module::setCallback( const Callback &callback ) {
	_callback = callback;
}

void Module::setInputEventCallback( const InputCallback &callback ) {
	_inCallback = callback;
}

//	LuaPlayer methods
void Module::dispatchKey( util::key::type key, bool isUp ) {
	key::dispatchKey( this, key, isUp );
}

void Module::dispatchBtn( bool isPress, int x, int y ) {
	pointer::dispatchBtn( this, isPress, x, y );
}

void Module::dispatchPresentation( evtAction::type action, const std::string &label ) {
	ncl::dispatchPresentation( this, action, label );
}

void Module::dispatchAttribution( const std::string &name, evtAction::type action, const std::string &value ) {
	ncl::dispatchAttribution( this, name, action, value );
}

//	Dispatch methods
void Module::dispatchIn( EventImpl *table ) {
	BOOST_FOREACH( HandlerPtr handler, _handlers ) {
		if (handler.get() && handler->dispatch( table )) {
			break;
		}
	}

	if (!_inCallback.empty()) {
		_inCallback( table );
	}
}

void Module::dispatchOut(
	nclEvtType::type type,
	evtAction::type action,
	const std::string &parameter,
	const std::string &value )
{
	if (!_callback.empty()) {
		_callback( type, action, parameter, value );
	}
}

//	Lua event methods
int Module::postEvent( const char *evtClass, bool isOut, int eventPos ) {
	Classes::const_iterator it=_classes.find( evtClass );
	if (it != _classes.end()) {
		return ((*it).second)( _lua, isOut, eventPos );
	}
	else {
		return luaL_error( _lua, "[player::event] Class not implemented: class=%s", evtClass );
	}
}

int Module::addHandler( int pos, int fncIndex, const std::string &classFilter ) {
	//	Find handler
	Handlers::const_iterator it=_handlers.begin();
	while (it != _handlers.end()) {
		HandlerPtr handler=(*it);
		if (handler.get() && handler->isEqual(fncIndex)) {
			return luaL_error( _lua, "[player::event] Handler already registered" );
		}
		it++;
	}

	//	Push function into registry
	lua_pushvalue( _lua, fncIndex );
	int ref = luaL_ref(_lua, LUA_REGISTRYINDEX);

	HandlerPtr handler( new Handler( _lua, ref, classFilter ) );
	if (pos < 0) {
		_handlers.push_back( handler );
	}
	else {
		if(_handlers.size() < static_cast<size_t>( pos )){
			_handlers.resize( pos, HandlerPtr() );
		}
		_handlers[pos-1] = handler;
	}
	return 0;
}

void Module::removeHandler( int index ) {
	Handlers::iterator it=_handlers.begin();
	while (it != _handlers.end()) {
		HandlerPtr handler = (*it);
		if (handler.get() &&handler->isEqual( index )) {
			_handlers.erase(it);
			break;
		}
		it++;
	}
}

util::DWORD Module::uptime() const {
	return _player->uptime();
}

void Module::registerTimer( util::DWORD ms ) {
	//	Register timer
	util::id::Ident timerID=_sys->registerTimer( ms, boost::bind( &Module::onTimerExpired, this, _1 ) );

	//	Push function into registry
	lua_pushvalue( _lua, 2 );
	int key = luaL_ref(_lua, LUA_REGISTRYINDEX);

	//	Push cancel function with timerID as upvalue(1)
	lua_pushnumber( _lua, key );
	lua_pushcclosure( _lua, l_cancel, 1 );

	_timers.push_back( std::make_pair(timerID,key) );
}

void Module::cancelTimer( int timerRef ) {
	//	Find timer in list
	TimerList::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByRef(timerRef)
	);
	if (it != _timers.end()) {
		cancelTimerAux( it, true );
	}
}

void Module::cancelTimer( util::id::Ident &timerID, bool needUnregister ) {
	//	Find timer in list
	TimerList::iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByID(timerID)
	);
	if (it != _timers.end()) {
		cancelTimerAux( it, needUnregister );
	}
}

void Module::cancelTimerAux( TimerList::iterator &it, bool needUnregister ) {
	//	Cancel function callback
	luaL_unref(_lua, LUA_REGISTRYINDEX, it->second );

	//	Cancel timer
	if (needUnregister) {
		_sys->unregisterTimer( it->first );
	}

	//	Remove from list
	_timers.erase( it );
}

void Module::onTimerExpired( util::id::Ident &timerID ) {
	LDEBUG("lua::Event::Module", "On Timer expired: timerID=%p", timerID->getID() );
	bool result=false;
	TimerList::const_iterator it=std::find_if(
		_timers.begin(),
		_timers.end(),
		FindTimerByID(timerID)
	);
	if  (it != _timers.end()) {
		//	Get function from registry into stack
		lua_rawgeti(_lua, LUA_REGISTRYINDEX, it->second);
		if (!lua_isnil( _lua, -1 )) {
			lua_call( _lua, 0, 0 );
			cancelTimer( timerID, false );
			result=true;
		}
		else {
			lua_pop( _lua, 1 );
		}
	}

	if (!result) {
		LWARN( "lua::Event::Module", "Timer not found" );
	}
}

util::id::Ident Module::connect( const std::string &host, const char *port, util::DWORD /*timeOut*/ ) {
	util::id::Ident socketID;

	// LINFO("lua::Event::Module", "attempt to connect to host=%s, port=%d", host.c_str(), port );

	std::vector<util::net::SockAddr> addresses;
	if ( !util::net::resolve( addresses, host, port, util::net::family::any, util::net::type::stream )) {
		LERROR("lua::Event::Module", "Cannot resolve socket address: host=%s, port=%d", host.c_str(), port );
		return socketID;
	}

	//	Create socket
	util::net::Socket *sock = new util::net::Socket();
	if (!sock->create( util::net::type::stream )) {
		LERROR("lua::Event::Module", "Cannot create socket: host=%s, port=%d", host.c_str(), port );
		return socketID;
	}

	BOOST_FOREACH( const util::net::SockAddr &sockAddr, addresses ) {
		LDEBUG("lua::Event::Module", "Trying connect: host=%s", host.c_str() );

		//	Try connect to remote host
		if (!sock->connect( sockAddr )) {
			LERROR("lua::Event::Module", "Cannot connect: host=%s", host.c_str());
			continue;
		}

		//	Set socket non blocking
		if (!sock->setNonBlocking( true )) {
			LERROR("lua::Event::Module", "Cannot set socket non blocking: host=%s", host.c_str());
			continue;
		}

		//	Add socket to dispatcher
		socketID = _sys->addSocket( sock->fd(), boost::bind(&Module::onDataReceived, this, _1));
		if (util::id::isValid(socketID)) {
			LDEBUG("lua::Event::Module", "Socket connected: socket=%d", sock->fd() );
			_sockets[socketID] = sock;
			break;
		}
	}

	if (!util::id::isValid(socketID)) {
		delete sock;
	}

	return socketID;
}

bool Module::disconnect( util::id::ID_TYPE socketID ) {
	bool res = false;
	SocketList::iterator it=std::find_if(
		_sockets.begin(),
		_sockets.end(),
		FindSocketByID(socketID)
	);
	if (it != _sockets.end()) {
		disconnect( it );
		res = true;
	}
	return res;
}

void Module::disconnect( SocketList::iterator &it ) {
	LDEBUG( "lua::Event::Module", "disconnect socketID: %ld", it->first->getID() );

	//	Stop listen for data
	util::id::Ident id = it->first;
	_sys->stopSocket( id );

	//	Disconnect and cleanup
	util::net::Socket *sock = it->second;
	delete sock;

	_sockets.erase( it );
}

void Module::onDataReceived( util::id::Ident &socketID ) {
	LDEBUG("lua::Event::Module", "onDataReceived socketID: %li", socketID->getID());
	SocketList::iterator it=_sockets.find( socketID );
	if (it != _sockets.end()) {
		util::net::Socket *sock = it->second;
		util::Buffer buf( 1024 );
		util::SSIZE_T len = sock->recv( buf.data(), buf.capacity() );
		while (len > 0) {
			{	//	Notify data
				std::string tmp( (const char *)buf.bytes(), len );
				tcp::onDataReceived( this, tmp, socketID->getID() );
			}

			len = sock->recv( buf.data(), buf.capacity() );
		}
		if (len == 0) {	//	No data and peer has performed an orderly shutdown ... close socket!
			LWARN("lua::Event::Module", "Connection closed, disconnect socket" );
			disconnect( it );
		}
	}
}

bool Module::send( util::id::ID_TYPE socketID, const std::string &data, util::DWORD /*timeout=0*/ ) {
	bool res = false;
	SocketList::iterator it=std::find_if(
		_sockets.begin(),
		_sockets.end(),
		FindSocketByID(socketID)
	);
	if (it != _sockets.end()) {
		LDEBUG( "lua::Event::Module", "Send data to connection: %ld", socketID );
		util::SSIZE_T bytes=it->second->send( data.c_str(), data.length() );
		res=bytes > 0 && static_cast<size_t>(bytes) == data.length();
		if (!res) {
			LWARN("lua::Event::Module", "Connection error, disconnect socket: bytes=%d", bytes );
			disconnect( it );
		}
	}
	else {
		LWARN("lua::Event::Module", "Socket not found" );
	}
	return res;
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>(st,LUA_EVENT);
}

}
}

