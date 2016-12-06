/******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "control.h"
#include "../lua.h"
#include <zapper/zapper.h>
#include <util/netlib/requestinfo.h>
#include <util/netlib/manager.h>
#include <util/buffer.h>
#include <util/log.h>
#include <util/mcr.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace luaz {
namespace network {

namespace bfs = boost::filesystem;

namespace impl {

class RequestInfo {
public:
	RequestInfo( lua_State *L, const util::id::Ident &id, util::Buffer *buf, int methodKey ) : _state(L), _id(id), _buf(buf), _methodKey(methodKey) {}
	RequestInfo( lua_State *L, const util::id::Ident &id, const std::string &fileName, int methodKey ) : _state(L), _id(id), _buf(NULL), _fileName(fileName), _methodKey(methodKey) {}
	~RequestInfo() {
		//	Remove function from registry
		luaL_unref(_state, LUA_REGISTRYINDEX, _methodKey );
		if (!_fileName.empty()) {
			bfs::remove( _fileName );
		}

		delete _buf;
	}

	const util::id::Ident &id() const {
		return _id;
	}

	void run( bool result ) {
		lua_rawgeti(_state, LUA_REGISTRYINDEX, _methodKey);
		lua_pushnumber( _state, _id->getID() );
		lua_pushnumber( _state, result ? 1 : 0 );
		lua_pushstring( _state, _buf ? (const char *)_buf->bytes() : _fileName.c_str() );
		lua_call( _state, 3, 0 );
		_fileName.clear();
	}

private:
	lua_State *_state;
	util::id::Ident _id;
	util::Buffer *_buf;
	std::string _fileName;
	int _methodKey;
};

struct FindByID {
	FindByID( const util::id::Ident &id ) : _id(id) {}

	bool operator()( const RequestInfo *info ) const  {
		return info->id() == _id;
	}

	const util::id::Ident &_id;

private:
	FindByID &operator=( const FindByID & ) {
		return *this;
	}
};

struct FindByNumber {
	FindByNumber( util::id::ID_TYPE id ) : _id(id) {}

	bool operator()( const RequestInfo *info ) const  {
		return info->id()->getID() == _id;
	}

	util::id::ID_TYPE _id;
};

//	Variables
static util::netlib::Manager *netlib = NULL;
static std::list<RequestInfo *> requests;


static void requestFinished( const util::id::Ident &id, bool result ) {
	std::list<RequestInfo *>::iterator it=std::find_if(
		requests.begin(),
		requests.end(),
		FindByID(id)
	);
	if (it != requests.end()) {
		RequestInfo *info = (*it);
		info->run( result );
		delete info;

		impl::requests.erase( it );
	}
}

}	//	namespace impl

static int l_httpRequest( lua_State *L ) {
	std::string url;
	std::string method="GET";
	std::vector<std::string> headers;
	std::string body;
	int callbackID=-1;
	bool verbose=false;

	int idx=lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0) {
		std::string key( luaL_checkstring(L, -2) );
		if (key == "url") {
			url = luaL_checkstring(L, -1);
		}
		else if (key == "method") {
			method = luaL_checkstring(L, -1);
		}
		else if (key == "headers") {
			lua::readList( L, lua_gettop(L), headers );
		}
		else if (key == "body") {
			body = luaL_checkstring(L, -1);
		}
		else if (key == "verbose") {
			verbose = (luaL_checknumber(L, -1) == 0) ? false : true;
		}
		else if (key == "callback") {
			//	Check function if function
			luaL_checktype( L, -1, LUA_TFUNCTION );

			//	Push function into registry
			lua_pushvalue( L, -1 );
			callbackID = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		lua_pop(L, 1);
	}

	util::netlib::RequestInfo *request = new util::netlib::RequestInfo();
	util::Buffer *buf = new util::Buffer(1024);

	//	Setup request
	request->url( url );
	request->verbose( verbose );
	request->headers( headers );
	request->method( method );
	request->postFields( body );
	request->onFinished( boost::bind(&impl::requestFinished,_1,_2) );

	//	Start request
	util::id::Ident id = impl::netlib->start( request, buf );
	if (util::id::isValid(id)) {
		//	Add request
		impl::RequestInfo *info = new impl::RequestInfo(L,id,buf,callbackID);
		impl::requests.push_back( info );

		lua_pushnumber( L, id->getID() );
	}
	else {
		lua_pushnumber( L, -1 );
		delete buf;
	}
	return 1;
}

static int l_fetch( lua_State *L ) {
	std::string url;
	int callbackID=-1;
	bool verbose=false;
	std::string tmpDirectory;

	int idx=lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, idx) != 0) {
		std::string key( luaL_checkstring(L, -2) );
		if (key == "url") {
			url = luaL_checkstring(L, -1);
		}
		else if (key == "verbose") {
			verbose = (luaL_checknumber(L, -1) == 0) ? false : true;
		}
		else if (key == "temporaryDirectory") {
			tmpDirectory = luaL_checkstring(L, -1);
		}
		else if (key == "callback") {
			//	Check function if function
			luaL_checktype( L, -1, LUA_TFUNCTION );

			//	Push function into registry
			lua_pushvalue( L, -1 );
			callbackID = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		lua_pop(L, 1);
	}

	//	Setup filename
	std::string fileName = util::netlib::temporaryName( tmpDirectory );

	//	Setup request
	util::netlib::RequestInfo *request = new util::netlib::RequestInfo();
	request->url( url );
	request->verbose( verbose );
	request->onFinished( boost::bind(&impl::requestFinished,_1,_2) );

	//	Start request
	util::id::Ident id = impl::netlib->start( request, fileName );
	if (util::id::isValid(id)) {
		//	Add request
		impl::RequestInfo *info = new impl::RequestInfo(L,id,fileName,callbackID);
		impl::requests.push_back( info );

		lua_pushnumber( L, id->getID() );
	}
	else {
		lua_pushnumber( L, -1 );
	}
	return 1;
}

static int l_cancel( lua_State *L ) {
	long int id = luaL_checkint(L, 1);
	std::list<impl::RequestInfo *>::const_iterator it=std::find_if(
		impl::requests.begin(),
		impl::requests.end(),
		impl::FindByNumber(id)
	);
	if (it != impl::requests.end()) {
		//	Only stop request. The request call to onFinished callback and free resources
		impl::RequestInfo *info = (*it);
		impl::netlib->stop( info->id() );
	}
	return 0;
}

static const struct luaL_Reg network_methods[] = {
	{ "httpRequest", l_httpRequest },
	{ "fetch",       l_fetch       },
	{ "cancel",      l_cancel      },
	{ NULL,          NULL          }
};

//	Public methods
void initialize( zapper::Zapper *zapper ) {
	impl::netlib = zapper->network();
	DTV_ASSERT(impl::netlib);
}

void finalize() {
	DTV_ASSERT(impl::requests.empty());
	impl::netlib = NULL;
}

void start( lua_State *st ) {
	luaL_register( st, "netlib", network_methods );
}

void stop() {
}

}
}

