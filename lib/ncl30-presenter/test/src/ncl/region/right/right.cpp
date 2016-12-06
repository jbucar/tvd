#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_right1 ) {
	ASSERT_TRUE( play("region/right/right1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, region_right2 ) {
	ASSERT_TRUE( play("region/right/right2.ncl") );
	ASSERT_TRUE( compareTo("right_1%") );
}
TEST_F( Ginga, region_right3 ) {
	ASSERT_TRUE( play("region/right/right3.ncl") );
	ASSERT_TRUE( compareTo("right_10%") );
}
TEST_F( Ginga, region_right4 ) {
	ASSERT_TRUE( play("region/right/right4.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}
TEST_F( Ginga, region_right5 ) {
	ASSERT_TRUE( play("region/right/right5.ncl") );
	ASSERT_TRUE( compareTo("right_99%") );
}
TEST_F( Ginga, region_right6 ) {
	ASSERT_TRUE( play("region/right/right6.ncl") );
	ASSERT_TRUE( compareTo("right_99_9%") );
}
TEST_F( Ginga, region_right7 ) {
	ASSERT_TRUE( play("region/right/right7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
// 
TEST_F( Ginga, region_right8 ) {
	ASSERT_TRUE( play("region/right/right8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_right9 ) {
	ASSERT_TRUE( play("region/right/right9.ncl") );
	ASSERT_TRUE( compareTo("blue180x576_red180x576_black360x576") );
}

TEST_F( Ginga, region_right10 ) {
	ASSERT_TRUE( play("region/right/right10.ncl") );
	ASSERT_TRUE( compareTo("blue180x576_red180x576_black360x576") );
}

TEST_F( Ginga, region_right11 ) {
	ASSERT_TRUE( play("region/right/right11.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_right12 ) {
	ASSERT_TRUE( play("region/right/right12.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, region_right13 ) {
	ASSERT_TRUE( play("region/right/right13.ncl") );
	ASSERT_TRUE( compareTo("right_10%") );
}

TEST_F( Ginga, region_right15 ) {
        ASSERT_TRUE( play("region/right/right15.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

}
