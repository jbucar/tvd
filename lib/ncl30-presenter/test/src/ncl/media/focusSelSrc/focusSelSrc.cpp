#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_focusSelSrc1 ) {
	ASSERT_TRUE( play("media/focusSelSrc/focusSelSrc1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("red360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusSelSrc2 ) {
	ASSERT_TRUE( play("media/focusSelSrc/focusSelSrc2.ncl") );
	ASSERT_TRUE( compareTo("red360x288_centered_borderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::enter ));
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

}
