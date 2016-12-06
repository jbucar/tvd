#include "../../../ginga.h"

namespace ginga {
//estaticos

TEST_F( Ginga, media_precedence_left_width_right_1 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_1.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_2 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_2.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_3 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_3.ncl") );
	ASSERT_TRUE( compareTo("black576x576_blue72x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_4 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_4.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_5 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_5.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_6 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_6.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_7 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_8 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_8.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue72x576_black288x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_9 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_10 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_10.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_11 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_12 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_12.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_13 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_13.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue504x576_black144x576") );
}

TEST_F( Ginga, media_precedence_left_width_right_14 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_14.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue504x576_black144x576") );
}
TEST_F( Ginga, media_precedence_left_width_right_15 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_15.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue504x576_black144x576") );
}
TEST_F( Ginga, media_precedence_left_width_right_16 ) {
	ASSERT_TRUE( play("media/precedence/precedence_left_width_right_16.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue504x576_black144x576") );
}
// END precedence tests between left,width and right


// BEGIN precedence tests between top, height and bottom

TEST_F( Ginga, media_precedence_top_height_bottom1 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom1.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x216_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom2 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom2.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x216_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom3 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom3.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x216_black720x72") );
}

TEST_F( Ginga, media_precedence_top_height_bottom4 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom4.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_precedence_top_height_bottom5 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom5.ncl") );
	ASSERT_TRUE( compareTo("black720x130_blue720x216_black720x360") );
}

TEST_F( Ginga, media_precedence_top_height_bottom6 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom6.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x72_black720x216") );
}

TEST_F( Ginga, media_precedence_top_height_bottom7 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom7.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x216_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom8 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_precedence_top_height_bottom10 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom10.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x216_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom11 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom11.ncl") );
	ASSERT_TRUE( compareTo("black720x72_blue720x360_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom12 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom12.ncl") );
	ASSERT_TRUE( compareTo("black720x72_blue720x360_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom13 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom13.ncl") );
	ASSERT_TRUE( compareTo("black720x72_blue720x360_black720x144") );
}

TEST_F( Ginga, media_precedence_top_height_bottom14 ) {
	ASSERT_TRUE( play("media/precedence/precedence_top_height_bottom14.ncl") );
	ASSERT_TRUE( compareTo("black720x72_blue720x360_black720x144") );
}
// END precedence tests between top_height_bottom
}

//dinamicos
