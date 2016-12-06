#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontColor1 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontColor2 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, fontColor3 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontColor4 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor4.ncl") );
	ASSERT_TRUE( compareTo("redText") );
}

TEST_F( Ginga, media_fontColor5 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_fontColor6 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_fontColor7 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor7.ncl") );
	ASSERT_TRUE( compareTo("blackText") );
}

TEST_F( Ginga, media_fontColor8 ) {
	ASSERT_TRUE( play("media/fontColor/fontColor8.ncl") );
	ASSERT_TRUE( compareTo("redText") );
}

}
