#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_descriptor_explicitDur_1 ) {
	ASSERT_TRUE( play("descriptor/explicitDur/explicitDur_1.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

}