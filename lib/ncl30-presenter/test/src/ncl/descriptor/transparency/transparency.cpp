#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_transparency1 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency1.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
}

TEST_F( Ginga, descriptor_transparency2 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency3 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_transparency4 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency4.ncl") );
	ASSERT_TRUE( compareTo("transparency37.5") );
}

TEST_F( Ginga, descriptor_transparency5 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency5.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency6 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency6.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
}

TEST_F( Ginga, descriptor_transparency7 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_transparency8 ) { 
	ASSERT_TRUE( play("descriptor/transparency/transparency8.ncl") );
	ASSERT_TRUE( compareTo("transparency0.5%") );
}

TEST_F( Ginga, descriptor_transparency9 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency9.ncl") );
	ASSERT_TRUE( compareTo("transparency70%red50%blue") );
}

TEST_F( Ginga, descriptor_transparency10 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency10.ncl") );
	ASSERT_TRUE( compareTo("transparency70%red50%blue") );
}

TEST_F( Ginga, descriptor_transparency11 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency11.ncl") );
	ASSERT_TRUE( compareTo("transparency70%red50%blue_2") );
}

TEST_F( Ginga, descriptor_transparency12 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency12.ncl") );
	ASSERT_TRUE( compareTo("transparency0%red50%blue") );
}

TEST_F( Ginga, descriptor_transparency13 ) {
	ASSERT_TRUE( play("descriptor/transparency/transparency13.ncl") );
	ASSERT_TRUE( compareTo("transparency0%red50%blue_2") );
}

TEST_F( Ginga, descriptor_transparency14 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency14.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency15 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency15.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency16 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency16.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency17 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency17.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency18 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency18.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency19 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency19.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency20 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency20.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency21 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency21.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency22 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency22.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency23 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency23.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_transparency24 ) {
        ASSERT_TRUE( play("descriptor/transparency/transparency24.ncl") );
        ASSERT_TRUE( compareTo("black720x576") );
}

}
