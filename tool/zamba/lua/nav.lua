local log = require 'log'
local util = require 'l_util'

local WindowStack = {}

local KeysLocked = false

---------- Key Locking ------------

function lockKeys()
	if not KeysLocked and not CurrentWindow().isRoot then
		KeysLocked = true
		mainWindow.lockKeys(1)
	end
end

function unlockKeys()
	if KeysLocked and not existsFocusableWin() then
		KeysLocked = false
		mainWindow.lockKeys(0)
	end
end

---------- Widgets ------------

function openWgt(widget)
	log.debug( "nav", "openWgt", "widget = " .. tostring(widget.name) .. ", visible = " .. tostring(widget.visible) )
	if not widget.visible then
		if widget.onShow == nil or (widget.onShow ~= nil and widget:onShow()) then
			log.debug( "nav", "openWgt", "insert into stack = " .. tostring(widget.name) )
			pushCurrentWindow(widget)
			if widget.lockKeys then
				lockKeys()
			end
			widget:onOpened()
			-- view.refresh()
		end
	end
end

function closeWgt(widget)
	log.debug( "nav", "closeWgt", "widget = " .. tostring(widget.name) .. ", visible = " .. tostring(widget.visible))
	if widget.visible then
		if widget.onHide == nil or (widget.onHide ~= nil and widget:onHide()) then
			log.debug( "nav", "closeWgt", "removed from stack = " .. tostring(widget.name) )
			removeWindow(widget.name)
			if widget.lockKeys then
				unlockKeys()
			end
			widget:onClosed()

			if #WindowStack == 1 and widget.name ~= "UpdateDlg" then
				checkUpdates()
			end

			-- view.refresh()
		end
	end
end

---------- Window Management ------------

