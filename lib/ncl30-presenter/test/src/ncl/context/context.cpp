#include "../../ginga.h"

namespace ginga {

TEST_F( Ginga, context_1 ) {
	ASSERT_TRUE( play("context/topCtx.ncl") );
	ASSERT_TRUE( compareTo("black720x288_blue720x288") );
}
}
