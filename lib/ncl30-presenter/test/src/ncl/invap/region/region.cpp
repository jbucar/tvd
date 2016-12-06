#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, invap_region_1 ) {
	ASSERT_TRUE( play("invap/region/01_top.ncl") );
	ASSERT_TRUE( compareTo("invap_01_top") );
}

TEST_F( Ginga, invap_region_2 ) {
	ASSERT_TRUE( play("invap/region/02_left.ncl") );
	ASSERT_TRUE( compareTo("invap_02_left") );
}

TEST_F( Ginga, invap_region_3 ) {
	ASSERT_TRUE( play("invap/region/03_bottom.ncl") );
	ASSERT_TRUE( compareTo("invap_03_bottom") );
}

TEST_F( Ginga, invap_region_4 ) {
	ASSERT_TRUE( play("invap/region/04_right.ncl") );
	ASSERT_TRUE( compareTo("invap_04_right") );
}

TEST_F( Ginga, invap_region_5 ) {
	ASSERT_TRUE( play("invap/region/05_width.ncl") );
	ASSERT_TRUE( compareTo("invap_05_width") );
}

TEST_F( Ginga, invap_region_6 ) {
	ASSERT_TRUE( play("invap/region/06_height.ncl") );
	ASSERT_TRUE( compareTo("invap_06_height") );
}

TEST_F( Ginga, invap_region_7 ) {
	ASSERT_TRUE( play("invap/region/07_percentage_pixel.ncl") );
	ASSERT_TRUE( compareTo("invap_07_percentage_pixel") );
}

TEST_F( Ginga, invap_region_8 ) {
	ASSERT_TRUE( play("invap/region/08_nested_regions.ncl") );
	ASSERT_TRUE( compareTo("invap_08_nested_regions") );
}

TEST_F( Ginga, invap_region_9 ) {
	ASSERT_TRUE( play("invap/region/09_zIndex.ncl") );
	ASSERT_TRUE( compareTo("invap_09_zIndex") );
}

TEST_F( Ginga, invap_region_10 ) {
	ASSERT_TRUE( play("invap/region/10_precedence.ncl") );
	ASSERT_TRUE( compareTo("invap_10_precedence") );
}

TEST_F( Ginga, invap_region_11 ) {
	ASSERT_TRUE( play("invap/region/11_import_regions_base.ncl") );
	ASSERT_TRUE( compareTo("invap_11_import_regions_base") );
}
}
