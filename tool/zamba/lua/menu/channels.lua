local log = require 'log'
local disable_support = require 'menu.util.disable_support'
local Widget = require "basewidget"
local Confirmation = require 'widgets.confirmation'
local config = require "menu.config_page"
local scanner = require "menu.channel_lib.scan"
local scan = require "models.scan"
local security = require "security"

require 'channel_list'
require "nav"

local _M = {}

local Channels = {}
setmetatable(Channels, Widget.mt) -- Inherits behaviour from Widget
Channels_mt = { __index = Channels }

local channel_list = nil
local refresh = nil

local button_w = 76
local button_h = 60


-- Internals

local function one_seg_label()
	if channel.showMobile() then
		return "Deshabilitar"
	else
		return "Habilitar"
	end
end

local function refresh_buttons( buttons, one_seg_btn )
	for i=1, 4 do
		buttonset:enableButton(buttons, i, channels.count() > 0 and TRUE or FALSE)
	end
	button:label(one_seg_btn, one_seg_label())
end

function refresh_channel_list()
	chListSetChannels(channel_list, channels.getAll())
	if channels.count() == 0 then
		chListSelectChannel(channel_list, -1)
	end
end

local function delete_channel()
	local chid = chListCurrentChannel(channel_list)
	channelDeleted(tonumber(chid)) -- tell the chs.lua module
	channels.remove(chid)
	local size = #channels.getAll()
	if chListSelectedIndex(channel_list) == size and size ~= 0 then
		chListSelectChannel(channel_list, size-1)
	end

	refresh()
end

local function create_confirmation( txt, callback )
	local confirm = Confirmation.new()
	confirm:onYes(callback)
	confirm:text(txt, 5)
	return confirm
end

local function toggle_flag( flag )
	local id = chListCurrentChannel(channel_list)
	flag(id)
	local changedChannel = channels.get(id)
	chListUpdateChannel(channel_list, changedChannel)
end

local function one_seg_help()
	if channel.showMobile() then
		return "Deshabilita todos los canales One-Seg"
	else
		return "Habilita todos los canales One-Seg"
	end
end

local function help( msg )
	return msg .. "\nOK = Realizar acción    EXIT/MENU = Menú ppal."
end

local function add_btn( buttons, x, label, icon, help_msg, callback, selected_callback )
	local btn = button:new(x, 10, button_w, button_h, config.buttons_color_table)
	button:label(btn, label)
	button:icon(btn, 'channels_' .. icon .. '.png', 'channels_' .. icon .. '_selected.png', 35, 30, 45, config.sep, 'channels_' .. icon .. '_disabled.png');
	button:action(btn, callback)

	local on_selected = nil
	if selected_callback then
		on_selected = function( selected )
			selected_callback(selected)
			if selected then updateHelp(help(help_msg)) end
		end
	else
		on_selected = function()
			updateHelp(help(help_msg))
		end
	end
	button:selectedCallback(btn, on_selected)

	buttonset:addButton(buttons, btn)

	return btn
end

local function ignore_up_and_down( ignore )
	wgt:ignoreKey(channel_list.list, KEY_CURSOR_UP, ignore)
	wgt:ignoreKey(channel_list.list, KEY_CURSOR_DOWN, ignore)
end

local function on_channel_found( ch )
	log.trace("LuaMenu::Channels", "on_channel_found")
	chListAddChannel(channel_list, ch)
end

local function on_begin_scan()
	log.trace("LuaMenu::Channels", "on_begin_scan")
	chListRefresh(channel_list)
	chListSetChannels(channel_list, {})
	chListSelectChannel(channel_list, -1)
end

local function start_scan( dlg, btnSet )
	log.trace("LuaMenu::Channels", "start_scan")
	if channels.count() > 0 then -- warn only when there are channels
		openWgt(dlg)
	else
		security.call(tuner.startScan)
	end
end

-- Buttons callback

local function fav_pressed()
	log.trace("LuaMenu::Channels", "fav_pressed")
	if channels.count() > 0 then
		toggle_flag(channels.toggleFavorite)
	end
end

local function del_pressed( dlg )
	log.trace("LuaMenu::Channels", "del_pressed")
	if channels.count() > 0 then
		openWgt(dlg)
	end
end

local function bloq_pressed()
	log.trace("LuaMenu::Channels", "bloq_pressed")
	if channels.count() > 0 then
		security.required(function() toggle_flag(channels.toggleBlocked) end)
	end
end

local function oneseg_pressed( one_seg_btn )
	log.trace("LuaMenu::Channels", "oneseg_pressed")
	channel.toggleMobile()
	button:label(one_seg_btn, one_seg_label())
	wgt:repaint(one_seg_btn)
	updateHelp(help(one_seg_help()))
end

local function disable_channel_list()
	log.trace("LuaMenu::Channels", "disable_channel_list")

	if chListSelectedIndex(channel_list) ~= -1 then
		ignore_up_and_down(TRUE)
		chListRefresh(channel_list, FALSE)
		channel_list.lastChannelSelected = chListSelectedIndex(channel_list)
		chListSelectChannel(channel_list, -1)
	end
end

