#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_fontStyle1 ) {
	ASSERT_TRUE( play("descriptor/fontStyle/fontStyle1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontStyle2 ) {
	ASSERT_TRUE( play("descriptor/fontStyle/fontStyle2.ncl") );
	ASSERT_TRUE( compareTo("standardText_italic") );
}

TEST_F( Ginga, descriptor_fontStyle3 ) {
	ASSERT_TRUE( play("descriptor/fontStyle/fontStyle3.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontStyle4 ) {
        ASSERT_TRUE( play("descriptor/fontStyle/fontStyle4.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontStyle5 ) {
        ASSERT_TRUE( play("descriptor/fontStyle/fontStyle5.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontStyle6 ) {
        ASSERT_TRUE( play("descriptor/fontStyle/fontStyle6.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontStyle7 ) {
        ASSERT_TRUE( play("descriptor/fontStyle/fontStyle7.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

}
