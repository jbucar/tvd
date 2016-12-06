#include "../../../ginga.h"

namespace ginga {

/*
TEST_F( Ginga, region_import1 ) {
	ASSERT_TRUE( play("region/import/import1.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
*/
TEST_F( Ginga, region_import2 ) {
	ASSERT_TRUE( play("region/import/import2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}
/*
TEST_F( Ginga, region_import3 ) {
        ASSERT_TRUE( play("region/import/import3.ncl") );
        ASSERT_TRUE( compareTo("blue360x288") );
}
*/
};
