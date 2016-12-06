#include "../../../ginga.h"

namespace ginga {

//test estaticos
TEST_F( Ginga, withOutRegion1 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, withOutRegion2 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, withOutRegion3 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, withOutRegion4 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion4.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, withOutRegion5 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, withOutRegion6 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion6.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, withOutRegion7 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, withOutRegion8 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, withOutRegion9 ) {
	ASSERT_TRUE( play("media/withOutRegion/withOutRegion7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

}
//test dinamicos

