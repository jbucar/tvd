#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, descriptor_bounds1 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds1.ncl") );
	ASSERT_TRUE( compareTo("black216x576_blue360x576_black144x576") );
}

TEST_F( Ginga, descriptor_bounds2 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds2.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x288_black720x144") );
}

TEST_F( Ginga, descriptor_bounds3 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds3.ncl") );
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue288x216") );
}

TEST_F( Ginga, descriptor_bounds4 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}

TEST_F( Ginga, descriptor_bounds5 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds5.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds6 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds6.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds7 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds8 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds8.ncl") );
	ASSERT_TRUE( compareTo("black270x576_blue450x576") );
}

TEST_F( Ginga, descriptor_bounds9 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds9.ncl") );
	ASSERT_TRUE( compareTo("black720x216_blue720x360") );
}

TEST_F( Ginga, descriptor_bounds10 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds10.ncl") );
	ASSERT_TRUE( compareTo("blue270x216_left_corner") );
}

TEST_F( Ginga, descriptor_bounds11 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds11.ncl") );
	ASSERT_TRUE( compareTo("blue270x216_left_corner") );
}


TEST_F( Ginga, descriptor_bounds12 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds12.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds13 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds13.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x288_black720x144") );
}

TEST_F( Ginga, descriptor_bounds14 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds14.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds15 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds15.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds16 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds16.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds17 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds17.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds18 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds18.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}


TEST_F( Ginga, descriptor_bounds19 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds19.ncl") );
	ASSERT_TRUE( compareTo("black720x144_blue360x288_black720x144") );
}

TEST_F( Ginga, descriptor_bounds20 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds20.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_corner") );
}

TEST_F( Ginga, descriptor_bounds21 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds21.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_top_corner") );
}

TEST_F( Ginga, descriptor_bounds22 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds22.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_top_corner") );
}

TEST_F( Ginga, descriptor_bounds23 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds23.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_top_corner") );
}

TEST_F( Ginga, descriptor_bounds24 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds24.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_left_top_corner") );
}

TEST_F( Ginga, descriptor_bounds25 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds25.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds26 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds26.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds27 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds27.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds28 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds28.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds29 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds29.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds30 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds30.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds31 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds31.ncl") );
	ASSERT_TRUE( compareTo("blue180x144_right_corner") );
}

TEST_F( Ginga, descriptor_bounds32 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds32.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds33 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds33.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, descriptor_bounds34 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds34.ncl") );
	ASSERT_TRUE( compareTo("black180x576_black180x288_blue180x288_black360x576") );
}

TEST_F( Ginga, descriptor_bounds35 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds35.ncl") );
	ASSERT_TRUE( compareTo("black360x288_black360x144_blue360x144_black720x288") );
}

TEST_F( Ginga, descriptor_bounds36 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds36.ncl") );
	ASSERT_TRUE( compareTo("black180x576_black180x288_blue180x288_black360x576") );
}

TEST_F( Ginga, descriptor_bounds37 ) {
	ASSERT_TRUE( play("descriptor/bounds/bounds37.ncl") );
	ASSERT_TRUE( compareTo("black360x288_black360x144_blue360x144_black720x288") );
}

TEST_F( Ginga, descriptor_bounds39 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds39.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds40 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds40.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds41 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds41.ncl") );
        ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, descriptor_bounds42 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds42.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_left_top_corner") );
}

TEST_F( Ginga, descriptor_bounds43 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds43.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds44 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds44.ncl") );
        ASSERT_TRUE( compareTo("black360x576_blue360x576") );
}

TEST_F( Ginga, descriptor_bounds45 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds45.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds46 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds46.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds47 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds47.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds48 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds48.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds49 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds49.ncl") );
        ASSERT_TRUE( compareTo("blue360x288_right_corner") );
}

TEST_F( Ginga, descriptor_bounds50 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds50.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds51 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds51.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds52 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds52.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds53 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds53.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds54 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds54.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds55 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds55.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds56 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds56.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds57 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds57.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds58 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds58.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds59 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds59.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds60 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds60.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds61 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds61.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds62 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds62.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, descriptor_bounds63 ) {
        ASSERT_TRUE( play("descriptor/bounds/bounds63.ncl") );
        ASSERT_TRUE( compareTo("blue720x576") );
}

}
