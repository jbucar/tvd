#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_dinamic_fontColor1 ) {
	ASSERT_TRUE( play("media/fontColor/dinamic_fontColor1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("redText") );
}

TEST_F( Ginga, media_dinamic_fontColor2 ) {
	ASSERT_TRUE( play("media/fontColor/dinamic_fontColor2.ncl") );
	ASSERT_TRUE( compareTo("redText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("redText") );
}

TEST_F( Ginga, media_dinamic_fontColor3 ) {
	ASSERT_TRUE( play("media/fontColor/dinamic_fontColor3.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("redTextIn360x288") );
}

}
