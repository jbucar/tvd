#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, invap_descriptor_12 ) {
	ASSERT_TRUE( play("invap/descriptor/12_top_height.ncl") );
	ASSERT_TRUE( compareTo("invap_12_top_height") );
}

TEST_F( Ginga, invap_descriptor_13 ) {
	ASSERT_TRUE( play("invap/descriptor/13_left_width.ncl") );
	ASSERT_TRUE( compareTo("invap_13_left_width") );
}

TEST_F( Ginga, invap_descriptor_14 ) {
	ASSERT_TRUE( play("invap/descriptor/14_location.ncl") );
	ASSERT_TRUE( compareTo("invap_14_location") );
}

TEST_F( Ginga, invap_descriptor_15 ) {
	ASSERT_TRUE( play("invap/descriptor/15_size.ncl") );
	ASSERT_TRUE( compareTo("invap_15_size") );
}

TEST_F( Ginga, invap_descriptor_16 ) {
	ASSERT_TRUE( play("invap/descriptor/16_bounds.ncl") );
	ASSERT_TRUE( compareTo("invap_16_bounds") );
}

TEST_F( Ginga, invap_descriptor_17 ) {
	ASSERT_TRUE( play("invap/descriptor/17_text_attributes.ncl") );
	ASSERT_TRUE( compareTo("invap_17_text_attributes") );
}

TEST_F( Ginga, DISABLED_invap_descriptor_19 ) {
	ASSERT_TRUE( play("invap/descriptor/19_key_navigation.ncl") );
	ASSERT_TRUE( compareTo("invap_19_key_navigation") );
}

}
