#include "../../../ginga.h"

namespace ginga {

//test estaticos
TEST_F( Ginga, media_width1 ) {
	ASSERT_TRUE( play("media/width/width1.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width2 ) {
	ASSERT_TRUE( play("media/width/width2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_width3 ) {
	ASSERT_TRUE( play("media/width/width3.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width4 ) {
	ASSERT_TRUE( play("media/width/width4.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width5 ) {
	ASSERT_TRUE( play("media/width/width5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width6 ) {
	ASSERT_TRUE( play("media/width/width6.ncl") );
	ASSERT_TRUE( compareTo("blue270x576_black450x576") );
}

TEST_F( Ginga, media_width7 ) {
	ASSERT_TRUE( play("media/width/width7.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width8 ) {
	ASSERT_TRUE( play("media/width/width8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width9 ) {
	ASSERT_TRUE( play("media/width/width9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width10 ) {
	ASSERT_TRUE( play("media/width/width10.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_width11 ) {
	ASSERT_TRUE( play("media/width/width11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
TEST_F( Ginga, media_width12 ) {
	ASSERT_TRUE( play("media/width/width12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width13 ) {
	ASSERT_TRUE( play("media/width/width13.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width14 ) {
	ASSERT_TRUE( play("media/width/width14.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width15 ) {
	ASSERT_TRUE( play("media/width/width15.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}


TEST_F( Ginga, media_width16 ) {
	ASSERT_TRUE( play("media/width/width16.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width17 ) {
	ASSERT_TRUE( play("media/width/width17.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_width18 ) {
	ASSERT_TRUE( play("media/width/width18.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_width19 ) {
	ASSERT_TRUE( play("media/width/width19.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width20 ) {
	ASSERT_TRUE( play("media/width/width20.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width21 ) {
	ASSERT_TRUE( play("media/width/width21.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width22 ) {
	ASSERT_TRUE( play("media/width/width22.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width23 ) {
	ASSERT_TRUE( play("media/width/width23.ncl") );
	ASSERT_TRUE( compareTo("blue180x576_black540x576") );
}

TEST_F( Ginga, media_width24 ) {
	ASSERT_TRUE( play("media/width/width24.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, media_width25 ) {
	ASSERT_TRUE( play("media/width/width25.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

}
//test dinamicos

