#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_visible1 ) {
	ASSERT_TRUE( play("descriptor/visible/visible1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_visible2 ) {
	ASSERT_TRUE( play("descriptor/visible/visible2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_visible3 ) {
        ASSERT_TRUE( play("descriptor/visible/visible3.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_visible4 ) {
        ASSERT_TRUE( play("descriptor/visible/visible4.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

}
