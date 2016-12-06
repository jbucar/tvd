#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_dinamic_visible1 ) {
	ASSERT_TRUE( play("media/visible/dinamic_visible1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_visible2 ) {
	ASSERT_TRUE( play("media/visible/dinamic_visible2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_visible3 ) {
	ASSERT_TRUE( play("media/visible/dinamic_visible3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_visible4 ) {
	ASSERT_TRUE( play("media/visible/dinamic_visible4.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

}
