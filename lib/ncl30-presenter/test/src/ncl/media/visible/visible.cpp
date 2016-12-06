#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_visible1 ) {
	ASSERT_TRUE( play("media/visible/visible1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_visible2 ) {
	ASSERT_TRUE( play("media/visible/visible2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_visible3 ) {
	ASSERT_TRUE( play("media/visible/visible3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_visible4 ) {
	ASSERT_TRUE( play("media/visible/visible4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_visible5 ) {
	ASSERT_TRUE( play("media/visible/visible5.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_visible6 ) {
	ASSERT_TRUE( play("media/visible/visible6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

}
