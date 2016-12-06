#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_bottom1 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom1.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, descriptor_bottom2 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom3 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bottom4 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom4.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, descriptor_bottom5 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, descriptor_bottom6 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bottom7 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom8 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom8.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, descriptor_bottom9 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom9.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, descriptor_bottom10 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom10.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}


TEST_F( Ginga, descriptor_bottom11 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom11.ncl") );
	ASSERT_TRUE( compareTo("blue720x360_black720x216") );
}

TEST_F( Ginga, descriptor_bottom12 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom12.ncl") );
	ASSERT_TRUE( compareTo("blue720x144_black720x432") );
}

TEST_F( Ginga, descriptor_bottom13 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom13.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, descriptor_bottom14 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom14.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, descriptor_bottom15 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom16 ) {
	ASSERT_TRUE( play("descriptor/bottom/bottom16.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom17 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom17.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bottom18 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom18.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom19 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom19.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom20 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom20.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom21 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom21.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom22 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom22.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom23 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom23.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom24 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom24.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bottom25 ) {
        ASSERT_TRUE( play("descriptor/bottom/bottom25.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

}
