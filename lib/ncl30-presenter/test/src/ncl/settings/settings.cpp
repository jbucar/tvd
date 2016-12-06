#include "../../ginga.h"

namespace ginga {

TEST_F( Ginga, settings_read_only1 ) {
	ASSERT_TRUE( play("settings/read_only/si_channelNumber.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_read_only2 ) {
	ASSERT_TRUE( play("settings/read_only/system_language.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_read_only3 ) {
	ASSERT_TRUE( play("settings/read_only/user_age.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_shared1 ) {
	ASSERT_TRUE( play("settings/shared/settings1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, settings_shared2 ) {
	ASSERT_TRUE( play("settings/shared/settings2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, settings_global1 ) {
	ASSERT_TRUE( play("settings/global/settings1.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, settings_global2 ) {
	ASSERT_TRUE( play("settings/global/settings2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue720x288_black720x288") );
}

TEST_F( Ginga, settings_system1 ) {
	ASSERT_TRUE( play("settings/system/system_screenGraphicSize.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system2 ) {
	ASSERT_TRUE( play("settings/system/system_screenGraphicSize2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system3 ) {
	ASSERT_TRUE( play("settings/system/system_screenSize.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system4 ) {
	ASSERT_TRUE( play("settings/system/system_screenSize2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system5 ) {
	ASSERT_TRUE( play("settings/system/system_audioType.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system6) {
	ASSERT_TRUE( play("settings/system/system_audioType2.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system7) {
	ASSERT_TRUE( play("settings/system/system_language.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system8) {
	ASSERT_TRUE( play("settings/system/system_language2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system9) {
	ASSERT_TRUE( play("settings/system/system_caption.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system10) {
	ASSERT_TRUE( play("settings/system/system_caption2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system11) {
	ASSERT_TRUE( play("settings/system/system_subtitle.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system12) {
	ASSERT_TRUE( play("settings/system/system_subtitle2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system13) {
	ASSERT_TRUE( play("settings/system/system_classNumber.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system14) {
	ASSERT_TRUE( play("settings/system/system_classNumber2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system15) {
	ASSERT_TRUE( play("settings/system/system_cpu.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system16) {
	ASSERT_TRUE( play("settings/system/system_cpu2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system17) {
	ASSERT_TRUE( play("settings/system/system_memory.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system18) {
	ASSERT_TRUE( play("settings/system/system_memory2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system19) {
	ASSERT_TRUE( play("settings/system/system_luaversion.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system20) {
	ASSERT_TRUE( play("settings/system/system_luaversion2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system21) {
	ASSERT_TRUE( play("settings/system/system_operatingsystem.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_system22) {
	ASSERT_TRUE( play("settings/system/system_operatingsystem2.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_system23) {
	ASSERT_TRUE( play("settings/system/system_caption_desc_switch.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_default_focusBorderColor1) {
	ASSERT_TRUE( play("settings/default/focusBorderColor1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, settings_default_focusBorderColor_invalid) {
	ASSERT_TRUE( play("settings/default/focusBorderColor1_invalid.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderColor2) {
	ASSERT_TRUE( play("settings/default/focusBorderColor2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
}

TEST_F( Ginga, settings_default_focusBorderColor3) {
	ASSERT_TRUE( play("settings/default/focusBorderColor3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderRed_2px") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency1) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency2) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency3) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency4) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency4.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_37.5%_transparent") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency5) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency5.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_with_focus_transparent_37%_border2px") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency6) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency6.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency7) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency7.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency8) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency8.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency9) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency9.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency10) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency10.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency11) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency11.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderTransparency12) {
	ASSERT_TRUE( play("settings/default/focusBorderTransparency12.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_focusBorderWidth) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth.ncl") );
	ASSERT_TRUE( compareTo("blue360x288") );
}

TEST_F( Ginga, settings_default_focusBorderColor_lua) {
	ASSERT_TRUE( play("settings/update/focusBorderColor.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
}

TEST_F( Ginga, settings_update_default_focusBorderColor) {
	ASSERT_TRUE( play("settings/update/updateFocusBorderColor.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("red720x576") );
}

TEST_F( Ginga, settings_update_shared) {
	ASSERT_TRUE( play("settings/update/updateMyVar.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("red720x576") );
}

TEST_F( Ginga, DISABLED_settings_update_service) {
	ASSERT_TRUE( play("settings/update/updateCurrentFocus.ncl") );
	ASSERT_TRUE( compareTo("blue720x576") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("red720x576") );
}

TEST_F( Ginga, settings_default_focusBorderWidth1_2px ) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth1.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, settings_default_focusBorderWidth1_5px ) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth1_5px.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_5px") );
}

TEST_F( Ginga, settings_default_focusBorderWidth1_minus2px ) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth1_minus2px.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_-2px") );
}

TEST_F( Ginga, settings_default_focusBorderWidth2 ) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth2.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, settings_default_focusBorderWidth3 ) {
	ASSERT_TRUE( play("settings/default/focusBorderWidth3.ncl") );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_-3px") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_5px") );
}

TEST_F( Ginga, settings_default_selBorderColor1) {
	ASSERT_TRUE( play("settings/default/selBorderColor1.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::enter ) );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderWhite_2px") );
}

TEST_F( Ginga, settings_default_selBorderColor_invalid) {
	ASSERT_TRUE( play("settings/default/selBorderColor1_invalid.ncl") );
	ASSERT_TRUE( compareTo("black720x576") );
}

TEST_F( Ginga, settings_default_selBorderColor2) {
	ASSERT_TRUE( play("settings/default/selBorderColor2.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::enter ) );
	ASSERT_TRUE( compareTo("blue360x288_centered_borderRed_2px") );
}

TEST_F( Ginga, settings_default_selBorderColor3) {
	ASSERT_TRUE( play("settings/default/selBorderColor3.ncl") );
	ASSERT_TRUE( pressKey( ::util::key::enter ) );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderWhite_2px") );
	ASSERT_TRUE( pressKey( ::util::key::red ) );
	ASSERT_TRUE( pressKey( ::util::key::enter ) );
	ASSERT_TRUE( compareTo("blue360x288_red360x288_blueHasBorderRed_2px") );
}
}
