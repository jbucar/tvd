local log = require 'log'

GingaWindow = {}

function GingaWindow:new()
	local obj = {
		wgt = {}
	}

	self = BaseWidget:new( self, "Ginga Application Window", false, false)
	setmetatable(obj, {__index = self})

	return obj
end

function GingaWindow:processKeys( key )
	return false
end

function GingaWindow:onHide( key )
	restoreMenu()
	return true
end

GingaWindow:new()

return GingaWindow