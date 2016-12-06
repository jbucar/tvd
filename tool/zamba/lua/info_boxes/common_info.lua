local layout = require 'info_boxes.layout'
local categories = require 'content_categories'
local timer = require 'timer'
require 'colors'

info_ignored_actions = {
	'onChannelList',
	'onLeft',
	'onRight',
	'onMenu',
	'onRecall',
	'onFav',
	'onAspect',
	'onChannel',
	'onTVmode',
	'onEPG',
}

local progressBarColorTable = {
	bgColor = Color.White,
	bgProgressBarColor = Color.White,
	bgBarColor = Color.Orange,
	textColor = Color.Black,
}

local fontSize = 14
local x = 50
local y = 50

local init_x = 66
local init_y = 102
local label_h = 22
local label_w = 84

function getText( str, sufix )
	if str == "" or str == "Unknown" then
		return "---"
	else
		return str .. ( sufix or "" )
	end
end

local function translate( str )
	str = getText(str)
	if str == "Spanish" then
		return "Español"
	end
	return str
end

local function getVideoType()
	local mode = channel:getResolutionMode()
	if mode == '720p' or mode == '720i' or mode == '1080p' or mode == '1080i' then
		mode = 'HD'
	elseif mode ~= 'Unknown' then
		mode = 'SD'
	end
	return mode
end

function getLogo( str )
	if (str == "") then
		return "INFO_logoCanal.png"
	end
	return str
end

local function getShowWidth( extended )
	return extended and 175 or 213
end

local container_separation = 20

function createCommonHeader( infobox, extended )
	local win = infobox:getWgt( 'win' )

	local dateLabel_x = 350
	local dateLabel_y = 8
	local dateLabel_w = 45
	local fontSize = 14

	infobox.bars = infobox.bars or {}
	table.insert(infobox.bars, image:new(0, 26, 620, 4, widgetZIndex, labelsColorTable, "infobox_line_long.png"))
	table.insert(infobox.bars, image:new(66, 96, 552, 4, widgetZIndex, labelsColorTable, "infobox_line_two.png"))

	infobox.date_layout = layout.new(win, 350, 2, 5)
	infobox.dateValue = infobox.date_layout:add_field_h("Fecha:", 41, 90, 5)
	infobox.timeValue = infobox.date_layout:add_field_h("Hora:", 33, 54, 5)

	infobox.logo = image:new( 5, 34, 53, 53, widgetZIndex, labelsColorTable, "INFO_logoCanal.png")
	infobox.serviceLabel = label:new( 72, 41, 200, 15, menuColorTable, "", fontSize, Alignment.vCenter_hLeft )

	infobox.icons = {}
	infobox.icons['bloq'] = image:new( dateLabel_x + 60, 35, 54, 56, widgetZIndex, labelsColorTable, "bloq.png")
	infobox.icons['fav'] = image:new( dateLabel_x + 130, 35, 54, 56, widgetZIndex, labelsColorTable, "fav.png")
	infobox.icons['genre'] = image:new( dateLabel_x + 200, 35, 108, 56, widgetZIndex, labelsColorTable, "content_01.png")
	wgt:setVisible( infobox.icons['bloq'], FALSE )
	wgt:setVisible( infobox.icons['fav'], FALSE )
	wgt:setVisible( infobox.icons['genre'], FALSE )

	local first_layout = layout.new(win, init_x, init_y)

	infobox.showLabel = first_layout:add_field("PROGRAMA", getShowWidth(extended), 1)
	infobox.beginLabel = first_layout:add_field("INICIA", label_w)
	infobox.endLabel = first_layout:add_field("TERMINA", label_w)

	local second_layout = nil
	if not extended then
		second_layout = layout.new(win, first_layout:x(), init_y)
	else
		second_layout = layout.new(win, init_x, first_layout:y() + container_separation)
	end

	infobox.languageLabel = second_layout:add_field("AUDIO", label_w)
	infobox.ageLabel = second_layout:add_field("EDAD", 90)

	infobox.second_layout = second_layout
	infobox.first_layout = first_layout

	for i, b in ipairs(infobox.bars) do
		window:addChild(win, b)
	end

	window:addChild( win, infobox.logo )
	window:addChild( win, infobox.serviceLabel )
	window:addChild( win, infobox.icons['bloq'] )
	window:addChild( win, infobox.icons['fav'] )
	window:addChild( win, infobox.icons['genre'] )

end

function createDescription( infobox, width, lines )
	local win = infobox:getWgt( 'win' )
	local container = layout.new(win, init_x, infobox.second_layout:y() + container_separation)
	local opts = {
		content = {alignment = Alignment.vTop_hLeft}
	}
	infobox.desc = container:add_field("DESCRIPCIÓN", width, lines, opts)
end

