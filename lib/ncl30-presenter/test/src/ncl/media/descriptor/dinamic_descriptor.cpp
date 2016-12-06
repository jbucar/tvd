#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_media_dinamic_descriptor_1 ) {
	ASSERT_TRUE( play("media/explicitDur/dinamic_descriptor_1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

}