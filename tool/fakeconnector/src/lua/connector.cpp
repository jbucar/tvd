#include "connector.h"
#include  "../fakeconnector.h"
#include <util/keydefs.h>
#include <connector/connector.h>

namespace lua {

namespace impl {
	static FakeConnector *connector = NULL;
	static lua_State *lState;
}

static int l_sleep(lua_State* L) {
       int time = luaL_checkinteger(L, 1);
       boost::this_thread::sleep( boost::posix_time::milliseconds(time) );
       return 0;
}

static int l_run(lua_State* /*L*/) {
	impl::connector->loop();
	return 0;
}

static int l_exit(lua_State* /*L*/) {
	impl::connector->exit();
	return 0;
}

static int l_sendKey(lua_State* L) {
	util::key::type keyCode = (util::key::type) luaL_checkinteger(L, 1);
	impl::connector->sendKey(keyCode);
	return 0;
}

static int l_sendPropertyValueEC(lua_State* L) {
	std::string appId      = luaL_checkstring(L, 1);
	std::string mediaId    = luaL_checkstring(L, 2);
	std::string propertyId = luaL_checkstring(L, 3);
	std::string value      = luaL_checkstring(L, 4);

	std::string payload = "null,"+appId+","+mediaId+","+propertyId+","+value;
	impl::connector->sendEditingCommand(0x2d, 1, (const unsigned char*)payload.c_str(), payload.size());
	return 0;
}

static const struct luaL_Reg connector_methods[] = {
	{ "sendPropertyValueEC" , l_sendPropertyValueEC  },
	{ "sendKey"             , l_sendKey              },
	{ "run"                 , l_run                  },
	{ "exit"                , l_exit                 },		
	{ NULL                  , NULL                   }
};

#define EXPORT_KEY( c, n, u )                             \
        lua_pushnumber( impl::lState, util::key::c  ); \
        lua_setglobal ( impl::lState, #u );

void init( lua_State *state, FakeConnector *connector ) {
	impl::connector = connector;
	impl::lState    = state;
	luaL_openlibs(impl::lState);
	luaL_register( impl::lState, "connector", connector_methods );
	lua_register(impl::lState, "sleep", l_sleep);	
	UTIL_KEY_LIST(EXPORT_KEY);
}

void fin() {
	impl::lState = NULL;
	impl::connector = NULL;
}

}
