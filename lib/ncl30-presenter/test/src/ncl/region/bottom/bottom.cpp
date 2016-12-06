#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_bottom1 ) {
	ASSERT_TRUE( play("region/bottom/bottom1.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_bottom2 ) {
	ASSERT_TRUE( play("region/bottom/bottom2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_bottom3 ) {
	ASSERT_TRUE( play("region/bottom/bottom3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_bottom4 ) {
	ASSERT_TRUE( play("region/bottom/bottom4.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, region_bottom5 ) {
	ASSERT_TRUE( play("region/bottom/bottom5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_bottom6 ) {
	ASSERT_TRUE( play("region/bottom/bottom6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_bottom7 ) {
	ASSERT_TRUE( play("region/bottom/bottom7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_bottom8 ) {
	ASSERT_TRUE( play("region/bottom/bottom8.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, region_bottom9 ) {
	ASSERT_TRUE( play("region/bottom/bottom9.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, region_bottom10 ) {
	ASSERT_TRUE( play("region/bottom/bottom10.ncl") );
	ASSERT_TRUE( compareTo("blue720x144_black720x432") );
}

TEST_F( Ginga, region_bottom11 ) {
        ASSERT_TRUE( play("region/bottom/bottom11.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}


};
