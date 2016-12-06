#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, region_zIndex1 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex1.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, region_zIndex2 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex2.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, region_zIndex3 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex3.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}
TEST_F( Ginga, region_zIndex4 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex4.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, region_zIndex5 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex5.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}
TEST_F( Ginga, region_zIndex6 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex6.ncl") );
	ASSERT_TRUE( compareTo("redOverTextOverBlue") );
}
TEST_F( Ginga, region_zIndex7 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex7.ncl") );
	ASSERT_TRUE( compareTo("greenOverBlue") );
}

TEST_F( Ginga, region_zIndex8 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex8.ncl") );
	ASSERT_TRUE( compareTo("redOverBlueOverGreen") );
}
TEST_F( Ginga, region_zIndex9 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex9.ncl") );
	ASSERT_TRUE( compareTo("blueOverRedOverGreen") );
}
TEST_F( Ginga, region_zIndex10 ) {
	ASSERT_TRUE( play("region/zIndex/zIndex10.ncl") );
	ASSERT_TRUE( compareTo("redOverGreenOverBlue2") );
}

}
