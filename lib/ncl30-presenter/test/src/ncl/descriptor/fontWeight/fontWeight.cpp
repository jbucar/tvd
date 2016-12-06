#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, descriptor_fontWeight1 ) {
	ASSERT_TRUE( play("descriptor/fontWeight/fontWeight1.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight2 ) {
	ASSERT_TRUE( play("descriptor/fontWeight/fontWeight2.ncl") );
	ASSERT_TRUE( compareTo("standardText_bold") );
	ASSERT_FALSE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight3 ) {
	ASSERT_TRUE( play("descriptor/fontWeight/fontWeight3.ncl") );
	ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight4 ) {
        ASSERT_TRUE( play("descriptor/fontWeight/fontWeight4.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight5 ) {
        ASSERT_TRUE( play("descriptor/fontWeight/fontWeight5.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight6 ) {
        ASSERT_TRUE( play("descriptor/fontWeight/fontWeight6.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}

TEST_F( Ginga, descriptor_fontWeight7 ) {
        ASSERT_TRUE( play("descriptor/fontWeight/fontWeight7.ncl") );
        ASSERT_TRUE( compareTo("standardText") );
}


}
