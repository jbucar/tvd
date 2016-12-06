#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_focusSrc1 ) {
	ASSERT_TRUE( play("media/focusSrc/focusSrc1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, media_focusSrc2 ) {
	ASSERT_TRUE( play("media/focusSrc/focusSrc2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_focusSrc3 ) {
	ASSERT_TRUE( play("media/focusSrc/focusSrc3.ncl") );
	ASSERT_TRUE( compareTo("red360x288_centered_borderWhite_2px") );
}

}
