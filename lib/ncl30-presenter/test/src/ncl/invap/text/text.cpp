#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_invap_text_21 ) {
	ASSERT_TRUE( play("invap/text/21_style_weight_variant.ncl") );
	ASSERT_TRUE( compareTo("invap_21_style_weight_variant") );
}

TEST_F( Ginga, invap_text_22 ) {
	ASSERT_TRUE( play("invap/text/22_color_size.ncl") );
	ASSERT_TRUE( compareTo("invap_22_color_size") );
}

TEST_F( Ginga, invap_text_23 ) {
	ASSERT_TRUE( play("invap/text/23_position.ncl") );
	ASSERT_TRUE( compareTo("invap_23_position") );
}

TEST_F( Ginga, invap_text_24 ) {
	ASSERT_TRUE( play("invap/text/24_bounds.ncl") );
	ASSERT_TRUE( compareTo("invap_24_bounds") );
}

}
