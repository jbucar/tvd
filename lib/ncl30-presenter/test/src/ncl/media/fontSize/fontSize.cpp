#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_fontSize1 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize1.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontSize2 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontSize3 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize3.ncl") );
	ASSERT_TRUE( compareTo("standardText_size12") );
}

TEST_F( Ginga, media_fontSize4 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize4.ncl") );
	ASSERT_TRUE( compareTo("standardText_size72") );
}
TEST_F( Ginga, media_fontSize5 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontSize6 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_fontSize7 ) {
	ASSERT_TRUE( play("media/fontSize/fontSize7.ncl") );
	ASSERT_TRUE( compareTo("standardText_size12") );
}

}
