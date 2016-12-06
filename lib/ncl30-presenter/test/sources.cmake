ADD_SOURCES(
	test/src/main.cpp
	test/src/ginga.cpp
	test/src/util.cpp
# 	REGION TESTS
	test/src/ncl/region/top/top.cpp
	test/src/ncl/region/bottom/bottom.cpp
	test/src/ncl/region/height/height.cpp
	test/src/ncl/region/width/width.cpp
	test/src/ncl/region/left/left.cpp
	test/src/ncl/region/right/right.cpp
	test/src/ncl/region/zIndex/zIndex.cpp
	test/src/ncl/region/precedence/precedence.cpp
	test/src/ncl/region/nested/nested.cpp
	test/src/ncl/region/device/device.cpp
	test/src/ncl/region/import/import.cpp
# 	DESCRIPTOR TESTS
	test/src/ncl/descriptor/explicitDur/explicitDur.cpp
	test/src/ncl/descriptor/top/top.cpp
	test/src/ncl/descriptor/bottom/bottom.cpp
	test/src/ncl/descriptor/height/height.cpp
	test/src/ncl/descriptor/width/width.cpp
	test/src/ncl/descriptor/left/left.cpp
	test/src/ncl/descriptor/right/right.cpp
	test/src/ncl/descriptor/precedence/precedence.cpp
	test/src/ncl/descriptor/location/location.cpp
	test/src/ncl/descriptor/transparency/transparency.cpp
	test/src/ncl/descriptor/bounds/bounds.cpp
	test/src/ncl/descriptor/visible/visible.cpp
	test/src/ncl/descriptor/size/size.cpp
	test/src/ncl/descriptor/fit/fit.cpp
	test/src/ncl/descriptor/fontColor/fontColor.cpp
	test/src/ncl/descriptor/fontFamily/fontFamily.cpp
	test/src/ncl/descriptor/fontSize/fontSize.cpp
	test/src/ncl/descriptor/fontVariant/fontVariant.cpp
	test/src/ncl/descriptor/fontWeight/fontWeight.cpp
	test/src/ncl/descriptor/fontStyle/fontStyle.cpp
	test/src/ncl/descriptor/focusSrc/focusSrc.cpp
	test/src/ncl/descriptor/focusBorderTransparency/focusBorderTransparency.cpp
	test/src/ncl/descriptor/focusBorderColor/focusBorderColor.cpp
	test/src/ncl/descriptor/focusBorderWidth/focusBorderWidth.cpp
	test/src/ncl/descriptor/selBorderColor/selBorderColor.cpp
	test/src/ncl/descriptor/focusSelSrc/focusSelSrc.cpp
	test/src/ncl/descriptor/zIndex/zIndex.cpp
	test/src/ncl/descriptor/moveLeft_moveRight_moveUp_moveDown/moveLeft_moveRight_moveUp_moveDown.cpp
# 	MEDIA TESTS
	test/src/ncl/media/area/area.cpp
	test/src/ncl/media/bottom/bottom.cpp
	test/src/ncl/media/event/event.cpp
	test/src/ncl/media/height/height.cpp
	test/src/ncl/media/width/width.cpp
	test/src/ncl/media/location/location.cpp
	test/src/ncl/media/left/left.cpp
	test/src/ncl/media/top/top.cpp
	test/src/ncl/media/right/right.cpp
	test/src/ncl/media/size/size.cpp
	test/src/ncl/media/precedence/precedence.cpp
	test/src/ncl/media/fit/fit.cpp
	test/src/ncl/media/fontColor/fontColor.cpp
	test/src/ncl/media/fontFamily/fontFamily.cpp
	test/src/ncl/media/fontSize/fontSize.cpp
	test/src/ncl/media/fontStyle/fontStyle.cpp
	test/src/ncl/media/fontVariant/fontVariant.cpp
	test/src/ncl/media/fontWeight/fontWeight.cpp
	test/src/ncl/media/zIndex/zIndex.cpp
	test/src/ncl/media/bounds/bounds.cpp	
	test/src/ncl/media/transparency/transparency.cpp
	test/src/ncl/media/visible/visible.cpp	
	test/src/ncl/media/focusIndex/focusIndex.cpp
	test/src/ncl/media/focusBorderColor/focusBorderColor.cpp
	test/src/ncl/media/focusBorderWidth/focusBorderWidth.cpp
	test/src/ncl/media/focusBorderTransparency/focusBorderTransparency.cpp
	test/src/ncl/media/focusSrc/focusSrc.cpp
	test/src/ncl/media/focusSelSrc/focusSelSrc.cpp
	test/src/ncl/media/selBorderColor/selBorderColor.cpp
	test/src/ncl/media/withOutRegion/withOutRegion.cpp
	test/src/ncl/media/soundLevel/soundLevel.cpp
#	MEDIA TEST > DINAMICOS
	test/src/ncl/media/descriptor/dinamic_descriptor.cpp
	test/src/ncl/media/explicitDur/dinamic_explicitDur.cpp
	test/src/ncl/media/bottom/dinamic_bottom.cpp
	test/src/ncl/media/height/dinamic_height.cpp
	test/src/ncl/media/left/dinamic_left.cpp
	test/src/ncl/media/precedence/dinamic_precedence.cpp
	test/src/ncl/media/right/dinamic_right.cpp
	test/src/ncl/media/width/dinamic_width.cpp
	test/src/ncl/media/zIndex/dinamic_zIndex.cpp
	test/src/ncl/media/top/dinamic_top.cpp
	test/src/ncl/media/bounds/dinamic_bounds.cpp
	test/src/ncl/media/fit/dinamic_fit.cpp
	test/src/ncl/media/visible/dinamic_visible.cpp
	test/src/ncl/media/location/dinamic_location.cpp
	test/src/ncl/media/size/dinamic_size.cpp
	test/src/ncl/media/transparency/dinamic_transparency.cpp
	test/src/ncl/media/fontColor/dinamic_fontColor.cpp
	test/src/ncl/media/fontFamily/dinamic_fontFamily.cpp
	test/src/ncl/media/fontSize/dinamic_fontSize.cpp
	test/src/ncl/media/fontStyle/dinamic_fontStyle.cpp
	test/src/ncl/media/fontVariant/dinamic_fontVariant.cpp
	test/src/ncl/media/fontWeight/dinamic_fontWeight.cpp
#	CONTEXT TEST
	test/src/ncl/context/context.cpp
#	SETTINGS TEST
	test/src/ncl/settings/settings.cpp
#	CONNECTOR TEST
	test/src/ncl/connector/simpleAction/simpleAction.cpp
#	INVAP TEST
	test/src/ncl/invap/region/region.cpp
	test/src/ncl/invap/descriptor/descriptor.cpp
	test/src/ncl/invap/text/text.cpp
	test/src/ncl/invap/lua/lua.cpp
	test/src/ncl/invap/dynamic/dynamic.cpp
	test/src/ncl/invap/buttons/buttons.cpp	
#	Lua Tests
	test/src/ncl/lua/events/events.cpp
#	Edittng coammands Tests
	test/src/editingcommands.cpp
)





