#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_fontStyle1 ) {
	ASSERT_TRUE( play("media/fontStyle/dinamic_fontStyle1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText_italic") );
}

TEST_F( Ginga, media_dinamic_fontStyle2 ) {
	ASSERT_TRUE( play("media/fontStyle/dinamic_fontStyle2.ncl") );
	ASSERT_TRUE( compareTo("standardText_italic") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText") );
}

}
