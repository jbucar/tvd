local config = require "menu.config_page"
local log = require "log"
local categories = require 'content_categories'

require "basewidget"
require 'channel_list'

Channel_List_Window = {
	list = nil,
	label = nil
}

local initY = 20
local btnWinH = 102
local chWinH = 392
local sep = 20

local function channels_found( value )
	wgt:setVisible(Channel_List_Window.list.list, value and TRUE or FALSE)
	wgt:setVisible(Channel_List_Window.label, value and TRUE or FALSE)
	wgt:needsClear(Channel_List_Window.list.list, true)
	wgt:setVisible(Channel_List_Window.no_channel, not value and TRUE or FALSE)
end

local function updateChannelList( getter )
	local chs = getter()
	if #chs == 0 then
		log.trace("Channel_List_Window", "No channels")
		channels_found(false)
		return false
	else
		local str = "LISTA DE CANALES - "
		if #chs > 1 then
			str = str .. tostring(#chs) .. " CANALES"
		elseif #chs == 1 then
			str = str .. tostring(#chs) .. " CANAL"
		end

		label:setText(Channel_List_Window.label, str)
		channels_found(true)
		local current = channel.current()

		chListSetChannels( Channel_List_Window.list, chs )

		local idx=0
		for index,value in ipairs(chs) do
			if value.channelID == current then
				idx = index-1
				break
			end
		end

		chListSelectChannel( Channel_List_Window.list, idx )

		return true
	end
end

local extra = {}

local function get_channels_by_condition( cond )
	local result = {}
	for index, value in ipairs(channels.getAll()) do
		if cond(value) then
			table.insert(result, value)
		end
	end
	return result
end

local function get_channels_by_category( cid )
	return get_channels_by_condition( function( ch ) return channels.getInfo(ch.channelID).content == cid end )
end

local fav = {
	name = 'fav',
	width = 36,
	icon = {normal = 'fav.png', selected = 'fav_selected.png'},
	channels = function() return get_channels_by_condition( function( ch ) return ch.favorite == TRUE end ) end
}

local block = {
	name = 'bloq',
	width = 36,
	icon = {normal = 'bloq.png', selected = 'bloq_selected.png'},
	channels = function() return get_channels_by_condition( function( ch ) return ch.blocked == TRUE end ) end
}

local all = {
	name = 'all',
	width = 44,
	icon = {normal = 'all_channels.png', selected = 'all_channels_selected.png'},
	channels = function() return channels.getAll() end
}

table.insert(extra, all)
table.insert(extra, fav)

local function add_button( buttons, x, cat, icon, on_selected )
	local btn = button:new(x, 15, cat.width, 56, config.buttons_color_table)
	button:icon(btn, icon.normal, icon.selected, cat.width, 56, 56, config.sep);
	button:selectedCallback(btn, on_selected)
	buttonset:addButton(buttons, btn)
	wgt:setName(btn, cat.name)
end

local function createButtons( width )
	local initX = 0
	local h = 71

	local buttons = buttonset:new(0, 0, width - 60, h, menuColorTable)
	buttonset:circular(buttons, TRUE)

	local sc = scroll.new(0, 23, width, h, sep, menuColorTable, buttons)
	
	for i=1, #extra do
		add_button(buttons, initX, extra[i], extra[i].icon, l_util.call_if_condition(updateChannelList, extra[i].channels))
		initX = initX + extra[i].width + sep
	end

	for i=1, #categories.values do
		add_button(buttons, initX, categories.values[i], categories.getIcon(categories.values[i].pos), l_util.call_if_condition(updateChannelList, function() return get_channels_by_category(categories.values[i].pos) end))
		initX = initX + categories.values[i].width + sep
	end

	add_button(buttons, initX, block, block.icon, l_util.call_if_condition(updateChannelList, block.channels))

	window:addChild( Channel_List_Window.btnWindow, sc )

	Channel_List_Window:addWgt('buttons', sc)

	scroll.reset(sc, 5)

	return sc
end

function Channel_List_Window.init()
	Channel_List_Window = BaseWidget:new( Channel_List_Window, "Channel_List_Window", true)
	
	local width = canvas_w - 2 * (safe_x + 35)
	local win = window:new(safe_x+34, initY, width, 500, channelListZIndex, colors_extend(menuColorTable, {bgColor = Color.None}), 0)

	Channel_List_Window.btnWindow = window:new(0, 0, width, btnWinH, channelListZIndex, menuColorTable, 0)
	Channel_List_Window.catLabel = label:new(0, 5, width, 14, labelsColorTable, "CATEGORÍAS DE CANALES", 14, Alignment.vTop_hCenter, 0)
	sc = createButtons(width)
	window:addChild(win, Channel_List_Window.btnWindow)
	window:addChild( Channel_List_Window.btnWindow, Channel_List_Window.catLabel, FALSE )
	
	Channel_List_Window.content = window:new(0, btnWinH + 5, width, chWinH, channelListZIndex, menuColorTable, 0)

	local h = 350

	Channel_List_Window.list = chListCreate( 'Channel_List_Standalone', 10, 35, width, h, 13, {364, 100} )
	window:addChild(Channel_List_Window.content, Channel_List_Window.list.list )
	
	Channel_List_Window.label = label:new(0, 5, width, 8, labelsColorTable, "", 14, Alignment.vTop_hCenter, 0)
	window:addChild(Channel_List_Window.content, Channel_List_Window.label )

	Channel_List_Window.no_channel = textarea:new(0, 0, width, h + 35, colors_extend(labelsColorTable, {bgColor = Color.Gray4}), "No hay canales en esta categoría", 16, 2, 0, Alignment.vCenter_hCenter)
	window:addChild(Channel_List_Window.content, Channel_List_Window.no_channel )
	wgt:setVisible(win, FALSE)

	window:addChild(win, Channel_List_Window.content)

	wgt:fixOffsets(win)

	Channel_List_Window:addWgt('window', win)
end

function Channel_List_Window:onShow()
	log.trace("Channel_List_Window", "onShow")
	scroll.reset(self:getWgt("buttons"), 5)
	scroll.selectChild(self:getWgt("buttons"), 0)
	return true
end

function Channel_List_Window:onHide()
	log.trace("Channel_List_Window", "onHide")
	chListSelectChannel(Channel_List_Window.list, -1)
	return true
end

function Channel_List_Window.onChannelList()
	closeWgt(Channel_List_Window)
	return true
end

function Channel_List_Window.onRed()
	closeWgt(Channel_List_Window)
	return true
end

function Channel_List_Window.onOk()
	if (chListRowCount(Channel_List_Window.list) > 0) and ( wgt:isVisible(Channel_List_Window.list.list) == TRUE ) then
		Channel_List_Window.list.lastChannelSelected = chListSelectedIndex( Channel_List_Window.list )
		chListSelectChannel( Channel_List_Window.list, Channel_List_Window.list.lastChannelSelected )
		local id = tonumber( chListCurrentChannel(Channel_List_Window.list) )
		closeWgt(Channel_List_Window)
		view.refresh()
		changeChannel(id, false)
	end

	return true
end

function Channel_List_Window:processKeys(key)
	if key == KEY_CURSOR_LEFT or key == KEY_CURSOR_RIGHT then
		wgt:processKey(self:getWgt("buttons"), key)
		return true
	elseif key == KEY_CURSOR_DOWN or key == KEY_CURSOR_UP then
		wgt:processKey(Channel_List_Window.content, key)
		return true
	end
	return false
end

Channel_List_Window.init()

Channel_List_Window:ignore({
	'onMenu',
	'onInfo',
	'onEPG',
	'onAudio',
	'onSub',
	'onAspect',
	'onChannel',
	'onTVmode',
	'onDigit',
})
