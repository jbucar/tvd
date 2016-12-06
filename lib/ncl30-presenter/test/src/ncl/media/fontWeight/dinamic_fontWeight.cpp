#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_dinamic_fontWeight1 ) {
	ASSERT_TRUE( play("media/fontWeight/dinamic_fontWeight1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText_bold") );
}

TEST_F( Ginga, media_dinamic_dinamic_fontWeight2 ) {
	ASSERT_TRUE( play("media/fontWeight/dinamic_fontWeight2.ncl") );
	ASSERT_TRUE( compareTo("standardText_bold") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText") );
}

}
