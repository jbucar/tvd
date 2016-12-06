#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_fontVariant1 ) {
	ASSERT_TRUE( play("descriptor/fontVariant/fontVariant1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga , DISABLED_descriptor_fontVariant2 ) {
	ASSERT_TRUE( play("descriptor/fontVariant/fontVariant2.ncl") );
	ASSERT_TRUE( compareTo("standardText_small_caps") );
}

TEST_F( Ginga, descriptor_fontVariant3 ) {
        ASSERT_TRUE( play("descriptor/fontVariant/fontVariant3.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontVariant4 ) {
        ASSERT_TRUE( play("descriptor/fontVariant/fontVariant4.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontVariant5 ) {
        ASSERT_TRUE( play("descriptor/fontVariant/fontVariant5.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontVariant6 ) {
        ASSERT_TRUE( play("descriptor/fontVariant/fontVariant6.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

}
