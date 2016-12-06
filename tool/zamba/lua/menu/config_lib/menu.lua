local log = require 'log'
local config = require 'menu.config_page'

menu = {}

local btnH = 90
local btnW = 105
local iconH = 65
local but_sep = 3
local separation = 15
local rowSep = 50
local grid_width = btnW * 3 + separation * 2
local grid_height = btnH * 2 + rowSep * 1
local rows = 2
local lastRow = 0
local lastColumn = 0
local init_x = 0

local function help( msg )
	return msg .. "\nOK = Ingresar   EXIT/MENU = Menú ppal."
end

local function add_btn( x, label, icon, callback, help_msg )
	local btn = button:new(init_x + x, 0, btnW, btnH, config.buttons_color_table)
	button:label(btn, label)
	button:icon(btn, "config_" ..  icon .. ".png", "config_" ..  icon .. "_selected.png", 55, 55, iconH, but_sep)
	button:action(btn, callback)
	button:selectedCallback(btn, function(selected) if selected then updateHelp(help(help_msg)) end end)
	return btn
end

function menu.create( x, y, w, h )
	init_x = (w - grid_width) / 2
	local init_y = (h - grid_height) / 2

	menu.win = buttongrid:new(0, 0, w, h, 3, btnH, rowSep, 0, buttonColorTable)

	for i=0, rows-1 do
		buttongrid:appendRow(menu.win, init_y)
	end


	local offset = btnW + separation

	local general = add_btn(0, "AUDIO/VIDEO", "audiovideo", function() openSubWin(general) end, "Configuración de audio y video")
	buttongrid:addButton(menu.win, general, 0)

	local apps = add_btn(offset, "APLICACIONES", "app", function() openSubWin(apps_grid) end, "Configuración de aplicaciones Zamba")
	buttongrid:addButton(menu.win, apps, 0)

	local network = add_btn(offset * 2, "RED", "net", function() openSubWin(network) end, "Configuración de red")
	buttongrid:addButton(menu.win, network, 0)

	local security = add_btn(0, "SEGURIDAD", "security", openSecurity, "Configuraciones de seguridad")
	buttongrid:addButton(menu.win, security, 1)

	local update = add_btn(offset, "ACTUALIZACIÓN", "update", function() openSubWin(update_cfg) end, "Configuración de actualización")
	buttongrid:addButton(menu.win, update, 1)

	local restore = add_btn(offset * 2, "RESTAURAR", "restore", function() openSubWin(restore) end, "Restaurar valores de fábrica")
	buttongrid:addButton(menu.win, restore, 1)

	buttongrid:setActiveButton(menu.win, 0,0)

	return menu.win
end

function menu.onSelected()
	log.debug("LuaMenu::Config::Menu", "onSelected")
	buttongrid:setActiveButton(menu.win, lastColumn, lastRow)
	return true
end

function menu:onUnselected()
	log.debug("LuaMenu::Config::Menu", "onUnselected")
	lastColumn, lastRow = buttongrid:getActiveButton(menu.win)
	buttongrid:setActiveButton(menu.win, -1)
	return true
end

function menu:reset()
	lastColumn = 0
	lastRow = 0
end

function menu.handleKey( key )
	log.debug("LuaMenu::Config::Menu", "handleKey", tostring(key))
	return wgt:processKey(menu.win, key)
end
