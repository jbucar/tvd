#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_fontColor1 ) {
	ASSERT_TRUE( play("descriptor/fontColor/fontColor1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontColor2 ) {
	ASSERT_TRUE( play("descriptor/fontColor/fontColor2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontColor3 ) {
	ASSERT_TRUE( play("descriptor/fontColor/fontColor3.ncl") );
	ASSERT_TRUE( compareTo("redText") );
}

TEST_F( Ginga, descriptor_fontColor4 ) {
	ASSERT_TRUE( play("descriptor/fontColor/fontColor4.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontColor5 ) {
	ASSERT_TRUE( play("descriptor/fontColor/fontColor5.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontColor6 ) {
        ASSERT_TRUE( play("descriptor/fontColor/fontColor5.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

}
