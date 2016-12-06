#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_height1 ) {
	ASSERT_TRUE( play("region/height/height1.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_height2 ) {
	ASSERT_TRUE( play("region/height/height2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_height3 ) {
	ASSERT_TRUE( play("region/height/height3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_height4 ) {
	ASSERT_TRUE( play("region/height/height4.ncl") );
	ASSERT_TRUE( compareTo("blue720x216_black720x360") );
}

TEST_F( Ginga, region_height5 ) {
	ASSERT_TRUE( play("region/height/height5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_height6 ) {
	ASSERT_TRUE( play("region/height/height6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_height7 ) {
	ASSERT_TRUE( play("region/height/height7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_height8 ) {
	ASSERT_TRUE( play("region/height/height8.ncl") );
	ASSERT_TRUE( compareTo("blue720x108_red720x108_black720x360") );
}

TEST_F( Ginga, region_height9 ) {
	ASSERT_TRUE( play("region/height/height9.ncl") );
	ASSERT_TRUE( compareTo("blue720x108_red720x108_black720x360") );
}

TEST_F( Ginga, region_height10 ) {
	ASSERT_TRUE( play("region/height/height10.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_height11 ) {
        ASSERT_TRUE( play("region/height/height11.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

}
