local log = require 'log'
local util = require "l_util"
local zmb_apps = require 'application.zmb_apps'
local security = require "security"
local config = require 'menu.config_page'

require 'menu.config_lib.app'

local configuring = false
local initialized = false

local default_icon = "zmbapp.png"

local separation = 15
local grid_button_h = 90
local grid_button_w = 105
local grid_width = grid_button_w * 3 + separation * 2
local grid_height = grid_button_h * 2 + separation * 1
local height = 0
local init_x = 0

apps_grid = {}

local grid_color_table = {
	bgColor = Color.Gray4,
	bgButtonColor = Color.Gray3,
	selectedButtonColor = Color.Orange,
	bgLabelColor = Color.None,
	scrollColor = Color.Gray6,
	scrollBgColor = Color.Gray3,
}

local function help( msg )
	return msg .. ".\nOK = Ingresar  EXIT = Salir del menú    MENU = Menú ppal."
end

local function resfresh()
	wgt:setVisible(apps_grid.apps_grid, configuring and FALSE or TRUE)
	wgt:setVisible(apps_grid.app_win, configuring and TRUE or FALSE)
end

local function get_app_path( app )
	local pos = #app.file
	local found = false
	
	while (not found) and pos ~= 1 do
		if app.file:sub(pos, pos) == '/' then
			found = true
		else
			pos = pos -1
		end
	end

	return app.file:sub(1, pos)
end

local function app_icon( app )
	local icon = nil
	local selectedIcon = nil
	local imgsPath = get_app_path(app)
	if app.icon == nil then
		icon = default_icon
		selectedIcon = "selected_" .. default_icon
	else
		icon = imgsPath .. app.icon
		if app.selectedIcon ~= nil then
			selectedIcon = imgsPath .. app.selectedIcon
		else
			selectedIcon = icon
		end
	end
	return icon, selectedIcon
end

local function enable_app(app)
	app_config_win.setApp(app)
	configuring = true
	resfresh()
end

local function fill_apps()
	local iconH = 40
	local sep = 3
	local appsPerRow = 3
	local apps = zmb_apps.list()

	local rows = math.ceil(#apps / appsPerRow)

	local init_y = (height - (rows * grid_button_h)) / 2
	
	for i=1, rows, 1 do
		buttongrid:appendRow(apps_grid.apps_grid, init_y)
	end
	
	local i = 1
	local j = 0
	local lastRow = math.ceil(i / appsPerRow) - 1
	for _,app in pairs(apps) do
		local newRow = math.ceil(i / appsPerRow ) - 1
		if lastRow ~= newRow then
			lastRow = newRow
			j = 0
		end
		local btnApp = button:new(init_x + (grid_button_w + separation)*j, 0, grid_button_w, grid_button_h, config.buttons_color_table)
		local icon, selectedIcon = app_icon(app)
		button:label(btnApp, string.upper(app.name))
		button:icon(btnApp, icon, selectedIcon, 55, 55, iconH, sep)
		button:action(btnApp, function() enable_app(app) end)
		button:selectedCallback(btnApp, function() updateHelp(help("Configura la aplicación " .. app.name)) end)
		buttongrid:addButton(apps_grid.apps_grid, btnApp, newRow)
		i = i + 1
		j = j + 1
	end

	wgt:fixOffsets(apps_grid.apps_grid)
end

local function select_apps_grids()
	do -- Hack: Same that openSubWin but bypassing the select method
		closeCurrent()
		selectWin(apps_grid)
	end

	if not initialized then
		fill_apps()
		initialized = true
	end
	if not configuring then
		show_grid(true)
	end
end

local function create_grid( w, h )
	height = h
	local grid = buttongrid:new(0, 0, w, h, 3, grid_button_h, separation, 0, grid_color_table)
	buttongrid:scrollImages(grid, "arrow_up_gris.png", "arrow_down_gris.png")

	return grid
end

function show_grid( reset_grid )
	if reset_grid then
		buttongrid:setActiveButton(apps_grid.apps_grid, 0, 0)
	else
		buttongrid:setActiveButton(apps_grid.apps_grid, buttongrid:getActiveButton(apps_grid.apps_grid))
	end

	configuring = false
	resfresh()
end

function apps_grid.create( x, y, w, h )
	init_x = (w - grid_width) / 2

	apps_grid.win = window:new(x, y, w, h, menuZIndex, menuColorTable)
	apps_grid.apps_grid = create_grid(w, h)
	apps_grid.app_win = app_config_win.create(0, 0, w, h)

	window:addChild(apps_grid.win, apps_grid.app_win)
	window:addChild(apps_grid.win, apps_grid.apps_grid)

	wgt:setVisible(apps_grid.win, FALSE)

	return apps_grid.win
end

function apps_grid.handleKey(key)
	local accepted = false

	if configuring then
		accepted = app_config_win.handleKey(key)
	end

	if (not accepted) and (key == KEY_EXIT or key == KEY_ESCAPE) then
		openSubWin(menu)
		accepted = true
	elseif not accepted then
		accepted = wgt:processKey(apps_grid.apps_grid, key)
	end

	return accepted
end

function apps_grid:onSelected()
	return security.call(select_apps_grids)
end

function apps_grid:onUnselected()
	configuring = false
	return true
end

function apps_grid.help()
	return help("Configuración de aplicaciones")
end
