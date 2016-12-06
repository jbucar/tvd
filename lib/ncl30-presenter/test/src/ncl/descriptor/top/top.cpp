#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_top1 ) {
	ASSERT_TRUE( play("descriptor/top/top1.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top2 ) {
	ASSERT_TRUE( play("descriptor/top/top2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top3 ) {
	ASSERT_TRUE( play("descriptor/top/top3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_top4 ) {
	ASSERT_TRUE( play("descriptor/top/top4.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, descriptor_top5 ) {
	ASSERT_TRUE( play("descriptor/top/top5.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top6) {
	ASSERT_TRUE( play("descriptor/top/top6.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top7 ) {
	ASSERT_TRUE( play("descriptor/top/top7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_top8 ) {
	ASSERT_TRUE( play("descriptor/top/top8.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top9 ) {
	ASSERT_TRUE( play("descriptor/top/top9.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top10 ) {
	ASSERT_TRUE( play("descriptor/top/top10.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top11 ) {
	ASSERT_TRUE( play("descriptor/top/top11.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, descriptor_top12 ) {
	ASSERT_TRUE( play("descriptor/top/top12.ncl") );
	ASSERT_TRUE( compareTo("black720x432_blue720x144") );
}

TEST_F( Ginga, descriptor_top13 ) {
	ASSERT_TRUE( play("descriptor/top/top13.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top14 ) {
	ASSERT_TRUE( play("descriptor/top/top14.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, descriptor_top15 ) {
	ASSERT_TRUE( play("descriptor/top/top15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top16 ) {
	ASSERT_TRUE( play("descriptor/top/top16.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top17 ) {
        ASSERT_TRUE( play("descriptor/top/top17.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_top18 ) {
        ASSERT_TRUE( play("descriptor/top/top18.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top19 ) {
        ASSERT_TRUE( play("descriptor/top/top19.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top20 ) {
        ASSERT_TRUE( play("descriptor/top/top20.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top21 ) {
        ASSERT_TRUE( play("descriptor/top/top21.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top22 ) {
        ASSERT_TRUE( play("descriptor/top/top22.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top23 ) {
        ASSERT_TRUE( play("descriptor/top/top23.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_top24 ) {
        ASSERT_TRUE( play("descriptor/top/top24.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_top25 ) {
        ASSERT_TRUE( play("descriptor/top/top25.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}


}
