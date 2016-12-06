#include "../../../ginga.h"

namespace ginga {

//test de propiedades dinamicas

TEST_F( Ginga, media_dinamic_bottom1 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom2 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom3 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom3.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") ); 
}

TEST_F( Ginga, media_dinamic_bottom4 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom4.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom5 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom5.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom6 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom6.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom7 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom7.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom8 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom8.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom9 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom10 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom11 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom12 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom13 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom13.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom14 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom14.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom15 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom15.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom16 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom16.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom17 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom17.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom18 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom18.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom19 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom19.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_dinamic_bottom20 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom20.ncl") );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, media_dinamic_bottom21 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom21.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom22 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom22.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom23 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom23.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom24 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom24.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_dinamic_bottom25 ) {
	ASSERT_TRUE( play("media/bottom/dinamic_bottom25.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}


}
