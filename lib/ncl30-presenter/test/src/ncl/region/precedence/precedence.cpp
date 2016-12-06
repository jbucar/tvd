#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_precedence_left_width_right1 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right1.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right2 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right2.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right3 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right3.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right4 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right4.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, region_precedence_left_width_right5 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right5.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right6 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right6.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right7 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_precedence_left_width_right8 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right8.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue72x567_black288x576") );
}

TEST_F( Ginga, region_precedence_left_width_right9 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_precedence_left_width_right10 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right10.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right11 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right11.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue567x567_black72x576") );
}

TEST_F( Ginga, region_precedence_left_width_right12 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_precedence_left_width_right13 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right13.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue504x576_black144x576") );
}

TEST_F( Ginga, region_precedence_left_width_right14 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right14.ncl") );
	ASSERT_TRUE( compareTo("black144x576_blue518x576_black58x576") );
}

TEST_F( Ginga, region_precedence_left_width_right15 ) {
	ASSERT_TRUE( play("region/precedence/precedence_left_width_right15.ncl") );
	ASSERT_TRUE( compareTo("black100x576_blue520x576_black100x576") );
}
// END precedence tests between left,width and right

// BEGIN precedence tests between top, height and bottom

TEST_F( Ginga, region_precedence_top_height_bottom1 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom1.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom2 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom2.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom3 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom3.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom4 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom4.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, region_precedence_top_height_bottom5 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom5.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom6 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_precedence_top_height_bottom7 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom7.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom8 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom8.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom9 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom9.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom10 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom10.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom11 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_precedence_top_height_bottom12 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom12.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue720x288_black720x144") );
}

TEST_F( Ginga, region_precedence_top_height_bottom13 ) {
	ASSERT_TRUE( play("region/precedence/precedence_top_height_bottom13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
// END precedence tests between left,width and right
}
