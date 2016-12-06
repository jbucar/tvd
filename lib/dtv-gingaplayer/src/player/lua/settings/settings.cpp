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

#include "settings.h"
#include "../../settings.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include <util/cfg/cfg.h>
#include <string>
#include <string.h>


#define LUA_SETTINGS  "lua_settings_module"

#define OPEN_RO_TABLE \
	lua_newtable( _lua ); /* fake table */ \
	lua_newtable( _lua ); /* fake metatable */ \
	lua_newtable( _lua ); /* real table */ \

#define CLOSE_RO_TABLE \
	lua_setfield( _lua, -2, "__index" ); \
	lua_pushcfunction( _lua, setHandler ); \
	lua_setfield( _lua, -2, "__newindex" ); \
	lua_setmetatable( _lua, -2 ); \
//	avoid metatable override?
//	lua_pushvalue( _lua, -1 );
//	lua_setfield( _lua, -2, "__metatable" );

namespace player {
namespace settings {

int setHandler( lua_State *st ) {
	if (!settings::Module::get( st )->isReadOnly()) {
		lua_rawset( st, 1 );
	} else {
		luaL_error( st, "[player::settings] settings is a read only table.\n" );
	}
	return 0;
}

class UtilCfg2Lua
{

public:
	UtilCfg2Lua( lua_State *st ) {
		_lua = st;
	}

	virtual ~UtilCfg2Lua( void ) {
		_lua = NULL;
	}

	void exportKey( const std::string &name ) {
		operator()( &util::cfg::get( std::string("gingaPlayer.")+name ) );
	}

	// visitor methods
	void operator()( const util::cfg::PropertyNode *node ) {
		OPEN_RO_TABLE
		node->visitValues( *this );
		node->visitNodes( *this );
		CLOSE_RO_TABLE
		lua_setfield( _lua, -2, node->name().c_str() );
	}

	void operator()( const util::cfg::PropertyValue *value ) {
		std::string type = value->type().name();
		if (type == "i") {
			lua_pushnumber( _lua, value->get<int>() );
		} else if (type == "f") {
			lua_pushnumber( _lua, value->get<float>() );
		} else {
			lua_pushstring( _lua, value->asString().c_str() );
		}
		lua_setfield( _lua, -2, value->name().c_str() );
	}

private:
	lua_State *_lua;
}
;

/*******************************************************************************
 *	Settings module
 *******************************************************************************/

Module::Module( lua_State *st ) :
	_lua( st ), _readOnly( false )
{

	//	Store module into stack
	lua::storeObject( _lua, this, LUA_SETTINGS );
	exportTable();
	settings::addListener( this );
}

Module::~Module() {
	settings::delListener( this );
}

bool Module::isReadOnly( void ) {
	return _readOnly;
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>( st, LUA_SETTINGS );
}

void Module::exportTable( void ) {
	//	Export settings table
	UtilCfg2Lua *o = new UtilCfg2Lua( _lua );

	OPEN_RO_TABLE
	o->exportKey( "system" );
	o->exportKey( "user" );
	o->exportKey( "default" );
	o->exportKey( "service" );
	o->exportKey( "si" );
	o->exportKey( "channel" );
	o->exportKey( "shared" );
	CLOSE_RO_TABLE
	delete o;
	readOnly( true );

	lua_setfield( _lua, LUA_GLOBALSINDEX, "settings" );  // add settings to global context
}

void Module::readOnly( bool value ) {
	_readOnly = value;
}

void Module::update( void ) {
	lua_pushnil( _lua );
	lua_setfield( _lua, LUA_GLOBALSINDEX, "settings" );
	lua_gc( _lua, LUA_GCCOLLECT, 0 );
	exportTable();
}

}
}

