#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_left1 ) {
	ASSERT_TRUE( play("region/left/left1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, region_left2 ) {
	ASSERT_TRUE( play("region/left/left2.ncl") );
	ASSERT_TRUE( compareTo("left_1%") );
}
TEST_F( Ginga, region_left3 ) {
	ASSERT_TRUE( play("region/left/left3.ncl") );
	ASSERT_TRUE( compareTo("left_10%") );
}
TEST_F( Ginga, region_left4 ) {
	ASSERT_TRUE( play("region/left/left4.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}
TEST_F( Ginga, region_left5 ) {
	ASSERT_TRUE( play("region/left/left5.ncl") );
	ASSERT_TRUE( compareTo("left_99%") );
}
TEST_F( Ginga, region_left6 ) {
	ASSERT_TRUE( play("region/left/left6.ncl") );
	ASSERT_TRUE( compareTo("left_99_9%") );
}
TEST_F( Ginga, region_left7 ) {
	ASSERT_TRUE( play("region/left/left7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, region_left8 ) {
	ASSERT_TRUE( play("region/left/left8.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, region_left9 ) {
	ASSERT_TRUE( play("region/left/left9.ncl") );
	ASSERT_TRUE( compareTo("left_10%") );
}
TEST_F( Ginga, region_left10 ) {
	ASSERT_TRUE( play("region/left/left10.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}
TEST_F( Ginga, region_left11 ) {
	ASSERT_TRUE( play("region/left/left11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_left12 ) {
	ASSERT_TRUE( play("region/left/left12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_left13 ) {
	ASSERT_TRUE( play("region/left/left13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_left14 ) {
	ASSERT_TRUE( play("region/left/left14.ncl") );
	ASSERT_TRUE( compareTo("black540x576_blue180x576") );
}

TEST_F( Ginga, region_left15 ) {
	ASSERT_TRUE( play("region/left/left15.ncl") );
	ASSERT_TRUE( compareTo("black540x576_blue180x576") );
}

TEST_F( Ginga, region_left16 ) {
        ASSERT_TRUE( play("region/left/left16.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

}
