--- KeyMap for Stages Module

function KeysMap_getMapForStage( anStage )
	local keyMap = {
		[KEY_CURSOR_LEFT] = function() anStage:leftPressed() end,
		[KEY_CURSOR_RIGHT] = function() anStage:rightPressed() end,
		[KEY_CURSOR_UP] = function() anStage:upPressed() end,
		[KEY_CURSOR_DOWN] = function() anStage:downPressed() end,
		[KEY_ENTER] = function() anStage:enterPressed() end,
		[KEY_OK] = function() anStage:enterPressed() end,
		[KEY_SMALL_C] = function() anStage:menuPressed() end, --cambiar cuando se deba, esto debería ser enterPressed
		[KEY_MENU] = function() anStage:menuPressed() end, --cambiar cuando se deba, esto debería ser enterPressed
		[KEY_NUMBER_0] = function() end,
		[KEY_NUMBER_1] = function() end,
		[KEY_NUMBER_2] = function() end,
		[KEY_NUMBER_3] = function() end,
		[KEY_NUMBER_4] = function() end,
		[KEY_NUMBER_5] = function() end,
		[KEY_NUMBER_6] = function() end,
		[KEY_NUMBER_7] = function() end,
		[KEY_NUMBER_8] = function() end,
		[KEY_NUMBER_9] = function() end,
	}
	return keyMap
end