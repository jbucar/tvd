#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_top1 ) {
	ASSERT_TRUE( play("region/top/top1.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, region_top2 ) {
	ASSERT_TRUE( play("region/top/top2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_top3 ) {
	ASSERT_TRUE( play("region/top/top3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_top4 ) {
	ASSERT_TRUE( play("region/top/top4.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, region_top5 ) {
	ASSERT_TRUE( play("region/top/top5.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, region_top6 ) {
	ASSERT_TRUE( play("region/top/top6.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, region_top7 ) {
	ASSERT_TRUE( play("region/top/top7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_top8 ) {
	ASSERT_TRUE( play("region/top/top8.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, region_top9 ) {
	ASSERT_TRUE( play("region/top/top9.ncl") );
	ASSERT_TRUE( compareTo("black720x432_blue720x144") );
}

TEST_F( Ginga, region_top10 ) {
	ASSERT_TRUE( play("region/top/top10.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, region_top11 ) {
	ASSERT_TRUE( play("region/top/top11.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, region_top12 ) {
        ASSERT_TRUE( play("region/top/top12.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, region_top13 ) {
        ASSERT_TRUE( play("region/top/top13.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}


}
