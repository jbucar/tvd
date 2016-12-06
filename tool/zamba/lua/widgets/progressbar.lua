local log = require 'log'
local l_util = require 'l_util'
require "basewidget"

local M = {}

M.ProgressBar = BaseWidget:new( {}, "ProgressBar", false, false)

-- Views
M.views = {}
M.views.Vertical = {}

-- Aux

local BarView = {}

function BarView.new( w, h )
	local obj = {
		_w = w,
		_h = h,
		_surface = canvas.createSurface(0, 0, w, h)
	}

	obj._surface:setColor(255, 255, 255, 255)
	obj._surface:fillRect(0, 0, w, h)

	setmetatable(obj, {__index = BarView})

	return obj
end

function BarView:draw( surface, x, y )
	surface:blit( x, y, self._surface )
end

function BarView:height()
	return self._h
end

-- Views Impl

function M.views.Vertical.new( x, y, w, h, bar_h, margin )
	local obj = {
		_x = x,
		_y = y,
		_w = w,
		_h = h,
		_bar = BarView.new(w, bar_h),
		_margin = margin,
		_surface = canvas.createSurface(x, y, w, h),
	}

	obj._surface:autoFlush(true)
	local zIndex = canvas:zapperZIndex()
	obj._surface:setZIndex(zIndex + widgetZIndex)

	setmetatable(obj, {__index = M.views.Vertical})

	return obj
end

function M.views.Vertical:draw( percentage )
	local y = self._h
	local offset = self._bar:height() + self._margin
	local to = l_util.math.round(self._h / offset * percentage)

	self._surface:clear()

	for i = 0, to do
		self._bar:draw( self._surface, 0, y )
		y =  y - offset
	end
end

function M.views.Vertical:visible( value )
	self._surface:setVisible(value)
end

-- Model/Controller

function M.new( view )
	local obj = {
		_value = 0,
		_view = view
	}

	setmetatable(obj, {__index = M.ProgressBar})
	return obj
end

function M.ProgressBar:value( value )
	if value then
		if value >= 0 and value <= 1 then
			self._value = value
			self._view:draw(value)
		else
			log.error("VerticalProgressBar", "value must be between 0 and 1")
		end
	end
	return self._value
end

function M.ProgressBar:onShow()
	log.debug("VerticalProgressBar", "onShow")
	self._view:visible(true)
	return true
end

function M.ProgressBar:onHide()
	log.debug("VerticalProgressBar", "onHide")
	self._view:visible(false)
	return true
end

return M
