#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_dinamic_bounds1 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds1.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x144_black360x216_blue360x216_black720x156") );
}

TEST_F( Ginga, media_dinamic_bounds2 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds2.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x216_black270x360_blue225x180") );
}

TEST_F( Ginga, media_dinamic_bounds3 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds3.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue288x216") );
}

TEST_F( Ginga, media_dinamic_bounds4 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds4.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x144_black360x216_blue360x216_black720x156") );
}

TEST_F( Ginga, media_dinamic_bounds5 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds5.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black360x288_blue360x288_black360x576") );
}

TEST_F( Ginga, media_dinamic_bounds6 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds6.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue288x360_black288x720") );
}

TEST_F( Ginga, media_dinamic_bounds7 ) {
	ASSERT_TRUE( play("media/bounds/dinamic_bounds7.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x216_black360x216") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}
}
