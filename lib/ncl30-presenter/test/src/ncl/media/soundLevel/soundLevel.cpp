#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, media_soundLevel1 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel1.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel2 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel2.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel3 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel3.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel4 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel4.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel5 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel5.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel6 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel6.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel7 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel7.ncl") );
	ASSERT_TRUE( compareTo("video") );
}

TEST_F( Ginga, media_soundLevel8 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_soundLevel9 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_soundLevel10 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, media_soundLeve11 ) {
	ASSERT_TRUE( play("media/soundLevel/soundLevel11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}
}