local function enable_channel_list()
	log.trace("LuaMenu::Channels", "enable_channel_list")
	if chListSelectedIndex(channel_list) == -1 then
		ignore_up_and_down(FALSE)
		chListRefresh(channel_list, TRUE)
		channel_list.lastChannelSelected = math.min(channel_list.lastChannelSelected, chListRowCount(channel_list)-1)
		chListSelectChannel(channel_list, channel_list.lastChannelSelected)
	end
end

-- API

function Channels:onActivated()
	log.trace("LuaMenu::Channels", "onActivated")
	buttonset:setActiveButton(self:getWgt("buttons"), 0)
	disable_support.enable(channel_list.list, {textColor = Color.Black})
	return true
end

function Channels:onSelected()
	log.trace("LuaMenu::Channels", "onSelected")
	buttonset:setActiveButton(self:getWgt("buttons"), -1)
	chListSelectChannel(channel_list, -1)
	disable_support.disable(channel_list.list, disable_support.disabled_list)
	refresh()
	ignore_up_and_down(TRUE)
	updateHelp("Administración de la lista de canales", true)
	return true
end

function Channels:onUnselected()
	log.trace("LuaMenu::Channels", "onUnselected")
	return true
end

function Channels:processKeys( key )
	log.trace("LuaMenu::Channels", "processKeys")

	if tuner.isScanning() then
		if key == KEY_EXIT or key == KEY_ESCAPE then
			log.debug("LuaMenu::Channels", "processKeys", "esc keys and scanning")
			tuner.cancelScan()
		end
		return true
	end

	if not wgt:processKey(self:getWgt("buttons"), key) then
		if not wgt:processKey(channel_list.list, key) then
			return false
		else
			return true
		end
	else
		log.debug("LuaMenu::Channels", "buttons managed")
	end

	return true
end

function Channels:onMenu()
	processKeys(KEY_ESCAPE)
	return true
end

function Channels:title()
	return "CANALES"
end

function Channels:icon()
	return "Menu-1-Canales"
end

function _M.new( win )
	local page = Widget.new("MenuChannels", false, false)
	setmetatable(page, Channels_mt)

	-- Channel list

	local name_w = config.page.list.width * 0.4
	local others_w = config.page.list.width * 0.2
	channel_list = chListCreate('Channel_List', 0, 0, config.page.width, config.page.height, 9, {name_w, others_w, others_w, others_w} )
	window:addChild(win, channel_list.list)

	-- Buttons
	local buttons = buttonset:new(config.page.buttons.x, config.page.buttons.y, config.page.buttons.w, config.button_h, menuColorTable)
	buttonset:circular(buttons, TRUE)
	page:addWgt('buttons', buttons)
	window:addChild(win, buttons)

	-- Scan
	local scan_confirm = create_confirmation(
		"¿Está seguro que desea actualizar la lista de canales? Si lo hace, perderá las marcas de favoritos y bloqueados, y recuperará los canales borrados.",
		function() security.call(tuner.startScan) end
	)

	add_btn(buttons, 0, "Buscar", "search", "Busca y actualiza la lista de canales", function() start_scan(scan_confirm, buttons) end, l_util.call_if_condition(disable_channel_list))

	-- Show/Hide One Seg
	local one_seg = add_btn(buttons, button_w, one_seg_label(), "oneseg", one_seg_help(), function() end, l_util.call_if_condition(disable_channel_list))
	button:action(one_seg, function() oneseg_pressed(one_seg) end)
	button:enable(one_seg, channels.count())

	local pos = function(index) return config.page.width - button_w * index end

	-- Favourite
	local fav = add_btn(buttons, pos(3), "Fav.", "fav", "Marca o desmarca como favorito el canal seleccionado", fav_pressed, l_util.call_if_condition(enable_channel_list))
	button:enable(fav, channels.count())

	-- Block
	local block = add_btn(buttons, pos(2), "Bloq.", "block", "Bloquea o desbloquea el canal seleccionado", bloq_pressed)
	button:enable(block, channels.count())

	-- Delete
	local delete_confirm = create_confirmation(
		"¿Está seguro que desea borrar el canal seleccionado?\nPara recuperarlo deberá actualizar la lista de canales con \"Buscar canales\".",
		function() security.call(delete_channel) end
	)
	local delete = add_btn(buttons, pos(1), "Borrar", "delete", "Borra el canal seleccionado", function() del_pressed(delete_confirm) end, l_util.call_if_condition(enable_channel_list))
	button:enable(delete, channels.count())

	list:setSelected(channel_list.list, -1)

	refresh = function()
		refresh_channel_list()
		refresh_buttons(buttons, one_seg)
	end

	-- Scanner
	local scan_win = scanner.create(config.page.buttons.x, config.page.buttons.y, config.page.buttons.w, config.button_h)
	window:addChild(win, scan_win)

	scan.observe('newChannelFound', on_channel_found)
	scan.observe('beginScan', on_begin_scan)
	scan.observe('beginScan', function() list:autoscroll(channel_list.list, true); scanner.open(); wgt:setVisible(buttons, FALSE); wgt:invalidate(win) end)
	scan.observe('endScan', function() list:autoscroll(channel_list.list, false); wgt:setVisible(buttons, TRUE); end)
	scan.observe('endScan', refresh)

	return page
end

return _M
