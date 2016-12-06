#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, descriptor_fontSize1 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize1.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_fontSize2 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_fontSize3 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize3.ncl") );
	ASSERT_TRUE( compareTo("standardText_size12") );
}

TEST_F( Ginga, descriptor_fontSize4 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize4.ncl") );
	ASSERT_TRUE( compareTo("standardText_size72") );
}

TEST_F( Ginga, descriptor_fontSize5 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_fontSize6 ) {
	ASSERT_TRUE( play("descriptor/fontSize/fontSize6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

}
