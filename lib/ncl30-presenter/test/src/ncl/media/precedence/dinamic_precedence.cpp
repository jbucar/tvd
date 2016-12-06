#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_precedence_left_width_right_1 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_1.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}

TEST_F( Ginga, media_dinamic_precedence_left_width_right_2 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_2.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black576x576_blue72x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_3 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_3.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue72x576_black648x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_4 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_4.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_5 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_5.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black576x576_blue72x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_6 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_6.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue72x576_black648x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_7 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_7.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_8 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_8.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_9 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_9.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_10 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_10.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_11 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_11.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_12 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_12.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_13 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_13.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_left_width_right_14 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_left_width_right_14.ncl") );
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black72x576_blue576x576_black72x576") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_1 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_1.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_2 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_2.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x230_blue720x172_black720x174") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_3 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_3.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_4 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_4.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x172_blue720x288_black720x116") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_5 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_5.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_6 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_6.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x230_blue720x172_black720x174") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_7 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_7.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}
TEST_F( Ginga, media_dinamic_precedence_top_height_bottom_8 ) {
	ASSERT_TRUE( play("media/precedence/dinamic_precedence_top_height_bottom_8.ncl") );
	ASSERT_TRUE( compareTo("black720x172_blue720x172_black720x232") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x172_black720x404") );
}


}
