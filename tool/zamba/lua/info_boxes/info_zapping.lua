require "info_boxes.common_info"
require "info_boxes.info_standard"
require "info_boxes.info_extended"
local banish = require "banish_mixin"
local log = require "log"

require "basewidget"

Info_Zapping = {
	w = 620,
	h = 160
}
local TIMEOUT = 4000
local seeing = nil
local update = common_interval(Info_Zapping)

function Info_Zapping.create()
	Info_Zapping = BaseWidget:new(Info_Zapping, "Info_Zapping", false, true)
	Info_Zapping:include(banish)
	
	local win = window:new( 50, 50, Info_Zapping.w, Info_Zapping.h, widgetZIndex, menuColorTable )
	Info_Zapping:addWgt('win', win)
	wgt:setVisible(win, FALSE)
	Info_Zapping:banishOn(TIMEOUT)

	createCommonHeader(Info_Zapping, false)

	wgt:fixOffsets(win)
end

function Info_Zapping.change( ch, virtual )
	seeing = ch
	refreshCommonData(Info_Zapping, channels.getInfo(ch))
	Info_Zapping:bypass_escape(not virtual)
end

function Info_Zapping.refreshData( ch )
	log.debug("Info_Zapping", "refreshData")
	if seeing == ch then
		refreshCommonData(Info_Zapping, channels.getInfo(ch))
	end
end

function Info_Zapping:onShow()
	update:start()
	return true
end

function Info_Zapping:onHide()
	update:stop()
	seeing = nil
	Info_Zapping:bypass_escape(false)
	resetVirtualChannel()
	ignoreChKeys(false)
	return true
end

function Info_Zapping:processKeys(key)
	return false
end

function Info_Zapping.onInfo()
	closeWgt(Info_Zapping)
	return true
end
