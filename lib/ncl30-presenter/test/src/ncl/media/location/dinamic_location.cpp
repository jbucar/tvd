#include "../../../ginga.h"

namespace ginga {
//test dinamicos

TEST_F( Ginga, media_dinamic_location1 ) {
	ASSERT_TRUE( play("media/location/dinamic_location1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}
TEST_F( Ginga, media_dinamic_location2 ) {
	ASSERT_TRUE( play("media/location/dinamic_location2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, media_dinamic_location3 ) {
	ASSERT_TRUE( play("media/location/dinamic_location3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, media_dinamic_location4 ) {
	ASSERT_TRUE( play("media/location/dinamic_location4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}
TEST_F( Ginga, media_dinamic_location5 ) {
	ASSERT_TRUE( play("media/location/dinamic_location5.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}
TEST_F( Ginga, media_dinamic_location6 ) {
	ASSERT_TRUE( play("media/location/dinamic_location6.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}
TEST_F( Ginga, media_dinamic_location7 ) {
	ASSERT_TRUE( play("media/location/dinamic_location7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_dinamic_location8 ) {
	ASSERT_TRUE( play("media/location/dinamic_location8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
}
