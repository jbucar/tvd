#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontVariant1 ) {
	ASSERT_TRUE( play("media/fontVariant/fontVariant1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontVariant2 ) {
	ASSERT_TRUE( play("media/fontVariant/fontVariant2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, DISABLED_media_fontVariant3 ) {
	ASSERT_TRUE( play("media/fontVariant/fontVariant3.ncl") );
	ASSERT_TRUE( compareTo("standardText_small_caps") );
}

TEST_F( Ginga, media_fontVariant4 ) {
	ASSERT_TRUE( play("media/fontVariant/fontVariant4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, DISABLED_media_fontVariant5 ) {
	ASSERT_TRUE( play("media/fontVariant/fontVariant5.ncl") );
	ASSERT_TRUE( compareTo("standardText_small_caps") );
}

}
