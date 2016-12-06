local _M = {}

-- TODO: Move this to a Model object
local observers = {}

local function register_aspect( aspect )
	observers[aspect] = {}
end

local function notify( aspect, new_value )
	if observers[aspect] then
		for _, callback in pairs(observers[aspect]) do
			callback(new_value)
		end
	end
end

local function notify( aspect, new_value1, new_value2 )
	if observers[aspect] then
		for _, callback in pairs(observers[aspect]) do
			callback(new_value1,new_value2)
		end
	end
end

function _M.observe( aspect, callback )
	table.insert(observers[aspect], callback)
end

register_aspect('beginScan')
register_aspect('newChannelFound')
register_aspect('scanNetwork')
register_aspect('endScan')

function zapperBeginScan()
	notify('beginScan')
	view.refresh()
end

function zapperScanNetwork( net, perc )
	notify('scanNetwork', net, perc )
	view.refresh()
end

function zapperEndScan()
	notify('endScan')
	view.refresh()
end

function zapperNewChannelFound( ch )
	notify('newChannelFound', ch)
	view.refresh()
end

return _M