function closeTopWgt()
	if #WindowStack > 1 then
		closeWgt(WindowStack[#WindowStack])
	end
end

function printWindowStack()
	log.debug("nav", "printWindowStack", "#windowStack=" .. tostring(#WindowStack))
	for i=#WindowStack, 1, -1 do
		local window = WindowStack[i]
		if window.name then
			log.debug("nav", "printWindowStack", "window=" .. window.name)
		else
			log.debug("nav", "printWindowStack", "window=?")
		end
	end
end

function removeWindow(windowName)
	local position = #WindowStack
	while ( position > 1) and (windowName ~= WindowStack[position].name) do
		position = position - 1
	end

	if windowName == WindowStack[position].name then
		table.remove(WindowStack, position)
	else
		log.warn("nav", "removeWindow", "Couldn't find " ..  windowName .. " in stack")
	end
end

function pushCurrentWindow(newWindow)
	table.insert(WindowStack, newWindow)
end

function popCurrentWindow()
	table.remove(WindowStack, #WindowStack)
end

function CurrentWindow()
	return WindowStack[#WindowStack]
end

function existsFocusableWin()
	if #WindowStack > 1 then
		for i=2, #WindowStack do --Start from 2 to skip RootNav
			if WindowStack[i].acceptsFocus then
				return true
			end
		end
	end
	return false
end

function closeToRoot()
	for i=#WindowStack,1,-1 do
		closeTopWgt()
	end
	view.refresh()
end

---------- Key processing ------------

local Actions = {
	  [KEY_INFO]         = 'onInfo'
	, [KEY_SMALL_I]      = 'onInfo'

	, [KEY_EPG]          = 'onEPG'
	, [KEY_SMALL_E]      = 'onEPG'

	, [KEY_CHANNEL_UP]   = 'onChannelUp'
	, [KEY_SMALL_Q]      = 'onChannelUp'
	, [KEY_CHANNEL_DOWN] = 'onChannelDown'
	, [KEY_SMALL_Z]      = 'onChannelDown'

	, [KEY_MUTE]         = 'onMute'
	, [KEY_SMALL_M]      = 'onMute'
	, [KEY_VOLUME_UP]    = 'onVolUp'
	, [KEY_SMALL_W]      = 'onVolUp'
	, [KEY_VOLUME_DOWN]  = 'onVolDown'
	, [KEY_SMALL_X]      = 'onVolDown'

	, [KEY_CURSOR_DOWN]  = 'onDown'
	, [KEY_CURSOR_LEFT]  = 'onLeft'
	, [KEY_CURSOR_RIGHT] = 'onRight'
	, [KEY_CURSOR_UP]    = 'onUp'
	, [KEY_OK]           = 'onOk'
	, [KEY_ENTER]        = 'onOk'

	, [KEY_RED]          = 'onRed'
	, [KEY_SMALL_R]      = 'onRed'
	, [KEY_GREEN]        = 'onGreen'
	, [KEY_SMALL_G]      = 'onGreen'
	, [KEY_YELLOW]       = 'onYellow'
	, [KEY_SMALL_Y]      = 'onYellow'
	, [KEY_BLUE]         = 'onBlue'
	, [KEY_SMALL_B]      = 'onBlue'

	, [KEY_NUMBER_0]     = 'onDigit'
	, [KEY_NUMBER_1]     = 'onDigit'
	, [KEY_NUMBER_2]     = 'onDigit'
	, [KEY_NUMBER_3]     = 'onDigit'
	, [KEY_NUMBER_4]     = 'onDigit'
	, [KEY_NUMBER_5]     = 'onDigit'
	, [KEY_NUMBER_6]     = 'onDigit'
	, [KEY_NUMBER_7]     = 'onDigit'
	, [KEY_NUMBER_8]     = 'onDigit'
	, [KEY_NUMBER_9]     = 'onDigit'

	, [KEY_MENU]         = 'onMenu'
	, [KEY_SMALL_C]      = 'onMenu'

	, [KEY_AUDIO]        = 'onAudio'
	, [KEY_SMALL_A]      = 'onAudio'

	, [KEY_PREVIOUS]     = 'onRecall'
	, [KEY_SMALL_P]      = 'onRecall'

	, [KEY_FAVORITES]    = 'onFav'
	, [KEY_SMALL_F]      = 'onFav'

	, [KEY_SUBTITLE]     = 'onSub'
	, [KEY_SMALL_S]      = 'onSub'

	, [KEY_ASPECT]       = 'onAspect'
	, [KEY_SMALL_T]      = 'onAspect'

	, [KEY_CH34]         = 'onChannel'
	, [KEY_SMALL_H]      = 'onChannel'

	, [KEY_MODE]         = 'onTVmode'
	, [KEY_SMALL_V]      = 'onTVmode'

	, [KEY_SMALL_L]      = 'onChannelList'
	, [KEY_CHLIST]       = 'onChannelList'

	, [KEY_F11]          = 'onToggleFullscreen'

}

local function execute_action(window, action_name, key)
	if action_name then
		local action_fnc = window[action_name]
		if action_fnc then
			log.trace("nav", "executeKeyAction", "calling action " .. action_name)
			return action_fnc(key-1)
		end
	end
	return false
end

function processKeys(key)
	local i = #WindowStack
	local keyAccepted = false
	while ( i > 0 and ( not keyAccepted ) ) do
		local widget = WindowStack[i]
		local action = Actions[key]
		log.debug("nav", "processKeys", "window=" .. widget.name .. " acceptsFocus= " .. tostring(widget.acceptsFocus))

		if widget:want_to_be_closed(action) then
			closeWgt(widget)
		elseif widget.acceptsFocus then
			keyAccepted = widget:processKeys(key)

			if not keyAccepted then
				keyAccepted = widget:want_ignore(action) or not widget:want_propagate_event()
			end

			if not keyAccepted then
				keyAccepted = execute_action(widget, action, key)
			end

			if (not keyAccepted) and (key == KEY_EXIT or key == KEY_ESCAPE) and not widget:bypass_escape() then
				closeWgt(widget)
				keyAccepted = true
			end
		end
		i = i - 1
	end
	return keyAccepted
end

function escKeys()
	return { [KEY_EXIT] = true, [KEY_ESCAPE] = true }
end

function navigationKeys()
	return { [KEY_CURSOR_DOWN] = true, [KEY_CURSOR_UP] = true, [KEY_CURSOR_LEFT] = true, [KEY_CURSOR_RIGHT] = true }
end
