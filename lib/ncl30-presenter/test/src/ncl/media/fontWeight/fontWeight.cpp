#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontWeight1 ) {
	ASSERT_TRUE( play("media/fontWeight/fontWeight1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontWeight2 ) {
	ASSERT_TRUE( play("media/fontWeight/fontWeight2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontWeight3 ) {
	ASSERT_TRUE( play("media/fontWeight/fontWeight3.ncl") );
	ASSERT_TRUE( compareTo("standardText_bold") );
}

TEST_F( Ginga, media_fontWeight4 ) {
	ASSERT_TRUE( play("media/fontWeight/fontWeight4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontWeight5 ) {
	ASSERT_TRUE( play("media/fontWeight/fontWeight5.ncl") );
	ASSERT_TRUE( compareTo("standardText_bold") );
}

}
