#include "../../../ginga.h"

namespace ginga {
//test estaticos 

TEST_F( Ginga, media_top1 ) {
	ASSERT_TRUE( play("media/top/top1.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top2 ) {
	ASSERT_TRUE( play("media/top/top2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_top3 ) {
	ASSERT_TRUE( play("media/top/top3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_top4 ) {
	ASSERT_TRUE( play("media/top/top4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_top5 ) {
	ASSERT_TRUE( play("media/top/top5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_top6 ) {
	ASSERT_TRUE( play("media/top/top6.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, media_top7 ) {
	ASSERT_TRUE( play("media/top/top7.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top8 ) {
	ASSERT_TRUE( play("media/top/top8.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_top9 ) {
	ASSERT_TRUE( play("media/top/top9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_top10 ) {
	ASSERT_TRUE( play("media/top/top10.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, media_top11 ) {
	ASSERT_TRUE( play("media/top/top11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_top12 ) {
	ASSERT_TRUE( play("media/top/top12.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top13 ) {
	ASSERT_TRUE( play("media/top/top13.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top14 ) {
	ASSERT_TRUE( play("media/top/top14.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}
TEST_F( Ginga, media_top15 ) {
	ASSERT_TRUE( play("media/top/top15.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top17 ) {
	ASSERT_TRUE( play("media/top/top17.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top18 ) {
	ASSERT_TRUE( play("media/top/top18.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_top19 ) {
	ASSERT_TRUE( play("media/top/top19.ncl") );
	ASSERT_TRUE( compareTo("black720x252_blue720x324") );
}
TEST_F( Ginga, media_top20 ) {
	ASSERT_TRUE( play("media/top/top20.ncl") );
	ASSERT_TRUE( compareTo("black720x432_blue720x144") );
}
TEST_F( Ginga, media_top21 ) {
	ASSERT_TRUE( play("media/top/top21.ncl") );
	ASSERT_TRUE( compareTo("black720x252_blue720x324") );
}

TEST_F( Ginga, media_top22 ) {
	ASSERT_TRUE( play("media/top/top22.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_top23 ) {
	ASSERT_TRUE( play("media/top/top23.ncl") );
	ASSERT_TRUE( compareTo("black720x360") );
}

TEST_F( Ginga, media_top24 ) {
	ASSERT_TRUE( play("media/top/top24.ncl") );
	ASSERT_TRUE( compareTo("black720x360") );
}

}
//test dinamicos 
