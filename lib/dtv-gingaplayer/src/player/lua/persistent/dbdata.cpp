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

#include "dbdata.h"
#include <util/mcr.h>
#include <util/log.h>
#include <stdexcept>

namespace player {

namespace lua {

void pushValue( lua_State *st, const std::string &index, persistent::NilData & /*data*/ ) {
	lua_pushnil(st);
	lua_setfield( st, -2, index.c_str() );
}

void pushValue( lua_State *st, const std::string &index, persistent::TableData &table ) {
	table.push( st, index );
}

} // end namespace lua

namespace persistent {

DBData::DBData( dbdata::type type, const std::string &key )
	: _type(type), _key(key)
{
}

DBData::~DBData()
{
}

const std::string &DBData::key() const {
	return _key;
}

std::string &DBData::mutableKey() {
	return _key;
}

const dbdata::type &DBData::type() const {
	return _type;
}

DBData *DBData::create( dbdata::type type, const std::string &key ) {
	switch (type) {
		case dbdata::nil:
			return new DBDataImpl<NilData>( type, key, NilData() );
		case dbdata::boolean:
			return new DBDataImpl<bool>( type, key, true );
		case dbdata::integer:
			return new DBDataImpl<int>( type, key, 0 );
		case dbdata::string:
			return new DBDataImpl<std::string>( type, key, "" );
		case dbdata::table:
			return new DBDataImpl<TableData>( type, key, TableData() );
		default:
			LERROR("lua::persistent::DBData", "Cannot create DBData. Invlid type!");
			throw std::runtime_error( "Cannot create DBData. Invlid type!" );
	}
}

/************************** TableData **************************/
TableData::TableData()
{
}

TableData::TableData( const TableData &other )
{
	*this = other;
}

TableData::TableData( lua_State *st, int idx )
{
	lua_pushnil(st);
	while (lua_next(st, idx) != 0) {
		std::string key( luaL_checkstring(st, -2) );
		switch (lua_type(st, -1)) {
			case LUA_TNIL:
				add( new DBDataImpl<NilData>( dbdata::nil, key, NilData() ) ); break;
			case LUA_TNUMBER:
				add( new DBDataImpl<int>( dbdata::integer, key, luaL_checkint(st, -1) ) ); break;
			case LUA_TSTRING:
				add( new DBDataImpl<std::string>( dbdata::string, key, std::string( luaL_checkstring(st, -1) ) ) ); break;
			case LUA_TBOOLEAN:
				add( new DBDataImpl<bool>( dbdata::boolean, key, lua_toboolean(st, -1) != 0 ) ); break;
			case LUA_TTABLE:
				add( new DBDataImpl<TableData>( dbdata::table, key, TableData(st, lua_gettop(st)) ) ); break;
			default:
				LERROR("lua::persistent::types", "Cannot save data for key %s. Invalid type!", key.c_str()); 
				throw std::runtime_error("Cannot save data. Invalid type!");
		}
		lua_pop(st, 1);
	}
}

TableData::~TableData()
{
	CLEAN_ALL( DBDataLoader*, _table );
}

TableData &TableData::operator=( const TableData &other ) {
	if (this != &other) {
		BOOST_FOREACH( DBDataLoader* dataLoader, const_cast<TableData&>(other).table() ) {
			_table.push_back( dataLoader->getCopy() );
		}
	}
	return *this;
}

DBData *TableData::operator[]( int pos ) {
	return _table[pos]->data();
}

void TableData::push( lua_State *st, const std::string &key ) {
	// create table and push it into lua stack
	lua_newtable( st );

	// push table fields into lua stack
	BOOST_FOREACH( DBDataLoader *dbDataLoader, _table ) {
		dbDataLoader->data()->push( st );
	}

	lua_setfield( st, -2, key.c_str() );
}

std::vector<DBDataLoader*> &TableData::table() {
	return _table;
}

void TableData::table( const std::vector<DBDataLoader*> &t ) {
	_table = t;
}

unsigned TableData::size() const {
	return _table.size();
}

void TableData::add( DBData *data ) {
	DBDataLoader *tmp = new DBDataLoader();
	tmp->data( data );
	_table.push_back( tmp );
}

}
}
