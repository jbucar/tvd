local log = require 'log'
local Alert = require 'widgets.alert'

require "menu.config_lib.security_lib.advance_security"

set_pin_win = {
	pinStored = {}, 
	pinUnmatched = {}
}

local PINcolorTable = {  
	bgColor = Color.None,
	bgLabelColor = Color.Gray4,
	textColor = Color.White
}
		
local modifyPINcolorTable = {  
	bgColor = Color.None,
	bgLabelColor = Color.Gray3,
	selectedColor = Color.Orange,
	textColor = Color.Black,
	selectedTextColor = Color.Black,
}

-- Internals

local function pinStored()
	openSubWin(advance_security)
end

local function reset_fields()
	label:setText(set_pin_win.pin1, "")
	label:setText(set_pin_win.pin8, "")
end

local function activate_label()
	local activeLabel, inactiveLabel
	local update = false
	if (#label:getRawText(set_pin_win.pin1) == 0) then
		set_pin_win.labelsAreActive = true
		activeLabel   = set_pin_win.label1
		inactiveLabel = set_pin_win.label2
		update = true
	end
	if (#label:getRawText(set_pin_win.pin1) == PINSize() - 1) then
		activeLabel   = set_pin_win.label2
		inactiveLabel = set_pin_win.label1
		update = true
	end
	if update then
		label:setBgColor(inactiveLabel, modifyPINcolorTable.bgLabelColor)
		label:setColor  (inactiveLabel, modifyPINcolorTable.textColor)
		label:setBgColor(activeLabel,   modifyPINcolorTable.selectedColor)
		label:setColor  (activeLabel,   modifyPINcolorTable.selectedTextColor)
		wgt:repaint(set_pin_win.pin1)
		wgt:repaint(set_pin_win.pin8)
	end
end


local function reset()
	log.debug("Menu::Config::Security::SetPin", "reset")
	reset_fields()
	activate_label()
end

local function create_dialogs()
	set_pin_win.pinStored = Alert.new()
	set_pin_win.pinStored:text("La nueva clave ha sido guardada.", 3)
	set_pin_win.pinStored:title("CLAVE")
	set_pin_win.pinStored:callback(pinStored)

	set_pin_win.pinUnmatched = Alert.new()
	set_pin_win.pinUnmatched:callback(reset)
	set_pin_win.pinUnmatched:text("Los valores no coinciden. Por favor, ingrese nuevamente la clave y su confirmación.", 5 )
end
-- Public

function set_pin_win.create(x, y, w, h)
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)

	set_pin_win.label1 = label:new( 0, 30, 404, 26, modifyPINcolorTable, "Escriba una clave de 4 números", 15, Alignment.vTop_hLeft, 3)
	set_pin_win.pin1 = label:new( 260, 33, 140, 20, PINcolorTable, "", 16, Alignment.vBottom_hLeft, 0)
	label:setMask(set_pin_win.pin1, "    *")
	label:setEmptyMask(set_pin_win.pin1, "    _")
	label:setTextLength(set_pin_win.pin1, 4)

	set_pin_win.label2 = label:new( 0, 90, 404, 26, modifyPINcolorTable, "Escriba nuevamente la clave", 15, Alignment.vBottom_hLeft, 3)
	set_pin_win.pin8 = label:new( 260, 93, 140, 20, PINcolorTable, "", 16, Alignment.vBottom_hLeft, 0)
	label:setMask(set_pin_win.pin8, "    *")
	label:setEmptyMask(set_pin_win.pin8, "    _")
	label:setTextLength(set_pin_win.pin8, 4)
	

	window:addChild(win, set_pin_win.label1)
	window:addChild(win, set_pin_win.pin1)

	window:addChild(win, set_pin_win.label2)
	window:addChild(win, set_pin_win.pin8)
	
	set_pin_win.win = win

	wgt:setVisible(set_pin_win.win, FALSE)
	
	create_dialogs()

	return set_pin_win.win
end

function set_pin_win:onSelected()
	reset()
	return true
end

function set_pin_win.handleKey( key )
	if escKeys()[key] then
		openSecurity()
		return true
	elseif navigationKeys()[key] then
		return true
	else 
		return false
	end
end

function set_pin_win.help()
	return "Ingrese la clave"
end

function set_pin_win.onUnselected()
	reset_fields()
	return true
end

function set_pin_win:onDigit(d)
	log.debug("Menu::Config::SetPin", "onDigit", tostring(d))

	activate_label()
	if (#label:getRawText(set_pin_win.pin1) < PINSize()) then
		label:setText(set_pin_win.pin1, label:getRawText(set_pin_win.pin1) .. d)
	else
		label:setText(set_pin_win.pin8, label:getRawText(set_pin_win.pin8) .. d)
	end

	local pin1raw = label:getRawText(set_pin_win.pin1)
	local pin8raw = label:getRawText(set_pin_win.pin8)
	log.debug("Page_PIN", "Page_PIN.onDigit", "pins="..pin1raw.. ", "..pin8raw)
	if (#pin1raw == #pin8raw) then
		if (pin1raw == pin8raw) then
			log.debug("Page_PIN", "pin set, " .. pin1raw)
			control.setPIN( pin1raw )
			openWgt(set_pin_win.pinStored)
		else
			log.debug("Page_PIN", "unmatched pins")
			openWgt(set_pin_win.pinUnmatched)
		end
	end
	return true
end
