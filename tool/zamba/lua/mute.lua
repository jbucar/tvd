local model = require 'models.volume'
local banish = require 'banish_mixin'
local log = require 'log'

require "basewidget"
require "colors"
require "zindex"

local widget = BaseWidget:new({}, "Mute", false)

local img = image:new(40, 40, 47, 38, dialogZIndex, menuColorTable, "Mute.png")
wgt:setVisible(img, 0) -- Not visible
widget:addWgt('img', img)

widget:close_on_all_events_except({'onMute'})

-- Observe volume change
model.observe('down', function() closeWgt(widget) end)
model.observe('up', function() closeWgt(widget) end)
model.observe('mute', function() openWgt(widget) end)
model.observe('unmute', function() closeWgt(widget) end)

-- Add banish behaviour
widget:include(banish)
widget:banishOn(banish_timeout)

function widget.checkInitStatus()
	if mixer.isMuted() == TRUE then
		model.mute()
	end
end

return widget
