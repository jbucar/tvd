#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_fontFamily1 ) {
	ASSERT_TRUE( play("descriptor/fontFamily/fontFamily1.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansText") );
	ASSERT_FALSE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontFamily2 ) {
	ASSERT_TRUE( play("descriptor/fontFamily/fontFamily2.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontFamily3 ) {
	ASSERT_TRUE( play("descriptor/fontFamily/fontFamily3.ncl") );
	ASSERT_TRUE( compareTo("standardDejaVuSansText") );
}

TEST_F( Ginga, descriptor_fontFamily4 ) {
	ASSERT_TRUE( play("descriptor/fontFamily/fontFamily4.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontFamily5 ) {
        ASSERT_TRUE( play("descriptor/fontFamily/fontFamily5.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

}
