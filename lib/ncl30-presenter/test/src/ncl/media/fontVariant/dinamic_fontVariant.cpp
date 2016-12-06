#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, DISABLED_media_dinamic_fontVariant1 ) {
	ASSERT_TRUE( play("media/fontVariant/dinamic_fontVariant1.ncl") );
	ASSERT_TRUE( compareTo("standardText_small_caps") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, DISABLED_media_dinamic_fontVariant2 ) {
	ASSERT_TRUE( play("media/fontVariant/dinamic_fontVariant2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText_small_caps") );
}

}
