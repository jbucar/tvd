#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, invap_lua_29 ) {
	ASSERT_TRUE( play("invap/lua/29_position.ncl") );
	ASSERT_TRUE( compareTo("invap_29_position") );
}

TEST_F( Ginga, invap_lua_30 ) {
	ASSERT_TRUE( play("invap/lua/30_canvas.ncl") );
	ASSERT_TRUE( compareTo("invap_30_canvas") );
}

}
