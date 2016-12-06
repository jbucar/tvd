#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_focusBorderTransparency1 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency2 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency3 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency4 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency5 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency6 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency6.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency7 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency7.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency8 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency8.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_top_corner") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency9 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency9.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent_and_width_border") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency10 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency10.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent_and_width_border") );
}

TEST_F( Ginga, descriptor_focusBorderTransparency11 ) {
	ASSERT_TRUE( play("descriptor/focusBorderTransparency/focusBorderTransparency11.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent_and_width_border") );
}


}
