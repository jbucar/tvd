#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_focusBorderTransparency1 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, media_focusBorderTransparency2 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}

TEST_F( Ginga, media_focusBorderTransparency3 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus") );
}

TEST_F( Ginga, media_focusBorderTransparency4 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, media_focusBorderTransparency5 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}

TEST_F( Ginga, media_focusBorderTransparency6 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency6.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus") );
}

TEST_F( Ginga, media_focusBorderTransparency7 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, media_focusBorderTransparency8 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency8.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, media_focusBorderTransparency9 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency9.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}

TEST_F( Ginga, media_focusBorderTransparency10 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency10.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, media_focusBorderTransparency11 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency11.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}
TEST_F( Ginga, media_focusBorderTransparency12 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency12.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}
TEST_F( Ginga, media_focusBorderTransparency13 ) {
	ASSERT_TRUE( play("media/focusBorderTransparency/focusBorderTransparency13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

}
