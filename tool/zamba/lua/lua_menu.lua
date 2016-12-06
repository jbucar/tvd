local log = require 'log'
local l_util = require 'l_util'
local disable_support = require 'menu.util.disable_support'
local Widget = require "basewidget"
local config = require "menu.config_page"
local channels = require "menu.channels"
local usb = require "menu.usb"
local about = require "menu.about"
local configuration = require "menu.config"

local Menu = {}
setmetatable(Menu, Widget.mt) -- Inherits behaviour from Widget
Menu_mt = { __index = Menu }

local help_box = nil
local restorePage = false

local controller = nil

local buttons = nil
local title = nil

local base = 20

local buttons_color_table = colors_extend(menuColorTable, {bgColor = Color.Gray4, textColor = Color.White, selectedTextColor = Color.Orange})
local help_color_table = colors_extend(buttons_color_table, {textColor = wgt:newColor( 249,  249,  249)})

-- Internals

local function format_help( msg )
	return msg .. ".\nOK = Entrar a la solapa   EXIT/MENU = Salir del menú"
end

-- Controllers impl

local MenuController = { target = nil }
local PageController = { target = nil }

function MenuController:processKeys( key )
	log.trace("LuaMenu", "processKeys")
	if wgt:processKey(buttons, key) then
		return true
	end
	return false
end

function MenuController:onDigit( key )
	log.trace("LuaMenu", "onDigit")
end

function MenuController:onMenu()
	closeWgt(self.target)
	return true
end

function PageController:processKeys( key )
	log.trace("PageController", "processKeys")

	if self.target:processKeys(key) then
		return true
	end

	if key == KEY_EXIT or key == KEY_ESCAPE then
		log.trace("PageController::Menu", "processKeys", "esc keys")
		self:deactive()
		return true
	end

	return false
end

function PageController:onDigit( digit )
	if self.target.onDigit then
		self.target:onDigit(digit)
	end
end

function PageController:onMenu()
	l_util.ensure_call(self.target.onMenu, self.target)
	return true
end

function PageController:deactive()
	log.trace("PageController", "deactive")
	controller = MenuController
	disable_support.disable(title)
	l_util.ensure_call(self.target.onSelected, self.target)
end

-- API

function Menu:onShow()
	log.trace("LuaMenu", "onShow")
	controller = MenuController
	verticalset:setActiveItem(buttons, 0)
	return true
end

function Menu:onHide()
	log.trace("LuaMenu", "onHide")
	verticalset:setActiveItem(buttons, -1)
	l_util.ensure_call(controller.deactive, controller)
	return true
end

function Menu:processKeys( key )
	log.trace("LuaMenu", "processKeys")
	return controller:processKeys(key)
end

function Menu.onDigit( digit )
	log.trace("LuaMenu", "onDigit")
	controller:onDigit(digit)
	return true -- not bubble this event
end

function Menu:onMenu()
	log.trace("LuaMenu", "onMenu")
	return controller:onMenu()
end

function Menu:select_page( page, actived )
	log.trace("LuaMenu", "select page", "page " .. page:title() .. " actived = " .. tostring(actived))

	if actived then
		label:setText(title, page:title())

		disable_support.disable(title)

		l_util.ensure_call(page.onSelected, page)
		wgt:setVisible(page:getWgt('win'), TRUE)
	else
		wgt:setVisible(page:getWgt('win'), FALSE)
		l_util.ensure_call(page.onUnselected, page)
	end
end

function Menu:active( page )
	disable_support.enable(title)
	l_util.ensure_call(page.onActivated, page)
	PageController.target = page
	controller = PageController
end

