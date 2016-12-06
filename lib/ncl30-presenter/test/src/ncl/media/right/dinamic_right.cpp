#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_right1 ) {
	ASSERT_TRUE( play("media/right/dinamic_right1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right2 ) {
	ASSERT_TRUE( play("media/right/dinamic_right2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );

}

TEST_F( Ginga, media_dinamic_right3 ) {
	ASSERT_TRUE( play("media/right/dinamic_right3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right4 ) {
	ASSERT_TRUE( play("media/right/dinamic_right4.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right5 ) {
	ASSERT_TRUE( play("media/right/dinamic_right5.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right6 ) {
	ASSERT_TRUE( play("media/right/dinamic_right6.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right7 ) {
	ASSERT_TRUE( play("media/right/dinamic_right7.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right8 ) {
	ASSERT_TRUE( play("media/right/dinamic_right8.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right9 ) {
	ASSERT_TRUE( play("media/right/dinamic_right9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right10 ) {
	ASSERT_TRUE( play("media/right/dinamic_right10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right11 ) {
	ASSERT_TRUE( play("media/right/dinamic_right11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right12 ) {
	ASSERT_TRUE( play("media/right/dinamic_right12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right13 ) {
	ASSERT_TRUE( play("media/right/dinamic_right13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right14 ) {
	ASSERT_TRUE( play("media/right/dinamic_right14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right15 ) {
	ASSERT_TRUE( play("media/right/dinamic_right15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right16 ) {
	ASSERT_TRUE( play("media/right/dinamic_right16.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right17 ) {
	ASSERT_TRUE( play("media/right/dinamic_right17.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right18 ) {
	ASSERT_TRUE( play("media/right/dinamic_right18.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right19 ) {
	ASSERT_TRUE( play("media/right/dinamic_right19.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_right20 ) {
	ASSERT_TRUE( play("media/right/dinamic_right20.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_right21 ) {
	ASSERT_TRUE( play("media/right/dinamic_right21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right22 ) {
	ASSERT_TRUE( play("media/right/dinamic_right22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right23 ) {
	ASSERT_TRUE( play("media/right/dinamic_right23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right24 ) {
	ASSERT_TRUE( play("media/right/dinamic_right24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_right25 ) {
	ASSERT_TRUE( play("media/right/dinamic_right25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

}
