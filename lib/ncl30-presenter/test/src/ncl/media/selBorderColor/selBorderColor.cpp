#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, media_selBorderColor1 ) {
	ASSERT_TRUE( play("media/selBorderColor/selBorderColor1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_selBorderColor2 ) {
	ASSERT_TRUE( play("media/selBorderColor/selBorderColor2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_selBorderColor3 ) {
	ASSERT_TRUE( play("media/selBorderColor/selBorderColor3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
}

TEST_F( Ginga, media_selBorderColor4 ) {
	ASSERT_TRUE( play("media/selBorderColor/selBorderColor4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_selBorderColor5 ) {
	ASSERT_TRUE( play("media/selBorderColor/selBorderColor5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_-2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_-2px") );
}
}

