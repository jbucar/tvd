#include "../../../ginga.h"

namespace ginga {
//test dinamicos

TEST_F( Ginga, media_dinamic_left1 ) {
	ASSERT_TRUE( play("media/left/dinamic_left1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left2 ) {
	ASSERT_TRUE( play("media/left/dinamic_left2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left3 ) {
	ASSERT_TRUE( play("media/left/dinamic_left3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left4 ) {
	ASSERT_TRUE( play("media/left/dinamic_left4.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left5 ) {
	ASSERT_TRUE( play("media/left/dinamic_left5.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left6 ) {
	ASSERT_TRUE( play("media/left/dinamic_left6.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left7 ) {
	ASSERT_TRUE( play("media/left/dinamic_left7.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left8 ) {
	ASSERT_TRUE( play("media/left/dinamic_left8.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left9 ) {
	ASSERT_TRUE( play("media/left/dinamic_left9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left10 ) {
	ASSERT_TRUE( play("media/left/dinamic_left10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left11 ) {
	ASSERT_TRUE( play("media/left/dinamic_left11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left12 ) {
	ASSERT_TRUE( play("media/left/dinamic_left12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left13 ) {
	ASSERT_TRUE( play("media/left/dinamic_left13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left14 ) {
	ASSERT_TRUE( play("media/left/dinamic_left14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left15 ) {
	ASSERT_TRUE( play("media/left/dinamic_left15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left16 ) {
	ASSERT_TRUE( play("media/left/dinamic_left16.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left17 ) {
	ASSERT_TRUE( play("media/left/dinamic_left17.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left18 ) {
	ASSERT_TRUE( play("media/left/dinamic_left18.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left19 ) {
	ASSERT_TRUE( play("media/left/dinamic_left19.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_left20 ) {
	ASSERT_TRUE( play("media/left/dinamic_left20.ncl") );
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, media_dinamic_left21 ) {
	ASSERT_TRUE( play("media/left/dinamic_left21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left22 ) {
	ASSERT_TRUE( play("media/left/dinamic_left22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left23 ) {
	ASSERT_TRUE( play("media/left/dinamic_left23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left24 ) {
	ASSERT_TRUE( play("media/left/dinamic_left24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_left25 ) {
	ASSERT_TRUE( play("media/left/dinamic_left25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

}
