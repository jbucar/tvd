local log = require 'log'
local l_util = require 'l_util'
local Widget = require "basewidget"
local config = require "menu.config_page"

-- tabs
local apps = require 'menu.content_lib.apps'
local images = require 'menu.content_lib.images'
local music = require 'menu.content_lib.music'
local video = require 'menu.content_lib.videos'

local _M = {}

local Usb = {}
setmetatable(Usb, Widget.mt) -- Inherits behaviour from Widget
Usb_mt = { __index = Usb }

-- Vars
local lists = {}
local current_list = nil

-- View
local button_h = 45

-- Internals

local function help( tab )
	return string.format("%s.\nOK = %s.    EXIT/MENU = Menú ppal.", tab.help, tab.action_help)
end

-- API

function Usb:onActivated()
	log.trace("LuaMenu::Usb", "onActivated")
	buttonset:setActiveButton(self:getWgt("buttons"), 0)
	current_list:enable_view()
	return true
end

function Usb:onSelected()
	log.trace("LuaMenu::Usb", "onSelected")
	buttonset:setActiveButton(self:getWgt("buttons"), -1)
	self:select_list(true, 1)
	current_list:disable_view()
	updateHelp("Permite acceder a contenidos multimedia", true)

	return true
end

function Usb:onUnselected()
	log.trace("LuaMenu::Usb", "onUnselected")
	buttonset:setActiveButton(self:getWgt("buttons"), -1)
	current_list:unselect()
	return true
end

function Usb:processKeys( key )
	log.trace("LuaMenu::Usb", "processKeys")
	if wgt:processKey(self:getWgt("buttons"), key) then
		return true
	else 
		return current_list:handle_key(key)
	end
end

function Usb:onMenu()
	processKeys(KEY_ESCAPE)
	return true
end

function Usb:title()
	return "CONTENIDOS"
end

function Usb:icon()
	return "Menu-2-Contenidos"
end

function Usb:select_list( selected, index )
	if selected then
		log.trace("LuaMenu::Usb", "select_list", tostring(index))

		current_list = lists[index]
		updateHelp(help(current_list))

		current_list:select()
	elseif current_list then
		current_list:unselect()
	end
end

local botton_w = 90
local x = (config.page.buttons.w - botton_w * 4) / 2

function Usb:add_btn( tab, index )
	local buttons = self:getWgt('buttons')

	local btn = button:new(x + botton_w * index, 10, botton_w, 60, config.buttons_color_table)
	button:label(btn, tab.title)
	button:icon(btn, 'content_' .. tab.icon .. ".png", 'content_' .. tab.icon .. "_selected.png", 35, 30, 45, config.sep);

	button:action(btn, function() tab:on_enter() end)
	button:selectedCallback(btn, function( selected ) self:select_list(selected, buttonset:getCurrentIndex(buttons) + 1) end)

	buttonset:addButton(buttons, btn)

	-- button:enable(btn, tab.enable)
end

function Usb:create_tab( win, tab, index )
	local instance = tab.new(win)
	table.insert(lists, instance)
	self:add_btn(instance, index)
end

function _M.new( win )
	local page = Widget.new("MenuUsb", false, false)
	setmetatable(page, Usb_mt)

	-- Buttons
	local buttons = buttonset:new(config.page.buttons.x, config.page.buttons.y, config.page.buttons.w, config.button_h, menuColorTable)
	buttonset:circular(buttons, TRUE)
	window:addChild(win, buttons)
	page:addWgt('buttons', buttons)

	do -- Tabs
		page:create_tab(win, apps, 0)
		page:create_tab(win, images, 1)
		page:create_tab(win, music, 2)
		page:create_tab(win, video, 3)
	end

	buttonset:setActiveButton(buttons, 0)

	return page
end

return _M
