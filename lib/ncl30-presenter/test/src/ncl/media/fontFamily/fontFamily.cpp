#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontFamily1 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontFamily2 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontFamily3 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily3.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontFamily3a ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily3a.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansText") );
}

TEST_F( Ginga, media_fontFamily4 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily4.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansText") );
}

TEST_F( Ginga, media_fontFamily5 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily5.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, media_fontFamily6 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily6.ncl") );
	ASSERT_TRUE( compareTo("standardText2") );
}

TEST_F( Ginga, media_fontFamily7 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily7.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansText") );
}

TEST_F( Ginga, media_fontFamily8 ) {
	ASSERT_TRUE( play("media/fontFamily/fontFamily8.ncl") );
	ASSERT_TRUE( compareTo("customFontText") );
}

}
