#include "../../../ginga.h"

namespace ginga {
//test dinamicos

TEST_F( Ginga, media_dinamic_size1 ) {
	ASSERT_TRUE( play("media/size/dinamic_size1.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_dinamic_size2 ) {
	ASSERT_TRUE( play("media/size/dinamic_size2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_dinamic_size3 ) {
	ASSERT_TRUE( play("media/size/dinamic_size3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}
TEST_F( Ginga, media_dinamic_size4 ) {
	ASSERT_TRUE( play("media/size/dinamic_size4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, media_dinamic_size5 ) {
	ASSERT_TRUE( play("media/size/dinamic_size5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}
TEST_F( Ginga, media_dinamic_size6 ) {
	ASSERT_TRUE( play("media/size/dinamic_size6.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}
TEST_F( Ginga, media_dinamic_size7 ) {
	ASSERT_TRUE( play("media/size/dinamic_size7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}
TEST_F( Ginga, media_dinamic_size8 ) {
	ASSERT_TRUE( play("media/size/dinamic_size8.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}
}
