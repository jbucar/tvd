#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_height1 ) {
	ASSERT_TRUE( play("media/height/dinamic_height1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height2 ) {
	ASSERT_TRUE( play("media/height/dinamic_height2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height3 ) {
	ASSERT_TRUE( play("media/height/dinamic_height3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height4 ) {
	ASSERT_TRUE( play("media/height/dinamic_height4.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height5 ) {
	ASSERT_TRUE( play("media/height/dinamic_height5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height6 ) {
	ASSERT_TRUE( play("media/height/dinamic_height6.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height7 ) {
	ASSERT_TRUE( play("media/height/dinamic_height7.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height8 ) {
	ASSERT_TRUE( play("media/height/dinamic_height8.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height9 ) {
	ASSERT_TRUE( play("media/height/dinamic_height9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height10 ) {
	ASSERT_TRUE( play("media/height/dinamic_height10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height11 ) {
	ASSERT_TRUE( play("media/height/dinamic_height11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height12 ) {
	ASSERT_TRUE( play("media/height/dinamic_height12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height13 ) {
	ASSERT_TRUE( play("media/height/dinamic_height13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height14 ) {
	ASSERT_TRUE( play("media/height/dinamic_height14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height15 ) {
	ASSERT_TRUE( play("media/height/dinamic_height15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height16 ) {
	ASSERT_TRUE( play("media/height/dinamic_height16.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height17 ) {
	ASSERT_TRUE( play("media/height/dinamic_height17.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height18 ) {
	ASSERT_TRUE( play("media/height/dinamic_height18.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height19 ) {
	ASSERT_TRUE( play("media/height/dinamic_height19.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_height20 ) {
	ASSERT_TRUE( play("media/height/dinamic_height20.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_height21 ) {
	ASSERT_TRUE( play("media/height/dinamic_height21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height22 ) {
	ASSERT_TRUE( play("media/height/dinamic_height22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height23 ) {
	ASSERT_TRUE( play("media/height/dinamic_height23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height24 ) {
	ASSERT_TRUE( play("media/height/dinamic_height24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_height25 ) {
	ASSERT_TRUE( play("media/height/dinamic_height25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
 	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

}
