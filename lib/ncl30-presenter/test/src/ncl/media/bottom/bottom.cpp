#include "../../../ginga.h"

namespace ginga {//test de propiedades estaticas

TEST_F( Ginga, media_bottom1 ) {
	ASSERT_TRUE( play("media/bottom/bottom1.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_bottom2 ) {
	ASSERT_TRUE( play("media/bottom/bottom2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_bottom3 ) {
	ASSERT_TRUE( play("media/bottom/bottom3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom4 ) {
	ASSERT_TRUE( play("media/bottom/bottom4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom5 ) {
	ASSERT_TRUE( play("media/bottom/bottom5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom6 ) {
	ASSERT_TRUE( play("media/bottom/bottom6.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, media_bottom7 ) {
	ASSERT_TRUE( play("media/bottom/bottom7.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_bottom8 ) {
	ASSERT_TRUE( play("media/bottom/bottom8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom9 ) {
	ASSERT_TRUE( play("media/bottom/bottom9.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
//
TEST_F( Ginga, media_bottom10 ) {
	ASSERT_TRUE( play("media/bottom/bottom10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
//
TEST_F( Ginga, media_bottom11 ) {
	ASSERT_TRUE( play("media/bottom/bottom11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );	
}

TEST_F( Ginga, media_bottom12 ) {
	ASSERT_TRUE( play("media/bottom/bottom12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom13 ) {
	ASSERT_TRUE( play("media/bottom/bottom13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom14 ) {
	ASSERT_TRUE( play("media/bottom/bottom14.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom15 ) {
	ASSERT_TRUE( play("media/bottom/bottom15.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_bottom16 ) {
	ASSERT_TRUE( play("media/bottom/bottom16.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom17 ) {
	ASSERT_TRUE( play("media/bottom/bottom17.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_bottom18 ) {
	ASSERT_TRUE( play("media/bottom/bottom18.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}
//
TEST_F( Ginga, media_bottom19 ) {
	ASSERT_TRUE( play("media/bottom/bottom19.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_bottom20 ) {
	ASSERT_TRUE( play("media/bottom/bottom20.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_bottom21 ) {
	ASSERT_TRUE( play("media/bottom/bottom21.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}
//
TEST_F( Ginga, media_bottom22 ) {
	ASSERT_TRUE( play("media/bottom/bottom22.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_bottom23 ) {
	ASSERT_TRUE( play("media/bottom/bottom23.ncl") );
	ASSERT_TRUE( compareTo("blue720x144_black720x432") );
}

TEST_F( Ginga, media_bottom24 ) {
	ASSERT_TRUE( play("media/bottom/bottom24.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_bottom25 ) {
	ASSERT_TRUE( play("media/bottom/bottom25.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

}
