#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_dinamic_fit1 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit1.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_dinamic_fit2 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit2.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_dinamic_fit3 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit3.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image360x288_black360x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_fit4 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit4.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image720x576") );
}

TEST_F( Ginga, media_dinamic_fit5 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit5.ncl") );
	ASSERT_TRUE( compareTo("image360x288_black360x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}

TEST_F( Ginga, media_dinamic_fit6 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit6.ncl") );
	ASSERT_TRUE( compareTo("image360x288_clipped_left_corner") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image360x288_resize_left_corner") );
}
TEST_F( Ginga, media_dinamic_fit7 ) {
	ASSERT_TRUE( play("media/fit/dinamic_fit7.ncl") );
	ASSERT_TRUE( compareTo("image720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("image720x576") );
}

}
