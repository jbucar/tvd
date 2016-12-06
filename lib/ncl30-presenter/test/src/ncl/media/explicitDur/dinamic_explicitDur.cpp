#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_media_dinamic_explicitDur_1 ) {
	ASSERT_TRUE( play("media/explicitDur/dinamic_explicitDur_1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

}