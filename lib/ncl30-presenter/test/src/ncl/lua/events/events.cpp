#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, lua_event_attributionNCLToLua ) {
	ASSERT_TRUE( play("lua/events/attributionNCLToLuaWithOneMedia.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("red720x576") );
	ASSERT_TRUE( pressKey( ::util::key::blue ));
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::yellow ));
	ASSERT_TRUE( compareTo("yellow720x576") );
	ASSERT_TRUE( pressKey( ::util::key::green ));
	ASSERT_TRUE( compareTo("green720x576") );
}

TEST_F( Ginga, lua_event_attributionNCLToLuaWithTwoMedias ) {
	ASSERT_TRUE( play("lua/events/attributionNCLToLuaWithTwoMedias.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("red720x576") );
	ASSERT_TRUE( pressKey( ::util::key::blue ));
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::yellow ));
	ASSERT_TRUE( compareTo("yellow720x576") );
	ASSERT_TRUE( pressKey( ::util::key::green ));
	ASSERT_TRUE( compareTo("green720x576") );
}

TEST_F( Ginga, lua_event_attributionLuaToNCL) {
	ASSERT_TRUE( play("lua/events/attributionLuaToNCL.ncl") );
	ASSERT_TRUE( compareTo("blue360x288") );
	ASSERT_TRUE( pressKey( ::util::key::green ));
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288") );
}

TEST_F( Ginga, lua_event_presentationNCLToLua) {
	ASSERT_TRUE( play("lua/events/presentationNCLToLua.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, lua_event_presentationLuaToNCL) {
	ASSERT_TRUE( play("lua/events/presentationLuaToNCL.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, lua_event_areaPresentationLuaToNCL) {
	ASSERT_TRUE( play("lua/events/areaPresentationLuaToNCL.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, lua_event_presentationLuaToNCLStopArea) {
	ASSERT_TRUE( play("lua/events/presentationLuaToNCLStopArea.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}



}

