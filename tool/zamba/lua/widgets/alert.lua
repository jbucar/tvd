local log = require 'log'
local l_util = require 'l_util'
local Widget = require "basewidget"
local banish = require 'banish_mixin'

require "zindex"
require "colors"

local BORDER = 4

-- Alerts are by default centered in the menu
local size_w = 385
local size_h = 165
local pos_x = 218
local pos_y = 157

local Alert = {}
setmetatable(Alert, Widget.mt) -- Inherits behaviour from Widget
Alert_mt = { __index = Alert }
Alert:include(banish) -- Add banish behaviour to the table

function Alert.new( x, y, w, h, border )
	local self = Widget.new("Alert", true, true)
	setmetatable(self, Alert_mt) -- Add Dialog behaviour to the instance

	local dlg = basicdialog:new(x or pos_x, y or pos_y, w or size_w, h or size_h, alertColorTable, dialogZIndex, border or BORDER)
	self:addWgt("dlg", dlg)

	self:title("ADVERTENCIA")
	self:icon("Confirmacion.png", 70, 61, "Confirmacion_Gris.png")

	self:propagateEvts(false)
	self:banishOn(4000)

	return self
end

function Alert:title( title )
	basicdialog:title(self:getWgt("dlg"), title)
end

function Alert:text( text, lines )
	basicdialog:text(self:getWgt("dlg"), text, lines or 4)
end

function Alert:icon( icon, w, h, selected_icon )
	basicdialog:icon(self:getWgt("dlg"), icon, w, h, selected_icon)
end

function Alert:onHide()
	if self._callback then
		self._callback()
	end
	return true
end

function Alert:processKeys( key )
end

function Alert:callback( fnc )
	self._callback = fnc
end

return Alert
