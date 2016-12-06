#include "../../../ginga.h"

namespace ginga {
TEST_F( Ginga, media_event_stop ) {
	ASSERT_TRUE( play("media/event/event1.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("../transparent") );
}

TEST_F( Ginga, media_event_stop_and_start ) {
	ASSERT_TRUE( play("media/event/event2.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_event_start ) {
	ASSERT_TRUE( play("media/event/event3.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, media_event_start_and_stop ) {
	ASSERT_TRUE( play("media/event/event4.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("../transparent") );
}


TEST_F( Ginga, DISABLED_media_event_start_and_stop_return ) {
	ASSERT_TRUE( play("media/event/event5.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue720x576") );
}

}

