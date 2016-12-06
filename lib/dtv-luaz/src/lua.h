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

#pragma once

#include <util/assert.h>
#include <string>
#include <vector>

extern "C" {
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#if LUA_VERSION_NUM	!= 501
#error "Lua version is incorrect"
#endif
}

namespace luaz {
namespace lua {

lua_State *init();
std::string fontsPath();
std::string imagesPath();
std::string scriptsPath();

void readList( lua_State* L, int ix, std::vector<std::string>& values );
void readList( lua_State* L, int ix, std::vector<std::vector<std::string> >& values, const std::vector<std::string>& keys );
void readList( lua_State* L, int ix, std::vector<int>& values );
int pushList(lua_State* L, std::vector<std::string>& values);

void call( lua_State *st, const char *method );
void call( lua_State *st, const int callbackref );
void call( lua_State *st, const char *method, int value );
void call( lua_State *st, const char *method, int value1, int value2 );
void call( lua_State *st, const char *method, int value1, int value2, int value3, int value4 );
void call( lua_State *st, const char *method, const std::string &value );
void call( lua_State *st, const char *method, const std::string &value, int value2 );

void setGlobal( lua_State *st, const std::string &name, const std::string &value );
void setGlobal( lua_State *st, const std::string &name, int value );
void setGlobal( lua_State *st, const std::string &name, bool value );
template<typename T> void setGlobalT( lua_State *st, const std::string &name, T val ) { setGlobal( st, name, val ); }

std::string getStrGlobal( lua_State *st, const std::string &name );

void setField( lua_State *st, const std::string &index, const int &value );
void setField( lua_State *st, const std::string &index, const std::string &value );
void setField( lua_State *st, const int &index, const std::string &value );
void setField( lua_State *st, const int &index, const int &value );

bool checkBool( lua_State *L, int pos=1 );

void freeRef( lua_State *st, const int ref );

template<typename T>
static inline int getModes( lua_State *L, T current, const std::vector< std::pair<T,const char *> > &values ) {
	int cur=-1;
	lua_newtable(L);
	for (size_t i=0; i<values.size(); i++) {
		if (values[i].first == current) {
			cur=i;
		}

		//	Key
		lua_pushnumber( L, i+1 );

		//	Value
		lua_pushstring( L, values[i].second );

		//	Set table
		lua_settable( L, -3);
	}
	lua_pushnumber( L, cur );
	BOOST_ASSERT(cur != -1);
	return 2;
}

template<typename T>
static inline int get( lua_State *L, T current, const std::vector< std::pair<T,const char *> > &values ) {
	int cur=-1;
	for (size_t i=0; i<values.size(); i++) {
		if (values[i].first == current) {
			cur=i;
			break;
		}
	}
	BOOST_ASSERT(cur >= 0);
	lua_pushnumber( L, cur );
	lua_pushstring( L, values[cur].second );
	return 2;
}

template<typename T>
static inline T getIndex( lua_State *L, const std::vector< std::pair<T,const char *> > &values, int lIndex=1 ) {
	int index = luaL_checkint(L, lIndex);
	BOOST_ASSERT( index >= 0 && index < static_cast<int>(values.size()) );
	return values[index].first;
}

}
}

