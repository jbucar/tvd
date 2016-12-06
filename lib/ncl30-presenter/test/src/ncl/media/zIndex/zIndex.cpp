#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_zIndex1 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex1.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}

TEST_F( Ginga, media_zIndex2 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex2.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}

TEST_F( Ginga, media_zIndex3 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex3.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex4 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex4.ncl") );
	ASSERT_TRUE( compareTo("zIndex_redOnly") );
}

TEST_F( Ginga, media_zIndex5 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex5.ncl") );
	ASSERT_TRUE( compareTo("zIndex_redOnly") );
}

TEST_F( Ginga, media_zIndex6 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex6.ncl") );
	ASSERT_TRUE( compareTo("zIndex_redOnly") );
}

TEST_F( Ginga, media_zIndex7 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex7.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}

TEST_F( Ginga, media_zIndex8 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex8.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}

TEST_F( Ginga, media_zIndex9 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex9.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex10 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex10.ncl") );
	ASSERT_TRUE( compareTo("zIndex_redOnly") );
}

TEST_F( Ginga, media_zIndex11 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex11.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex12 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_zIndex13 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex13.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex14 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex14.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex15 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex15.ncl") );
	ASSERT_TRUE( compareTo("redOverBlue") );
}

TEST_F( Ginga, media_zIndex16 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex16.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}

TEST_F( Ginga, media_zIndex17 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex17.ncl") );
	ASSERT_TRUE( compareTo("blueOverRed") );
}
TEST_F( Ginga, media_zIndex18 ) {
	ASSERT_TRUE( play("media/zIndex/zIndex18.ncl") );
	ASSERT_TRUE( compareTo("blueOverGreenOverRed") );
}

}
