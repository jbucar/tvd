#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_device1 ) {
	ASSERT_TRUE( play("region/device/device1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_device2 ) {
	ASSERT_TRUE( play("region/device/device2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_device3 ) {
	ASSERT_TRUE( play("region/device/device3.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_device4 ) {
	ASSERT_TRUE( play("region/device/device4.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_device5 ) {
	ASSERT_TRUE( play("region/device/device5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_device6 ) {
	ASSERT_TRUE( play("region/device/device6.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

};
