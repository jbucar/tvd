#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, invap_buttons_41 ) {
	ASSERT_TRUE( play("invap/buttons/41_lua.ncl") );
	ASSERT_TRUE( compareTo("invap_41_lua") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_up ));
	ASSERT_TRUE( compareTo("invap_41_lua_key_press") );
}

}
