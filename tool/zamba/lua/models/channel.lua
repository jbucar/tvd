local _M = {}

-- TODO: Move this to a Model object
local observers = {}

local function register_aspect( aspect )
	observers[aspect] = {}
end

local function notify( aspect, ... )
	if observers[aspect] then
		for _, callback in pairs(observers[aspect]) do
			callback(...)
		end
	end
end

function _M.observe( aspect, callback )
	table.insert(observers[aspect], callback)
end

register_aspect("change")

function _M.change( ix )
	notify('change', ix)
end

return _M
