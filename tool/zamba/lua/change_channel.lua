local Widget = require "basewidget"
local banish = require 'banish_mixin'
require "colors"

local BORDER_W = 4
local x, y, w, h = 549, 57, 100, 50
local color_table = {
	bgLabelColor = Color.Gray4,
	textColor = Color.White
}

local win = window:new(x, y, w, h, widgetZIndex, menuColorTable, BORDER_W)
local masked_label = label:newMasked(x + BORDER_W, y + BORDER_W, w - 2 * BORDER_W, h - 2 * BORDER_W, color_table, "", 28, Alignment.vCenter_hCenter)
window:addChild(win, masked_label)
wgt:setVisible(win, FALSE)

local widget = Widget.new("ChannelByDigitNav", false, true)
setmetatable(widget, Widget.mt ) -- Inherits behaviour from Widget

widget:addWgt('window', win)

function widget:onHide()
	label:reset(masked_label)
	return true
end

function widget:processKeys(key)
	local res = wgt:processKey(self.wgt.window, key)
	if res then
		self:banish_restart()
	end

	return res
end

function widget:onDown()
	closeWgt(widget)
	return false
end

function widget:onUp()
	closeWgt(widget)
	return false
end

function widget:onOk()
	local str = label:getRawText(masked_label)
	closeWgt(widget)
	-- don't change to the same channel
	local cur = channel.current()
	if cur ~= -1 and channels.getInfo(cur).channel ~= str then
		local channels = channels.getAll()
		for i, ch in pairs(channels) do
			if ch.channel == str then
				changeChannel(ch.channelID, false)
			end
		end
	end
	checkUpdates()

	return true
end

-- Add banish behaviour to the widget
widget:include(banish)
widget:banishOn(5000)

widget:ignore({
	'onInfo',
	'onEPG',
	'onRight',
	'onLeft',
	'onMute',
	'onVolumeUp',
	'onVolumeDown',
	'onMenu',
	'onAudio',
	'onRecall',
	'onFav',
	'onSub',
	'onAspect',
	'onChannel',
	'onTVmode',
})

return widget
