#include "../../../ginga.h"

namespace ginga {
  
TEST_F( Ginga, descriptor_moveLeft_moveRight_moveUp_moveDown1 ) {
	ASSERT_TRUE( play("descriptor/moveLeft_moveRight_moveUp_moveDown/moveLeft_moveRight_moveUp_moveDown.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_blueSelected") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_down ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_redSelected") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_right ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_yellowSelected") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_up ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_greenSelected") );
	ASSERT_TRUE( pressKey( ::util::key::cursor_left ));
	ASSERT_TRUE( compareTo("blue360x288_red360x288_green360x288_yellow360x288_blueSelected") );
}

}
