#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_zIndex1 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex1.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, descriptor_zIndex2 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex2.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, descriptor_zIndex3 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex3.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}
TEST_F( Ginga, descriptor_zIndex4 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex4.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}
TEST_F( Ginga, descriptor_zIndex5 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex5.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}
TEST_F( Ginga, descriptor_zIndex6 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex6.ncl") );
	ASSERT_TRUE( compareTo("greenOverBlue") );
}
TEST_F( Ginga, descriptor_zIndex7 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex7.ncl") );
	ASSERT_TRUE( compareTo("redOverTextOverBlue") );
}
TEST_F( Ginga, descriptor_zIndex8 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex8.ncl") );
	ASSERT_TRUE( compareTo("redOverBlueOverGreen") );
}
TEST_F( Ginga, descriptor_zIndex9 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex9.ncl") );
	ASSERT_TRUE( compareTo("blueOverRedOverGreen") );
}
TEST_F( Ginga, descriptor_zIndex10 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex10.ncl") );
	ASSERT_TRUE( compareTo("blue360x288") );
}
TEST_F( Ginga, descriptor_zIndex11 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex11.ncl") );
	ASSERT_TRUE( compareTo("blue360x288") );
}
TEST_F( Ginga, descriptor_zIndex12 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex12.ncl") );
	ASSERT_TRUE( compareTo("blueOverRedOverGreen2") );
}
TEST_F( Ginga, descriptor_zIndex13 ) {
	ASSERT_TRUE( play("descriptor/zIndex/zIndex13.ncl") );
	ASSERT_TRUE( compareTo("blue360x288") );
}

}
