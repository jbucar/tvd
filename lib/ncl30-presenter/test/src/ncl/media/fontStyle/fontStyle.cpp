#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontStyle1 ) {
	ASSERT_TRUE( play("media/fontStyle/fontStyle1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontStyle2 ) {
	ASSERT_TRUE( play("media/fontStyle/fontStyle2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontStyle3 ) {
	ASSERT_TRUE( play("media/fontStyle/fontStyle3.ncl") );
	ASSERT_TRUE( compareTo("standardText_italic") );
}

TEST_F( Ginga, media_fontStyle4 ) {
	ASSERT_TRUE( play("media/fontStyle/fontStyle4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontStyle5 ) {
	ASSERT_TRUE( play("media/fontStyle/fontStyle5.ncl") );
	ASSERT_TRUE( compareTo("standardText_italic") );
}

}