function Menu:add_btn( x, y, page )
	local btn = button:new(x, y, config.menu.button_w, config.menu.button_h, config.buttons_color_table)

	local title = page:title()
	button:label(btn, title)
	wgt:setName(btn, title)

	local icon = page:icon()
	button:icon(btn, icon .. ".png", icon .. "_selec.png", config.menu.icon_w, config.menu.icon_h, 40, config.menu.icon_sep);

	if not page.deny_activation then
		button:action(btn, function() self:active(page) end)
	end

	button:selectedCallback(btn, function(actived) self:select_page(page, actived) end)

	verticalset:addWidget(buttons, btn)

	wgt:setName(page:getWgt('win'), string.format("Page(%s)", title))
end

function setRestorePage( value )
	restorePage = value
end

function updateHelp( help_msg, format )
	if help_box then
		wgt:setVisible(help_box, TRUE)
		if format then
			textarea:setText(help_box, format_help(help_msg))
		else
			textarea:setText(help_box, help_msg)
		end
	end
end

function closeHelp()
	log.trace("LuaMenu", "closeHelp")
	wgt:setVisible(help_box, FALSE)
	wgt:needsClear(help_box, true)
	wgt:invalidate(help_box)
end

function Menu:restoreMenu()
	if restorePage then
		openWgt( self )
		processKeys( KEY_CURSOR_DOWN )
		processKeys( KEY_ENTER )
		restorePage = false
		view.refresh()
	end
end

function Menu:add_page( page, index )
	local win_page = window:new(config.page.x, config.page_y + config.page_border + config.page_margin, config.page.width, config.page.height, menuZIndex, menuColorTable)
	wgt:setVisible(win_page, FALSE)

	local inst = page.new(win_page)
	inst:addWgt('win', win_page)

	self:add_btn(0, base + (config.menu.button_h + 20) * index, inst)

	window:addChild(container, win_page)
end

function init()
	local menu = Widget.new("MainMenu", true, true)
	setmetatable(menu, Menu_mt)

	local wh = config.menu.y + config.container.y + config.container.h + config.menu_separation + config.help_height
	local win = window:new(config.menu.x, config.menu.y, config.menu.w, wh, menuZIndex, colors_extend(buttons_color_table, {bgColor = Color.None}))
	wgt:setName(win, "LuaMenuWin")
	wgt:setVisible(win, FALSE)

	-- The menu bottons
	buttons = verticalset:new(0, 23, config.menu.button_w, config.menu.buttons_h, buttons_color_table)
	verticalset:circular(buttons, TRUE)
	wgt:setName(buttons, "LuaMenuButtons")
	window:addChild(win, buttons)

	container = window:new(config.container.x, 23, config.container.w, config.container.h, menuZIndex + 1, buttons_color_table)
	do -- Create container
		wgt:setName(container, "LuaMenuContainer")
		window:addChild(win, container)

		-- Title Page
		title = label:new(config.menu.title.x, config.menu.title.y, config.container.w, config.menu.title.h, disable_support.disable_color_table, "", 15, Alignment.vCenter_hCenter)
		wgt:setName(title, "LuaMenuTitle")
		window:addChild(container, title, FALSE)

		menu:add_page(channels, 0)
		menu:add_page(usb, 1)
		menu:add_page(configuration, 2)
		menu:add_page(about, 3)
	end

	-- Logo
	local logo = image:new(config.logo.x, 0, config.logo.width, config.logo.height, widgetZIndex, {}, "Logo.png")
	window:addChild(win, logo)

	-- Helpbox
	help_box = textarea:new(config.container.x, 23 + config.container.h + config.menu_separation, config.container.w, config.help_height, help_color_table, "", 14, 4, 10, Alignment.vCenter_hLeft)
	wgt:setName(help_box, "LuaMenuHelpBox")
	window:addChild(win, help_box)

	-- menu:addWgt('help_box', help_box)

	wgt:fixOffsets(win)
	menu:addWgt('menu_win', win)

	MenuController.target = menu
	
	menu:ignore({
		'onInfo',
		'onEPG',
		'onChannelList',
		'onAudio',
		'onSub',
		'onAspect',
		'onChannel',
		'onTVmode',
	})

	return menu
end

return init()
