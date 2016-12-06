#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_top1 ) {
	ASSERT_TRUE( play("media/top/dinamic_top1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top2 ) {
	ASSERT_TRUE( play("media/top/dinamic_top2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top3 ) {
	ASSERT_TRUE( play("media/top/dinamic_top3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top4 ) {
	ASSERT_TRUE( play("media/top/dinamic_top4.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top5 ) {
	ASSERT_TRUE( play("media/top/dinamic_top5.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top6 ) {
	ASSERT_TRUE( play("media/top/dinamic_top6.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top7 ) {
	ASSERT_TRUE( play("media/top/dinamic_top7.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top8 ) {
	ASSERT_TRUE( play("media/top/dinamic_top8.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top9 ) {
	ASSERT_TRUE( play("media/top/dinamic_top9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top10 ) {
	ASSERT_TRUE( play("media/top/dinamic_top10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top11 ) {
	ASSERT_TRUE( play("media/top/dinamic_top11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top12 ) {
	ASSERT_TRUE( play("media/top/dinamic_top12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top13 ) {
	ASSERT_TRUE( play("media/top/dinamic_top13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top14 ) {
	ASSERT_TRUE( play("media/top/dinamic_top14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top15 ) {
	ASSERT_TRUE( play("media/top/dinamic_top15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top16 ) {
	ASSERT_TRUE( play("media/top/dinamic_top16.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top17 ) {
	ASSERT_TRUE( play("media/top/dinamic_top17.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top18 ) {
	ASSERT_TRUE( play("media/top/dinamic_top18.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top19 ) {
	ASSERT_TRUE( play("media/top/dinamic_top19.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_top20 ) {
	ASSERT_TRUE( play("media/top/dinamic_top20.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

TEST_F( Ginga, media_dinamic_top21 ) {
	ASSERT_TRUE( play("media/top/dinamic_top21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top22 ) {
	ASSERT_TRUE( play("media/top/dinamic_top22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top23 ) {
	ASSERT_TRUE( play("media/top/dinamic_top23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top24 ) {
	ASSERT_TRUE( play("media/top/dinamic_top24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_top25 ) {
	ASSERT_TRUE( play("media/top/dinamic_top25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}

}

