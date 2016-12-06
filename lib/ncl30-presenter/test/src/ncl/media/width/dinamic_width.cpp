#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_dinamic_width1 ) {
	ASSERT_TRUE( play("media/width/dinamic_width1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width2 ) {
	ASSERT_TRUE( play("media/width/dinamic_width2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );

}

TEST_F( Ginga, media_dinamic_width3 ) {
	ASSERT_TRUE( play("media/width/dinamic_width3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width4 ) {
	ASSERT_TRUE( play("media/width/dinamic_width4.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width5 ) {
	ASSERT_TRUE( play("media/width/dinamic_width5.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width6 ) {
	ASSERT_TRUE( play("media/width/dinamic_width6.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width7 ) {
	ASSERT_TRUE( play("media/width/dinamic_width7.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width8 ) {
	ASSERT_TRUE( play("media/width/dinamic_width8.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width9 ) {
	ASSERT_TRUE( play("media/width/dinamic_width9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width10 ) {
	ASSERT_TRUE( play("media/width/dinamic_width10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width11 ) {
	ASSERT_TRUE( play("media/width/dinamic_width11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width12 ) {
	ASSERT_TRUE( play("media/width/dinamic_width12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width13 ) {
	ASSERT_TRUE( play("media/width/dinamic_width13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width14 ) {
	ASSERT_TRUE( play("media/width/dinamic_width14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width15 ) {
	ASSERT_TRUE( play("media/width/dinamic_width15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width16 ) {
	ASSERT_TRUE( play("media/width/dinamic_width16.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width17 ) {
	ASSERT_TRUE( play("media/width/dinamic_width17.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width18 ) {
	ASSERT_TRUE( play("media/width/dinamic_width18.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width19 ) {
	ASSERT_TRUE( play("media/width/dinamic_width19.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_width20 ) {
	ASSERT_TRUE( play("media/width/dinamic_width20.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_dinamic_width21 ) {
	ASSERT_TRUE( play("media/width/dinamic_width21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width22 ) {
	ASSERT_TRUE( play("media/width/dinamic_width22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width23 ) {
	ASSERT_TRUE( play("media/width/dinamic_width23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width24 ) {
	ASSERT_TRUE( play("media/width/dinamic_width24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_width25 ) {
	ASSERT_TRUE( play("media/width/dinamic_width25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}
}
