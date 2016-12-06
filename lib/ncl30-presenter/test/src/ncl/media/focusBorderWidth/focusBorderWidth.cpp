#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, media_focusBorderWidth1 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusBorderWidth2 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth2.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
}

TEST_F( Ginga, media_focusBorderWidth3 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_-2px") );
}

TEST_F( Ginga, media_focusBorderWidth4 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_720px") );
}

TEST_F( Ginga, media_focusBorderWidth5 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_360px") );
}

TEST_F( Ginga, media_focusBorderWidth5a ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth5a.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
}

TEST_F( Ginga, media_focusBorderWidth6 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_focusBorderWidth7 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_focusBorderWidth8 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth8.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_origin_borderWhite_2px") );
}

TEST_F( Ginga, media_focusBorderWidth9 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth9.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_origin_borderWhite_2px") );
}

TEST_F( Ginga, media_focusBorderWidth10 ) {
	ASSERT_TRUE( play("media/focusBorderWidth/focusBorderWidth10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
}
