#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_width1 ) {
	ASSERT_TRUE( play("region/width/width1.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, region_width2 ) {
	ASSERT_TRUE( play("region/width/width2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_width3 ) {
	ASSERT_TRUE( play("region/width/width3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_width4 ) {
	ASSERT_TRUE( play("region/width/width4.ncl") );
	ASSERT_TRUE( compareTo("blue270x576_black450x576") );
}

TEST_F( Ginga, region_width5 ) {
	ASSERT_TRUE( play("region/width/width5.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, region_width6 ) {
	ASSERT_TRUE( play("region/width/width6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_width7 ) {
	ASSERT_TRUE( play("region/width/width7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_width8 ) {
	ASSERT_TRUE( play("region/width/width8.ncl") );
	ASSERT_TRUE( compareTo("blue108x576_red108x576_black504x576") );
}

TEST_F( Ginga, region_width9 ) {
	ASSERT_TRUE( play("region/width/width9.ncl") );
	ASSERT_TRUE( compareTo("blue108x576_red108x576_black504x576") );
}

TEST_F( Ginga, region_width10 ) {
	ASSERT_TRUE( play("region/width/width10.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, region_width11 ) {
        ASSERT_TRUE( play("region/width/width11.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

}
