#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, descriptor_left1 ) {
	ASSERT_TRUE( play("descriptor/left/left1.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left2 ) {
	ASSERT_TRUE( play("descriptor/left/left2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_left3 ) {
	ASSERT_TRUE( play("descriptor/left/left3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left4 ) {
	ASSERT_TRUE( play("descriptor/left/left4.ncl") );
	ASSERT_TRUE( compareTo("black270x576_blue450x576") );
}

TEST_F( Ginga, descriptor_left5 ) {
	ASSERT_TRUE( play("descriptor/left/left5.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left6 ) {
	ASSERT_TRUE( play("descriptor/left/left6.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left7 ) {
	ASSERT_TRUE( play("descriptor/left/left7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_left8 ) {
	ASSERT_TRUE( play("descriptor/left/left8.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left9 ) {
	ASSERT_TRUE( play("descriptor/left/left9.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left10 ) {
	ASSERT_TRUE( play("descriptor/left/left10.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}


TEST_F( Ginga, descriptor_left11 ) {
	ASSERT_TRUE( play("descriptor/left/left11.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left12 ) {
	ASSERT_TRUE( play("descriptor/left/left12.ncl") );
	ASSERT_TRUE( compareTo("black540x576_blue180x576") );
}

TEST_F( Ginga, descriptor_left13 ) {
	ASSERT_TRUE( play("descriptor/left/left13.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left14 ) {
	ASSERT_TRUE( play("descriptor/left/left14.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_left15 ) {
        ASSERT_TRUE( play("descriptor/left/left15.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_left16 ) {
        ASSERT_TRUE( play("descriptor/left/left16.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left17 ) {
        ASSERT_TRUE( play("descriptor/left/left17.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left18 ) {
        ASSERT_TRUE( play("descriptor/left/left18.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left19 ) {
        ASSERT_TRUE( play("descriptor/left/left19.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left21 ) {
        ASSERT_TRUE( play("descriptor/left/left21.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left22 ) {
        ASSERT_TRUE( play("descriptor/left/left22.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_left23 ) {
        ASSERT_TRUE( play("descriptor/left/left23.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}


}
