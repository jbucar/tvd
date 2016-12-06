#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_focusIndex0 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex0.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderGreen_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex1 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderRed_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderGreen_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex1a ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex1a.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderRed_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderGreen_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex2 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderGreen_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex3 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderGreen_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
}

TEST_F( Ginga, media_focusIndex4 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_down ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_right ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_up ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_left ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
}

TEST_F( Ginga, media_focusIndex5 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_down ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_right ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusIndex6 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex6.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_down ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_up ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex7 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_down ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, DISABLED_media_focusIndex7a ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex7a.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_redHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_up ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
}

TEST_F( Ginga, DISABLED_media_focusIndex8 ) {
	ASSERT_TRUE( play("media/focusIndex/focusIndex8.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_blueSelected") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_blueSelected") );
}

}
