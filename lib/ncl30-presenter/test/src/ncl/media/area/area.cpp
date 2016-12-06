#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_media_area_1 ) {
	ASSERT_TRUE( play("media/area/area_1.ncl") );
	ASSERT_TRUE( compareTo("green720x576") );
}

}