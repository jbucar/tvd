#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_height1 ) {
	ASSERT_TRUE( play("region/height/height1.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_height2 ) {
	ASSERT_TRUE( play("media/height/height2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height3 ) {
	ASSERT_TRUE( play("media/height/height3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
//
TEST_F( Ginga, media_height4 ) {
	ASSERT_TRUE( play("media/height/height4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height5 ) {
	ASSERT_TRUE( play("media/height/height5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height6 ) {
	ASSERT_TRUE( play("media/height/height6.ncl") );
	ASSERT_TRUE( compareTo("blue720x216_black720x360") );
}
//
TEST_F( Ginga, media_height7 ) {
	ASSERT_TRUE( play("media/height/height7.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_height8 ) {
	ASSERT_TRUE( play("media/height/height8.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
//
TEST_F( Ginga, media_height9 ) {
	ASSERT_TRUE( play("media/height/height9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height10 ) {
	ASSERT_TRUE( play("media/height/height10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height11 ) {
	ASSERT_TRUE( play("media/height/height11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );	
}

TEST_F( Ginga, media_height12 ) {
	ASSERT_TRUE( play("media/height/height12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height13 ) {
	ASSERT_TRUE( play("media/height/height13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height14 ) {
	ASSERT_TRUE( play("media/height/height14.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height15 ) {
	ASSERT_TRUE( play("media/height/height15.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_height16 ) {
	ASSERT_TRUE( play("media/height/height16.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height17 ) {
	ASSERT_TRUE( play("media/height/height17.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_height18 ) {
	ASSERT_TRUE( play("media/height/height18.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
//
TEST_F( Ginga, media_height19 ) {
	ASSERT_TRUE( play("media/height/height19.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_height20 ) {
	ASSERT_TRUE( play("media/height/height20.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_height21 ) {
	ASSERT_TRUE( play("media/height/height21.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_height22 ) {
	ASSERT_TRUE( play("media/height/height22.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_height23 ) {
	ASSERT_TRUE( play("media/height/height23.ncl") );
	ASSERT_TRUE( compareTo("blue720x144_black720x432") );
}
//
TEST_F( Ginga, media_height24 ) {
	ASSERT_TRUE( play("media/height/height24.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_height25 ) {
	ASSERT_TRUE( play("media/height/height25.ncl") );
	ASSERT_TRUE( compareTo("blue720x144_red720x144_black720x288") );
}

}
//tests dinamicos 
