local log = require 'log'
local security = require "security"
local Confirmation = require 'widgets.confirmation'
local Alert = require 'widgets.alert'

restore = {}

-- Internals

local function reset_configuration()
	mainWindow.showBackground(TRUE)
	mainWindow.resetConfig()
	control.setExpireTime( -1 )
	zmb_apps.restore()
	general.needsRestore = true

	local flash = Alert.new()
	flash:title("RESTAURACIÓN EXITOSA")
	flash:text("La restauración se ha realizado correctamente.", 3)
	flash:callback(function() openSubWin(menu) end)
	openWgt(flash)
end

local function confirm_reset_config()
	return security.call(reset_configuration)
end

local function create_restore_confirmation()
	local confirm = Confirmation.new()
	confirm:onYes(confirm_reset_config)
	confirm:text("¿Está seguro que desea hacerlo?", 5)
	return confirm
end

-- API

function restore.create(x, y, w, h)
	log.trace("zamba", "Create page config restore with x: " .. tostring(x) .. " y: " .. tostring(y))
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)
	local restoreBtn = button:new(0, y + h/2, w, 26, buttonColorTable)
	restore.btnSet = buttonset:new(0, 0, 0, 0, menuColorTable)

	local restore_dialog = create_restore_confirmation()

	button:label(restoreBtn, "Restaurar valores de fábrica")
	button:fontSize(restoreBtn, 15)
	button:action(restoreBtn, function() openWgt(restore_dialog) end)
	
	local text = textarea:new(x, (y + h/2) - 75, w, 50, menuColorTable
		 , "Advertencia.\nSe perderán los canales y los ajustes realizados.", 16, 2, 0, Alignment.vTop_hCenter)

	buttonset:addButton(restore.btnSet, restoreBtn)
	buttonset:setActiveButton(restore.btnSet, 0)
	
	window:addChild(win, text)
	window:addChild(win, restore.btnSet)
	restore.win = win

	wgt:setVisible(restore.win, FALSE)
	return win
end

function restore.handleKey(key)
	log.trace("LuaMenu::Config::Menu", "handleKey", tostring(key))

	if key == KEY_EXIT or key == KEY_ESCAPE then
		openSubWin(menu)
		return true
	elseif key == KEY_CURSOR_UP or key == KEY_CURSOR_DOWN then
		return true
	else
		return  wgt:processKey(restore.btnSet, key)
	end
end

function restore:onSelected()
	updateHelp("Restaurar valores de fábrica.\nOK = Restaurar    EXIT = Salir del menú    MENU = Menú ppal.")
	return true
end
