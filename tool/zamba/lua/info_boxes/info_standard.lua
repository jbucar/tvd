require "info_boxes.common_info"
local log = require "log"
require "basewidget"

Info_Standard = {
		w = 620,
		h = 283
}

local x = 50
local y = 50
local label_h = 26
local fontSize = 14

local progressBarColorTable = {
	bgColor = Color.White,
	bgProgressBarColor = Color.White,
	bgBarColor = Color.Orange,
	textColor = Color.Black,
}

local update = common_interval(Info_Standard)

function Info_Standard.create()
	Info_Standard = BaseWidget:new( Info_Standard, "Info_Standard", true, true )
	createExtended(Info_Standard, false)
	createDescription(Info_Standard, 544, 3)
	
	local win = Info_Standard:getWgt( 'win' )
	Info_Standard.createHelp( win )

	wgt:fixOffsets( win )

	Info_Standard:ignore(info_ignored_actions)
end

function Info_Standard.createHelp(win)
	local exitHeaderLabel = label:new( 246, 261, 90, 24, labelsColorTable, "Botón \'INFO\' =", fontSize, Alignment.vCenter_hLeft )
	local okHeaderLabel = label:new( 405, 261, 87, 24, labelsColorTable, "Botón \'OK\' =", fontSize, Alignment.vCenter_hLeft )
	local exitLabel = label:new( 344, 261, 30, 24, menuColorTable, "Salir", fontSize, Alignment.vCenter_hLeft )
	local okLabel = label:new( 488, 261, 121, 24, menuColorTable, "Mas Información...", fontSize, Alignment.vCenter_hLeft )
	local separator = label:new( 385, 261, 121, 24, dateColorTable, "|", fontSize, Alignment.vCenter_hLeft )
	window:addChild( win, exitHeaderLabel )
	window:addChild( win, okHeaderLabel )
	window:addChild( win, exitLabel )
	window:addChild( win, okLabel )
	window:addChild( win, separator )
end

function Info_Standard.onShow()
	log.debug("Info_Standard", "onShow")

	local ch = channel.current()
	if ch ~= -1 then
		Info_Standard.refreshData(ch)
		update:start()
		return true
	else
		return false
	end
end

function Info_Standard.onHide()
	log.debug("Info_Standard", "onHide")
	update:stop()
	return true
end

function Info_Standard.refreshData(ch)
	local info = channels.getInfo(ch)
	refreshCommonData( Info_Standard, info )
	refreshCommonExtended( Info_Standard, info )
	view:refresh()
end

function Info_Standard:processKeys(key)
	return false
end

function Info_Standard.onInfo()
	closeWgt(Info_Standard)
	return true
end

function Info_Standard.onOk()
	closeWgt( Info_Standard )
	openWgt( Info_Extended )
	return true
end
