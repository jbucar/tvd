#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, descriptor_width1 ) {
	ASSERT_TRUE( play("descriptor/width/width1.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width2 ) {
	ASSERT_TRUE( play("descriptor/width/width2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width3 ) {
	ASSERT_TRUE( play("descriptor/width/width3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_width4 ) {
	ASSERT_TRUE( play("descriptor/width/width4.ncl") );
	ASSERT_TRUE( compareTo("blue270x576_black450x576") );
}

TEST_F( Ginga, descriptor_width5 ) {
	ASSERT_TRUE( play("descriptor/width/width5.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width6 ) {
	ASSERT_TRUE( play("descriptor/width/width6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_width7 ) {
	ASSERT_TRUE( play("descriptor/width/width7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width8 ) {
	ASSERT_TRUE( play("descriptor/width/width8.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width9 ) {
	ASSERT_TRUE( play("descriptor/width/width9.ncl") );
	ASSERT_TRUE( compareTo("blue108x576_black612x576") );
}

TEST_F( Ginga, descriptor_width10 ) {
	ASSERT_TRUE( play("descriptor/width/width10.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width11 ) {
	ASSERT_TRUE( play("descriptor/width/width11.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}


TEST_F( Ginga, descriptor_width14 ) {
	ASSERT_TRUE( play("descriptor/width/width14.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width15 ) {
	ASSERT_TRUE( play("descriptor/width/width15.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width16 ) {
	ASSERT_TRUE( play("descriptor/width/width16.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width17 ) {
	ASSERT_TRUE( play("descriptor/width/width17.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width18 ) {
	ASSERT_TRUE( play("descriptor/width/width18.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width19 ) {
        ASSERT_TRUE( play("descriptor/width/width19.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_width20 ) {
        ASSERT_TRUE( play("descriptor/width/width20.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width21 ) {
        ASSERT_TRUE( play("descriptor/width/width21.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width22 ) {
        ASSERT_TRUE( play("descriptor/width/width22.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width23 ) {
        ASSERT_TRUE( play("descriptor/width/width23.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width25 ) {
        ASSERT_TRUE( play("descriptor/width/width25.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width26 ) {
        ASSERT_TRUE( play("descriptor/width/width26.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width27 ) {
        ASSERT_TRUE( play("descriptor/width/width27.ncl") );
        ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_width28 ) {
        ASSERT_TRUE( play("descriptor/width/width28.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_width29 ) {
        ASSERT_TRUE( play("descriptor/width/width29.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

}
