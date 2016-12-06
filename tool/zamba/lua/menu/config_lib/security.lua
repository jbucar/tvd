local log = require 'log'
require "menu.config_lib.security_lib.set_pin"

security = {}

function openSecurity()
	if PINSet() then 
		openSubWin(advance_security)
	else 
		openSubWin(security)
	end
end

-- API

function security.create(x, y, w, h)
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)
	local setBtn = button:new(0, y + h/2, w, 26, buttonColorTable)
	security.btnSet = buttonset:new(0, 0, 0, 0, menuColorTable)

	button:label(setBtn, "Establecer clave")
	button:fontSize(setBtn, 15)
	button:action(setBtn, function() openSubWin(set_pin_win) end)
	button:selectedCallback(setBtn, function() updateHelp("Permite establecer una clave de seguridad para el sistema\nOK = Establecer clave   EXIT = Salir del menú    MENU = Menú ppal.") end)
	
	local text = textarea:new(x, (y + h/2) - 75, w, 50, menuColorTable
		  , "Permite establecer una clave para poder controlar la \n seguridad del sistema", 16, 2, 0, Alignment.vTop_hCenter)

	buttonset:addButton(security.btnSet, setBtn)
	
	window:addChild(win, text)
	window:addChild(win, security.btnSet)
	security.win = win

	wgt:setVisible(security.win, FALSE)
	return win
end

function security.handleKey(key)
	log.debug("LuaMenu::Config::Menu", "handleKey", tostring(key))

	if key == KEY_EXIT or key == KEY_ESCAPE then
		openSubWin(menu)
		return true
	elseif key == KEY_CURSOR_UP or key == KEY_CURSOR_DOWN then
		return true
	else
		return  wgt:processKey(security.btnSet, key)
	end
end

function security.onSelected()
	buttonset:setActiveButton(security.btnSet, 0)
	return true
end
