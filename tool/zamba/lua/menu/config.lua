local log = require 'log'
local Widget = require "basewidget"
local config = require "menu.config_page"

require "menu.config_lib.restore"
require "menu.config_lib.general"
require "menu.config_lib.apps_grid"
require "menu.config_lib.network"
require "menu.config_lib.security"
require "menu.config_lib.update"
require "menu.config_lib.menu"

local _M = {}

local Config = {}
setmetatable(Config, Widget.mt) -- Inherits behaviour from Widget
Config_mt = { __index = Config }

-- Vars

local current_win = nil

-- Subwin API
-- TODO: Move this to a generic widget

function openSubWin( subWin )
	log.trace("LuaMenu::Config", "openSubWin")
	if subWin.onSelected == nil or (subWin.onSelected ~= nil and subWin:onSelected()) then
		if closeSubWin(current_win) then
			selectWin(subWin)
		end
	end
end

function closeSubWin( window )
	log.trace("LuaMenu::Config", "closeSubWin", tostring(window))
	if window then
		if window.onUnselected == nil or (window.onUnselected ~= nil and window:onUnselected()) then
			wgt:setVisible(window.win, FALSE)
		else
			return false
		end
	end
	return true
end

function selectWin( subWin )
	log.trace("LuaMenu::Config", "selectWin")
	wgt:setVisible(subWin.win, TRUE)
	current_win = subWin
end

-- Only for hack, use under your own responsibility
function closeCurrent()
	closeSubWin(current_win)
end

-- Widget API

function Config:onActivated()
	log.trace("LuaMenu::Config", "onActivated")
	l_util.ensure_call(current_win.onSelected, current_win)
	return true
end

function Config:onSelected()
	log.trace("LuaMenu::Config", "onSelected")
	selectWin(menu)
	updateHelp("Configuraciones del Set Top Box", true)
	l_util.ensure_call(menu.onUnselected, menu)
	menu:reset()
	return true
end

function Config:onUnselected()
	log.trace("LuaMenu::Config", "onUnselected")
	return true
end

function Config:onMenu()
	log.trace("LuaMenu::Config", "onMenu")
	openSubWin(menu)
	processKeys(KEY_ESCAPE)
	return true
end

function Config:processKeys( key )
	log.trace("LuaMenu::Config", "processKeys", tostring(key))
	return current_win.handleKey(key)
end

function Config:onDigit( digit )
	log.trace("LuaMenu::Config", "onDigit", tostring(digit))
	return l_util.ensure_call(current_win.onDigit, current_win, digit)
end

function Config:title()
	return "AJUSTES"
end
	
function Config:icon()
	return "Menu-3-Config"
end

local function add_sub_win( parent, child )
	local subwin = child.create(0, 0, config.page.width, config.page.height)
	window:addChild(parent, subwin)
end

function _M.new( win )
	local page = Widget.new("MenuConfig", false, false)
	setmetatable(page, Config_mt)

	add_sub_win(win, general)
	add_sub_win(win, apps_grid)
	add_sub_win(win, network)
	do -- Security
		add_sub_win(win, advance_security)
		add_sub_win(win, set_pin_win)
		add_sub_win(win, parental)
		add_sub_win(win, security)
	end
	add_sub_win(win, update_cfg)
	add_sub_win(win, restore)
	add_sub_win(win, menu)

	return page
end

return _M
