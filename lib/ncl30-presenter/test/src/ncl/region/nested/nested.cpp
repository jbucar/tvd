#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_nested1 ) {
	ASSERT_TRUE( play("region/nested/nested1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

}
