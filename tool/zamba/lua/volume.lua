local banish = require 'banish_mixin'
local model = require 'models.volume'

require "basewidget"
require "colors"
require "zindex"

local BAR_MARGIN = 6

local widget = BaseWidget:new({}, "Volume", false)

local pb = progressbar.new(180, 412, 360, 24, BAR_MARGIN, dialogZIndex, barColorTable, "Volumen")
progressbar.setMaxValue(pb, model.maxValue())

wgt:setVisible(pb, 0) -- Not visible
widget:addWgt('pb', pb)

widget:close_on_all_events_except({'onVolUp', 'onVolDown'})

local function setProgressBar(vol)
	progressbar.setValue(widget:getWgt('pb'), vol, tostring(vol))
end

local function updateValue(vol)
	openWgt(widget)
	widget:banish_restart() -- In case that the widget was already open
	setProgressBar(vol)
end

function widget.onShow()
	setProgressBar(mixer.getVolume())
	return true
end

-- Observe model
model.observe('down', updateValue)
model.observe('up', updateValue)
model.observe('unmute', function() openWgt(widget) end)
model.observe('mute', function() closeWgt(widget) end)

-- Add banish behaviour
widget:include(banish)
widget:banishOn(banish_timeout)

return widget
