#pragma once

class FakeConnector;

extern "C" {
#include <lua5.1/lua.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#if LUA_VERSION_NUM	!= 501
#error "Lua version is incorrect"
#endif
}

namespace lua {

void init( lua_State *lua, FakeConnector *connector );
void fin();

}
