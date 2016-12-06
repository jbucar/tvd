#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_dinamic_fontFamily1 ) {
	ASSERT_TRUE( play("media/fontFamily/dinamic_fontFamily1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardDejaVuSansTextSize10") );
}

TEST_F( Ginga, media_dinamic_fontFamily2 ) {
	ASSERT_TRUE( play("media/fontFamily/dinamic_fontFamily2.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansTextSize10") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText") );
}

}
