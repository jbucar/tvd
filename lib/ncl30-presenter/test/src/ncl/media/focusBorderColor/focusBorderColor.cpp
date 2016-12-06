#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, media_focusBorderColor1 ) {
	ASSERT_TRUE( play("media/focusBorderColor/focusBorderColor1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusBorderColor2 ) {
	ASSERT_TRUE( play("media/focusBorderColor/focusBorderColor2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
}

TEST_F( Ginga, media_focusBorderColor3 ) {
	ASSERT_TRUE( play("media/focusBorderColor/focusBorderColor3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_focusBorderColor4 ) {
	ASSERT_TRUE( play("media/focusBorderColor/focusBorderColor4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusBorderColor5 ) {
	ASSERT_TRUE( play("media/focusBorderColor/focusBorderColor5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
}
