local log = require 'log'
local l_util = require 'l_util'
local Widget = require "basewidget"
local config = require "menu.config_page"

require "zindex"
require "colors"

local BORDER = 4

-- Confirmation are by default centered in the menu
local size_w = 400
local size_h = 200
local pos_x, pos_y = config.center(size_w, size_h)

local Confirmation = {}
setmetatable(Confirmation, Widget.mt) -- Inherits behaviour from Widget
Confirmation_mt = { __index = Confirmation }

function Confirmation.new( x, y, w, h, border )
	local self = Widget.new("Confirmation", true, true)
	setmetatable(self, Confirmation_mt) -- Add Dialog behaviour to the instance

	local dlg = yesnodialog:new(x or pos_x, y or pos_y, w or size_w, h or size_h, alertColorTable, dialogZIndex, border or BORDER, 75, 36)
	self:addWgt("dlg", dlg)

	self:title("ADVERTENCIA")
	self:icon("Confirmacion.png", 70, 61, "Confirmacion_Gris.png")

	yesnodialog:noAction(dlg, function() closeWgt(self) end)
	yesnodialog:yesAction(dlg, function() closeWgt(self) end)

	self:propagateEvts(false)

	return self
end

function Confirmation:title( title )
	yesnodialog:title(self:getWgt("dlg"), title)
end

function Confirmation:text( text, lines )
	yesnodialog:text(self:getWgt("dlg"), text, lines or 4)
end

function Confirmation:onYes( callback )
	yesnodialog:yesAction(self:getWgt("dlg"), function()
		log.debug("Confirmation", "onYes")
		closeWgt(self)
		l_util.ensure_call(callback)
	end)
end

function Confirmation:onNo( callback )
	yesnodialog:noAction(self:getWgt("dlg"), function()
		log.debug("Confirmation", "onNo")
		closeWgt(self)
		l_util.ensure_call(callback)
	end)
end

function Confirmation:icon( icon, w, h, selected_icon )
	yesnodialog:icon(self:getWgt("dlg"), icon, w, h, selected_icon)
end

function Confirmation:onHide( callback )
	if callback then
		self._callback = callback
	elseif self._callback then
		self._callback()
	end

	return true
end

function Confirmation:processKeys( key )
	log.debug("Confirmation", "processKeys", tostring(key))
	return wgt:processKey(self:getWgt("dlg"), key)
end

return Confirmation
