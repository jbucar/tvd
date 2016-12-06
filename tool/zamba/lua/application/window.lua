local log = require 'log'
local l_util = require 'l_util'
local app_surface = require 'application.surface'
local Widget = require "basewidget"
local Alert = require "widgets.alert"

AppWindow = {}
setmetatable( AppWindow, Widget.mt ) -- Inherits behaviour from Widget
AppWindow_mt = { __index = AppWindow }

local surface = nil

local function getSurface()
	if not surface then
		local s = canvas.createSurface(0, 0, canvas.size())
		s:setZIndex(canvas.zapperZIndex() + widgetZIndex)
		s:autoFlush(true)
		s:setVisible(false)
		surface = app_surface.create(s)
	end

	return surface
end

function AppWindow:configure_listeners()
	self.listeners = {
		onShow = {},
		onHide = {},
		onExit = {},
		onKey  = {},
	}
end

function AppWindow:new()
	local inst = Widget.new("Zamba App Window", true, true)
	setmetatable(inst, AppWindow_mt) -- Add AppWindow behaviour to the instance

	inst._alert = Alert.new()

	inst:configure_listeners()

	return inst
end

function AppWindow:getSurface()
	return getSurface()
end

function AppWindow:onShow()
	log.trace("AppWindow", "onShow", "window=" .. self.name)
	appEnabled(TRUE)

	app_surface.reset(getSurface())
	getSurface():clear()
	getSurface():setVisible(true)

	self:dispatch_event('onShow')

	return true
end

function AppWindow:onHide()
	log.trace("AppWindow::Window", "onHide", "window=" .. self.name)
	
	self:dispatch_event('onHide')

	getSurface():setVisible(false)

	appEnabled(FALSE)
	restoreMenu()
	return true
end

function AppWindow:onClosed()
	BaseWidget.onClosed(self)
end

function AppWindow:listen( evt, callback )
	log.trace("AppWindow", "listen", "add listener, window=" .. self.name .. " evt=" .. evt)
	table.insert(self.listeners[evt], callback)
end

function AppWindow:processKeys( key )
	log.trace("AppWindow", "processKeys", "key=" .. tostring(key))

	if key == KEY_EXIT or key == KEY_ESCAPE then
		return false
	else
		return self:dispatch_event('onKey', key)
	end
end

function AppWindow:warn( title, text )
	self._alert:title(title)
	self._alert:text(text)
	openWgt(self._alert)
end

----------------------------------------------------------------------------------
-- Internal Functions (private)
----------------------------------------------------------------------------------

function AppWindow:dispatch_event( evt, ... )
	log.trace("AppWindow", "dispatch_event", "evt=" .. tostring(evt) .. " #listeners=" .. tostring(#self.listeners[evt]))
	local result = false
	
	for _, callback in ipairs(self.listeners[evt]) do
		result = callback(self, ...)
	end
	return result
end

return AppWindow
