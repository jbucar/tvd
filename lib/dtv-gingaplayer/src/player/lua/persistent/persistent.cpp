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

#include "persistent.h"
#include "dbdata.h"
#include "../lua.h"
#include "../../luaplayer.h"
#include "generated/config.h"
#include <util/serializer/xml.h>
#include <util/assert.h>
#include <util/mcr.h>
#include <util/cfg/configregistrator.h>
#include <util/cfg/cfg.h>

#define LUA_PERSISTENT "lua_persistent_module"

#define OPEN_RO_TABLE \
	lua_newtable( _lua ); /* fake table */ \
	lua_newtable( _lua ); /* fake metatable */ \
	lua_pushcfunction( _lua, impl::newIndexRO ); \
	lua_setfield( _lua, -2, "__newindex" ); \
	lua_newtable( _lua ); /* table */ \

#define CLOSE_RO_TABLE \
	lua_setfield( _lua, -2, "__index" ); \
	lua_setmetatable( _lua, -2 ); \
	lua_setfield( _lua, LUA_GLOBALSINDEX, "persistent" );

REGISTER_INIT_CONFIG( gingaPlayer_luaPersistence ) {
	root().addNode("luaPersistence").addValue("file", "Path to persist data from lua", GINGAPLAYER_LUA_PERSISTENT_FILE);
}

namespace player {
namespace persistent {

namespace impl {

static int newIndexRO( lua_State *st ) {
	return luaL_error( st, "[player::persistent] persistent is a read only table.\n" );
}

} // end namespace impl

/*******************************************************************************
 *	Persistent module
 *******************************************************************************/

Module::Module( lua_State *st )
	: _lua( st )
{
	//	Store module into stack
	lua::storeObject( _lua, this, LUA_PERSISTENT );

	//	Export persistent table
	OPEN_RO_TABLE
	TableData persistentTable;
	load( &persistentTable );

	if (persistentTable.table().size() == 0) {
		persistentTable.add( new DBDataImpl<TableData>( dbdata::table, "shared", TableData() ) );
		persistentTable.add( new DBDataImpl<TableData>( dbdata::table, "service", TableData() ) );
		persistentTable.add( new DBDataImpl<TableData>( dbdata::table, "channel", TableData() ) );
	}
	BOOST_FOREACH( DBDataLoader* data, persistentTable.table() ) {
		data->data()->push( _lua );
	}
	CLOSE_RO_TABLE
}

Module::~Module() {
	lua_getglobal(_lua, "persistent");
	int idx = lua_gettop(_lua);
	luaL_getmetafield(_lua, idx, "__index");

	// Create table from lua stack
	TableData persistentTable( _lua, idx+1 );

	// Save table
	save( &persistentTable );
}

Module *Module::get( lua_State *st ) {
	return lua::getObject<Module>( st, LUA_PERSISTENT );
}

#define PERFORM( operation, data ) \
	std::string path = util::cfg::getValue<std::string> ("gingaPlayer.luaPersistence.file"); \
	if (!path.empty()) { \
		util::serialize::XML xmlSer(path); \
		if (!xmlSer.operation( *data )) { \
			LWARN("lua::Persistent", "Fail to %s persistent tables", #operation); \
		} \
	} \

void Module::load( TableData *data ) {
	PERFORM(load, data)
}

void Module::save( TableData *data ) {
	PERFORM(save, data)
}

#undef PERFORM

}
}
