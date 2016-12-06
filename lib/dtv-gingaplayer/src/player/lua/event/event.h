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

#pragma once

#include "types.h" 
#include <canvas/types.h>
#include <util/id/ident.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

typedef struct lua_State lua_State;

namespace util {
namespace net {
	class Socket;
}
}

namespace player {

class System;
class LuaPlayer;

namespace event {

class Handler;

class Module {
public:
	Module( System *sys, LuaPlayer *player, lua_State *lua );
	virtual ~Module();

	//	Initialization
	void setCallback( const Callback &callback );
	void setInputEventCallback( const InputCallback &callback );	

	//	Lua player methods
	void dispatchKey( util::key::type key, bool isUp );
	void dispatchBtn( bool isPress, int x, int y );
	void dispatchPresentation( evtAction::type action, const std::string &label );
	void dispatchAttribution( const std::string &name, evtAction::type action, const std::string &value );

	//	Dispatch methods
	void dispatchIn( EventImpl *table );
	void dispatchOut(
		nclEvtType::type type,
		evtAction::type action,
		const std::string &parameter,
		const std::string &value );

	//	Lua methods (Internal use)
	int postEvent( const char *evtClass, bool isOut, int eventPos );
	int addHandler( int pos, int method, const std::string &classFilter );
	void removeHandler( int index );
	util::DWORD uptime() const;
	
	//	Timer
	typedef std::pair<util::id::Ident,int> TimerRef;
	typedef std::vector<TimerRef> TimerList;
	void registerTimer( util::DWORD ms );
	void cancelTimer( int timerRef );
	void cancelTimer( util::id::Ident &id, bool needUnregister );
	
	//	Socket
	typedef std::map<util::id::Ident,util::net::Socket *> SocketList;
	util::id::Ident connect( const std::string &host, const char *port, util::DWORD timeout=0 );
	bool disconnect( util::id::ID_TYPE socketID );
	bool send( util::id::ID_TYPE socketID, const std::string &data, util::DWORD timeout=0 );

	//	Aux
	static Module *get( lua_State *st );

protected:
	typedef int (*PostEventMethod)( lua_State *st, bool isOut, int eventPos );	
	typedef std::map<std::string,PostEventMethod> Classes;
	typedef boost::shared_ptr<Handler> HandlerPtr;
	typedef std::vector<HandlerPtr> Handlers;

	//	Timers 
	void cancelTimerAux( TimerList::iterator &it, bool unregister );
	void onTimerExpired( util::id::Ident &timerID );

	//	Sockets
	void disconnect( SocketList::iterator &it );
	void onDataReceived( util::id::Ident &socketID );

private:
	System *_sys;	
	LuaPlayer *_player;
	lua_State *_lua;
	Callback _callback;
	InputCallback _inCallback;
	Classes _classes;
	Handlers _handlers;

	SocketList _sockets;
	TimerList _timers;
};

}	// namespace event
}	// namespace player

