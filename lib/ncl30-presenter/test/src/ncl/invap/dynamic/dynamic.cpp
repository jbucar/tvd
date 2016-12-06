#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_invap_lua_36 ) {
	ASSERT_TRUE( play("invap/dynamic/36_img_position.ncl") );
	ASSERT_TRUE( compareTo("invap_36_img_position") );
}

TEST_F( Ginga, DISABLED_invap_lua_37 ) {
	ASSERT_TRUE( play("invap/dynamic/37_text_position.ncl") );
	ASSERT_TRUE( compareTo("invap_37_text_position") );
}

}
