local log = require 'log'
local l_util = require 'l_util'
local timer = require 'timer'
local banish = require 'banish_mixin'
local Widget = require "basewidget"

local M = {} -- Module API

local BAR_MARGIN = 6

local DownloadBar = {}
setmetatable( DownloadBar, Widget.mt ) -- Inherits behaviour from Widget
DownloadBar_mt = { __index = DownloadBar }

DownloadBar:include(banish) -- Add banish behaviour to the table

function M.new( name, label_text, width, zindex )
	local inst = Widget.new(name, false)
	setmetatable(inst, DownloadBar_mt) -- Add DownloadBar behaviour to the instance

	local canvas_w, canvas_h = canvas.size()
	local pb = progressbar.new( ( canvas_w / 2 ) - ( width / 2 ), 450, width, 24, BAR_MARGIN, zindex, barColorTable, label_text)

	wgt:setVisible(pb, FALSE)
	inst:addWgt('bar', pb)

	inst:banishOn(3000)

	return inst
end

function DownloadBar:updateValue( value )
	log.debug("DownloadBar", "updateValue", "value=" .. tostring(value))

	local bar = self:getWgt('bar')

	local v = l_util.math.round(value)
	progressbar.setValue(bar, v, tostring(v) .. "%")
	self:banish_restart()

	wgt:invalidate(bar)
end

function DownloadBar:onShow()
	log.debug("DownloadBar", "onShow")
	progressbar.setValue(self:getWgt('bar'), 0, tostring(0) .. "%")
	return true
end

function DownloadBar:onHide()
	log.debug("DownloadBar", "onHide")
	wgt:needsClear(self:getWgt('bar'), true)
	return true
end

return M
