#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_transparency1 ) {
	ASSERT_TRUE( play("media/transparency/transparency1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency2 ) {
	ASSERT_TRUE( play("media/transparency/transparency2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency3 ) {
	ASSERT_TRUE( play("media/transparency/transparency3.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
}

TEST_F( Ginga, media_transparency4 ) {
	ASSERT_TRUE( play("media/transparency/transparency4.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency5 ) {
	ASSERT_TRUE( play("media/transparency/transparency5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency6 ) {
	ASSERT_TRUE( play("media/transparency/transparency6.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
}

TEST_F( Ginga, media_transparency7 ) {
	ASSERT_TRUE( play("media/transparency/transparency7.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency8 ) {
	ASSERT_TRUE( play("media/transparency/transparency8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency9 ) {
	ASSERT_TRUE( play("media/transparency/transparency9.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency10 ) {
	ASSERT_TRUE( play("media/transparency/transparency10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency11 ) {
	ASSERT_TRUE( play("media/transparency/transparency11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_transparency12 ) {
	ASSERT_TRUE( play("media/transparency/transparency12.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency13 ) {
	ASSERT_TRUE( play("media/transparency/transparency13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency14 ) {
	ASSERT_TRUE( play("media/transparency/transparency14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_transparency15 ) {
	ASSERT_TRUE( play("media/transparency/transparency15.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_transparency16 ) {
	ASSERT_TRUE( play("media/transparency/transparency16.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

}
