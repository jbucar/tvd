local log = require 'log'
local banish = require 'banish_mixin'

local W = 240
local H =  30
local X = 720/2 - W/2
local Y = 410
local BORDER_W = 3
local FONT_SIZE = 15

local toogleColorTable = {
	bgColor = Color.Gray4,
	borderColor = Color.Gray3,
	textColor = Color.White,
}

local toggleLabel = label:new(0, 0, W, H, toogleColorTable, "", FONT_SIZE, Alignment.vCenter_hCenter)
local videoToggle = window:new(X, Y, W, H, widgetZIndex, toogleColorTable, BORDER_W)
window:addChild(videoToggle, toggleLabel)
wgt:fixOffsets(videoToggle)
wgt:setVisible(videoToggle, FALSE)

-- Video modes
function toggleVideoMode()
	local content, selected = display.listVideoModes(general.WorkingTVOut)
	if general.WorkingTVOut == general.CompositeVideoIndex then
		content = general.CompositeVideoContents
	end
	if (ComboToggle.visible) then
		selected = (selected + 1) % #content
		if display.setVideoMode( general.WorkingTVOut, selected ) == FALSE then
			log.warn("toggles", "toggleVideoMode", "Error setting video mode")
		end
	end
	ComboToggle.text = content[selected+1]
	ComboToggle.updateValue()
end

-- Aspect modes
function toggleAspectMode()
	content,selected = display.getAspectModes()
	if (ComboToggle.visible) then
		selected = (selected + 1) % #content
		display.setAspectMode( selected )
	end

	ComboToggle.text = content[selected+1]
	ComboToggle.updateValue()
end

-- Audio
function toggleAudio()
	channel.nextAudio()
	audio = channel.getAudioInfo(-1)
	ComboToggle.text = audio
	ComboToggle.updateValue()
end

-----------------------------------------------------------------------------------
ComboToggle = {
	text = ""
}

ComboToggle = BaseWidget:new( ComboToggle, "ComboToggle", false)
ComboToggle.wgt.win = videoToggle

function ComboToggle.updateValue()
	if not ComboToggle.visible then
		openWgt( ComboToggle )
	else
		ComboToggle:banish_restart()
	end
	label:setText(toggleLabel, ComboToggle.text)
end

-- Add banish behaviour
ComboToggle:include(banish)
ComboToggle:banishOn(banish_timeout)
