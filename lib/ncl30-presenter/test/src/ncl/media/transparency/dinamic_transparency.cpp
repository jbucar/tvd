#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_transparency1 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("transparency50%") );
}
TEST_F( Ginga, media_dinamic_transparency2 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_dinamic_transparency3 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency3.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}
TEST_F( Ginga, media_dinamic_transparency4 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency4.ncl") );
	ASSERT_TRUE( compareTo("transparency50%") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_dinamic_transparency5 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("transparency50%") );
}
TEST_F( Ginga, media_dinamic_transparency6 ) {
	ASSERT_TRUE( play("media/transparency/dinamic_transparency6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

}

