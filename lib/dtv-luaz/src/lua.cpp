/*******************************************************************************

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

#include "lua.h"
#include "generated/config.h"
#include <util/fs.h>
#include <util/log.h>
#include <boost/filesystem.hpp>

namespace luaz {
namespace lua {

namespace fs = boost::filesystem;

void call( lua_State *st, const char *method ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_call( st, 0, 0 );
}

void call( lua_State *st, int callbackref ) {
	DTV_ASSERT(callbackref != LUA_NOREF);
	lua_rawgeti( st, LUA_REGISTRYINDEX, callbackref );
	lua_call( st, 0, 0 );
}

void call( lua_State *st, const char *method, int value ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_pushnumber( st, value );
	lua_call( st, 1, 0 );
}

void call( lua_State *st, const char *method, int value1, int value2 ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_pushnumber( st, value1 );
	lua_pushnumber( st, value2 );
	lua_call( st, 2, 0 );
}

void call( lua_State *st, const char *method, int value1, int value2, int value3, int value4 ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_pushnumber( st, value1 );
	lua_pushnumber( st, value2 );
	lua_pushnumber( st, value3 );
	lua_pushnumber( st, value4 );
	lua_call( st, 4, 0 );
}

void call( lua_State *st, const char *method, const std::string &value ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_pushstring( st, value.c_str() );
	lua_call( st, 1, 0 );
}

void call( lua_State *st, const char *method, const std::string &value, int value2 ) {
	DTV_ASSERT(method);
	lua_getfield( st, LUA_GLOBALSINDEX, method );
	lua_pushstring( st, value.c_str() );
	lua_pushnumber( st, value2 );
	lua_call( st, 2, 0 );
}

void setGlobal( lua_State *st, const std::string &name, const std::string &value ) {
	lua_pushstring( st, value.c_str() );
	lua_setglobal( st, name.c_str() );
}

void setGlobal( lua_State *st, const std::string &name, int value ) {
	lua_pushnumber( st, value );
	lua_setglobal( st, name.c_str() );
}

void setGlobal( lua_State *st, const std::string &name, bool value ) {
	lua_pushboolean( st, value ? 1 : 0 );
	lua_setglobal( st, name.c_str() );
}

std::string getStrGlobal( lua_State *st, const std::string &name ) {
	lua_getglobal(st, name.c_str());
	return lua_tostring(st, -1);
}

void setField( lua_State *st, const std::string &index, const int &value ) {
	lua_pushstring(st, index.c_str());
	lua_pushnumber(st, value );
	lua_settable(st, -3);
}

void setField( lua_State *st, const std::string &index, const std::string &value ) {
	lua_pushstring(st, index.c_str());
	lua_pushstring(st, value.c_str());
	lua_settable(st, -3);
}

void setField( lua_State *st, const int &index, const std::string &value ) {
	lua_pushnumber(st, index);
	lua_pushstring(st, value.c_str());
	lua_settable(st, -3);
}

void setField( lua_State *st, const int &index, const int &value ) {
	lua_pushnumber(st, index);
	lua_pushnumber(st, value);
	lua_settable(st, -3);
}

bool checkBool( lua_State *L, int pos/*=1*/ ) {
	if ( lua_isboolean( L, pos ) ) {
		return lua_toboolean( L, pos ) ? true : false;
	} else {
		luaL_argcheck(L, false, pos, "boolean expected" );
		return false;
	}
}

void freeRef( lua_State *st, const int ref ) {
	luaL_unref( st, LUA_REGISTRYINDEX, ref );
}

static void addPath( lua_State *L, const std::string &modulePath ) {
	lua_getglobal( L, "package" );
	lua_getfield( L, -1, "path" ); // get field "path" from table at top of stack (-1)
	std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack

	{	// Add search: modulePath/?
		cur_path.append( ";" );
		fs::path path = modulePath;
		path /= "?";
		cur_path.append( path.string().c_str() );
	}

	{	// Add search: modulePath/?.lua
		cur_path.append( ";" ); // do your path magic here
		fs::path path = modulePath;
		path /= "?.lua";
		cur_path.append( path.string().c_str() );
	}

	lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring( L, cur_path.c_str() ); // push the new one
	lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
	lua_pop( L, 1 ); // get rid of package table from top of stack
}

std::string imagesPath() {
	return util::fs::make( util::fs::installDataDir(), "imgs" );
}

std::string scriptsPath() {
	return util::fs::make( util::fs::installDataDir(), "lua" );
}

std::string fontsPath() {
	return util::fs::make( util::fs::installDataDir(), "fonts" );
}

lua_State *init() {
	//	Setup lua
	lua_State *st = lua_open();
	if (!st) {
		LERROR( "lua", "Cannot initialize lua engine" );
		return NULL;
	}

	//	Assign more stack slots ..... HACK!
	if (lua_checkstack( st, 40 ) < 0) {
		LERROR( "lua", "Cannot initialize lua engine" );
		return NULL;
	}

	//	Setup
	luaL_openlibs(st);
	addPath( st, scriptsPath() );

	return st;
}

void readList(lua_State* L, int ix, std::vector<std::string>& values)
{
	lua_pushnil(L);
	while (lua_next(L, ix) != 0) {
		char *name = (char *)luaL_checkstring(L, -1);
		values.push_back(name);
		lua_pop(L, 1);
	}
}

void readList(lua_State* L, int ix, std::vector<std::vector<std::string> >& values, const std::vector<std::string>& keys)
{
	lua_pushnil(L);
	while (lua_next(L, ix) != 0) {
		std::vector<std::string> row;

		for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
			lua_pushstring(L, (*it).c_str());
			lua_gettable(L, -2);
			char *btext = (char *)luaL_checkstring(L, -1);
			lua_pop(L, 1);
			row.push_back(btext);
		}

		values.push_back(row);
		lua_pop(L, 1);
	}
}

void readList(lua_State* L, int ix, std::vector<int>& values)
{
	lua_pushnil(L);
	while (lua_next(L, ix) != 0) {
		int value = luaL_checkint(L, -1);
		values.push_back(value);
		lua_pop(L, 1);
	}
}

int pushList(lua_State* L, std::vector<std::string>& values)
{
	lua_createtable(L, values.size(), 0);
	int newTable = lua_gettop(L);
	int index = 1;
	std::vector<std::string>::const_iterator iter = values.begin();
	while(iter != values.end()) {
		lua_pushstring(L, (*iter).c_str());
		lua_rawseti(L, newTable, index);
		++iter;
		++index;
	}
	return 1;
}

}
}