function createExtended( infobox, extended )
	local win = window:new(x, y, infobox.w, infobox.h, widgetZIndex, menuColorTable)

	infobox:addWgt('win', win)
	wgt:setVisible(win, FALSE)

	infobox.bars = {}

	if extended then
		table.insert(infobox.bars, image:new(0, 372, 620, 4, widgetZIndex, labelsColorTable, "infobox_line_long.png"))
		table.insert(infobox.bars, image:new(419, 99, 3, 273, widgetZIndex, labelsColorTable, "infobox_line_vertical.png"))
		table.insert(infobox.bars, image:new(420, 230, 198, 4, widgetZIndex, labelsColorTable, "infobox_line_short.png"))
	else
		table.insert(infobox.bars, image:new(0, 255, 620, 4, widgetZIndex, labelsColorTable, "infobox_line_long.png"))
	end

	createCommonHeader(infobox, extended)

	infobox.hdLabel = label:new( init_x, 69, 32, 20, menuColorTable, "", fontSize, Alignment.vCenter_hCenter )
	infobox.audioLabel = label:new( init_x + 30, 69, 93, 20, menuColorTable, "", fontSize, Alignment.vCenter_hCenter )

	infobox.barWin = window:new( init_x + getShowWidth(extended), infobox.first_layout:y(), 148, 5, widgetZIndex, progressBarColorTable )
	infobox.bar = bar:new( 0, 0, 148, 5, progressBarColorTable, -1, 0, 100)
	wgt:setVisible(infobox.barWin, FALSE)
	window:addChild( infobox.barWin, infobox.bar )

	window:addChild( win, infobox.hdLabel )
	window:addChild( win, infobox.audioLabel )
	window:addChild( win, infobox.barWin )
end

local function position_icon( icon, x, y )
	local icon_separation = 10
	local w, _ = wgt:size(icon)
	x = x - w
	wgt:position( icon, x, y )
	return x - icon_separation
end

function refreshCommonData( infobox, info )
	label:setText( infobox.dateValue, mainWindow.getTime('%d/%m/%Y'))
	label:setText( infobox.timeValue, mainWindow.getTime('%H:%Mhs'))
	label:setText( infobox.serviceLabel, info['name'] .. ' - ' .. info['channel'])
	image:setImage( infobox.logo, getLogo(info.logo) )

	textarea:setText( infobox.showLabel, getText( info.showName ) )
	label:setText( infobox.beginLabel, getText( info.startTime, 'hs' ) )
	label:setText( infobox.endLabel, getText( info.endTime, 'hs' ) )
	label:setText( infobox.languageLabel, translate( channel.getAudioInfo(-1) ) )
	label:setText( infobox.ageLabel, getText( info.parentalAge ) )

	local init_x = x + infobox.w - 10
	local init_y = y + 35

	image:setImage( infobox.icons['genre'], categories.getIcon(info.content).normal )
	init_x = position_icon(infobox.icons['genre'], init_x, init_y)
	wgt:setVisible( infobox.icons['genre'], TRUE )

	if info.blocked == TRUE then
		init_x = position_icon(infobox.icons['bloq'], init_x, init_y)
	end

	if info.favorite == TRUE then
		init_x = position_icon(infobox.icons['fav'], init_x, init_y)
	end

	wgt:setVisible( infobox.icons['fav'], info.favorite or FALSE and TRUE )
	wgt:setVisible( infobox.icons['bloq'], info.blocked or FALSE and TRUE )

	for _, b in ipairs(infobox.bars) do
		wgt:setVisible(b, TRUE)
	end
end

function refreshCommonExtended( infobox, info )
	local tmp = getVideoType()
	label:setText( infobox.hdLabel, tmp )
	wgt:setVisible( infobox.hdLabel, tmp ~= 'Unknown' and TRUE or FALSE )
	tmp = channel:getAudio()
	label:setText( infobox.audioLabel, tmp )
	wgt:setVisible( infobox.audioLabel, tmp ~= 'Unknown' and TRUE or FALSE )

	textarea:setText(infobox.desc, getText(info.showDescription))

	if info.percentage > -1 then
		wgt:setVisible(infobox.barWin, TRUE)
		bar:setValue(infobox.bar, info.percentage)
	else
		wgt:setVisible(infobox.barWin, FALSE)
	end
end

local function get_current_infobox()
	local infobox = nil

	if Info_Zapping.visible then
		infobox = Info_Zapping
	elseif Info_Standard.visible then
		infobox = Info_Standard
	elseif Info_Extended.visible then
		infobox = Info_Extended
	end

	return infobox
end

function common_interval(info)
	return timer.newInterval(function () info.refreshData(channel.current()) end, 5 * timer.SECOND)
end

function close_current_infobox()
	local infobox = get_current_infobox()
	if infobox then
		closeWgt(infobox)
	end
end
