#include "../../../ginga.h"

namespace ginga {

TEST_F( Ginga, connector_simpleAction01 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction01.ncl") );
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue360x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, DISABLED_connector_simpleAction02 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction02.ncl") );
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue360x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue360x288") );
}

TEST_F( Ginga, connector_simpleAction03 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction03.ncl") );
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue360x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_yellow360x576") );
}

TEST_F( Ginga, DISABLED_connector_simpleAction04 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction04.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, connector_simpleAction05 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction05.ncl") );
	ASSERT_TRUE( compareTo("black720x288_black360x288_blue360x288") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_noneWithBorder") );
}

TEST_F( Ginga, DISABLED_connector_simpleAction06 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction06.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

TEST_F( Ginga, connector_simpleAction07 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction07.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_yellow360x576") );
}

TEST_F( Ginga, connector_simpleAction08 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction08.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_yellow360x576") );
}

TEST_F( Ginga, connector_simpleAction09 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction09.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::red ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_yellow360x576") );
}

TEST_F( Ginga, DISABLED_connector_simpleAction10 ) {
	ASSERT_TRUE( play("connector/simpleAction/simpleAction10.ncl") );
	ASSERT_TRUE( compareTo("blue360x576_black360x576") );
}

}
