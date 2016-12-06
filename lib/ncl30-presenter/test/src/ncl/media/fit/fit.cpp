#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_fit0 ) {
	ASSERT_TRUE( play("media/fit/fit0.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fit1 ) {
	ASSERT_TRUE( play("media/fit/fit1.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit2 ) {
	ASSERT_TRUE( play("media/fit/fit2.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit3 ) {
	ASSERT_TRUE( play("media/fit/fit3.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit4 ) {
	ASSERT_TRUE( play("media/fit/fit4.ncl") );
	ASSERT_TRUE( compareTo("black720x216_black270x360_image225x180") );
}

TEST_F( Ginga, media_fit5 ) {
	ASSERT_TRUE( play("media/fit/fit5.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit5a) {
	ASSERT_TRUE( play("media/fit/fit5a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit6 ) {
	ASSERT_TRUE( play("media/fit/fit6.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit6a) {
	ASSERT_TRUE( play("media/fit/fit6a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit7 ) {
	ASSERT_TRUE( play("media/fit/fit7.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit8 ) {
	ASSERT_TRUE( play("media/fit/fit8.ncl") );
	ASSERT_TRUE( compareTo("image360x288_clipped_left_corner") );
}

TEST_F( Ginga, media_fit9 ) {
	ASSERT_TRUE( play("media/fit/fit9.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit10 ) {
	ASSERT_TRUE( play("media/fit/fit10.ncl") );
	ASSERT_TRUE( compareTo("black720x50_black50x526_image670x536_clipped") );
}

TEST_F( Ginga, media_fit11 ) {
	ASSERT_TRUE( play("media/fit/fit11.ncl") );
	ASSERT_TRUE( compareTo("image360x288_black360x576_clipped") );
}
TEST_F( Ginga, media_fit11a) {
	ASSERT_TRUE( play("media/fit/fit11a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_black360x576_clipped") );
}
TEST_F( Ginga, media_fit12 ) {
	ASSERT_TRUE( play("media/fit/fit12.ncl") );
	ASSERT_TRUE( compareTo("image360x288_black360x576_clipped") );
}
TEST_F( Ginga, media_fit12a ) {
	ASSERT_TRUE( play("media/fit/fit12a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_black360x576_clipped") );
}
TEST_F( Ginga, media_fit13 ) {
	ASSERT_TRUE( play("media/fit/fit13.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit14 ) {
	ASSERT_TRUE( play("media/fit/fit14.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit15 ) {
	ASSERT_TRUE( play("media/fit/fit15.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, DISABLED_media_fit16 ) {
	ASSERT_TRUE( play("media/fit/fit16.ncl") );
	ASSERT_TRUE( compareTo("black720x50_black50x526_image670x536_resize_meet") );
}

TEST_F( Ginga, media_fit17 ) {
	ASSERT_TRUE( play("media/fit/fit17.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit17a ) {
	ASSERT_TRUE( play("media/fit/fit17a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit18 ) {
	ASSERT_TRUE( play("media/fit/fit18.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit18a ) {
	ASSERT_TRUE( play("media/fit/fit18a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit19 ) {
	ASSERT_TRUE( play("media/fit/fit19.ncl") );
	ASSERT_TRUE( compareTo("image576x720") );
}

TEST_F( Ginga, media_fit20 ) {
	ASSERT_TRUE( play("media/fit/fit21.ncl") );
	ASSERT_TRUE( compareTo("black576x720") );
}

TEST_F( Ginga, media_fit22 ) {
	ASSERT_TRUE( play("media/fit/fit22.ncl") );
	ASSERT_TRUE( compareTo("image576x720") );
}

TEST_F( Ginga, DISABLED_media_fit23 ) {
	ASSERT_TRUE( play("media/fit/fit23.ncl") );
	ASSERT_TRUE( compareTo("black720x50_black50x526_image670x536_resize_meet") );
}

TEST_F( Ginga, media_fit24 ) {
	ASSERT_TRUE( play("media/fit/fit24.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit24a ) {
	ASSERT_TRUE( play("media/fit/fit24a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit25 ) {
	ASSERT_TRUE( play("media/fit/fit25.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit25a ) {
	ASSERT_TRUE( play("media/fit/fit25a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit26 ) {
	ASSERT_TRUE( play("media/fit/fit26.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_fit27 ) {
	ASSERT_TRUE( play("media/fit/fit27.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit28 ) {
	ASSERT_TRUE( play("media/fit/fit28.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_fit29 ) {
	ASSERT_TRUE( play("media/fit/fit29.ncl") );
	ASSERT_TRUE( compareTo("black720x50_black50x526_image670x536_resize") );
}

TEST_F( Ginga, media_fit30 ) {
	ASSERT_TRUE( play("media/fit/fit30.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit30a ) {
	ASSERT_TRUE( play("media/fit/fit30a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit31 ) {
	ASSERT_TRUE( play("media/fit/fit31.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_fit31a ) {
	ASSERT_TRUE( play("media/fit/fit31a.ncl") );
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

}
