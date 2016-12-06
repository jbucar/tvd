#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_fontSize1 ) {
	ASSERT_TRUE( play("media/fontSize/dinamic_fontSize1.ncl") );
	ASSERT_TRUE( compareTo("standardText_size12") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText_size12") );
}

TEST_F( Ginga, media_dinamic_fontSize2 ) {
	ASSERT_TRUE( play("media/fontSize/dinamic_fontSize2.ncl") );
	ASSERT_TRUE( compareTo("standardText_size12") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("standardText_size72") );
}

}
