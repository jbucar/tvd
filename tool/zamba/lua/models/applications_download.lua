local log = require 'log'

local _M = {}

-- TODO: Move this to a Model object
local observers = {}

local function register_aspect( aspect )
	observers[aspect] = {}
end

local function notify( aspect, ... )
	local result = false
	if observers[aspect] then
		for _, callback in pairs(observers[aspect]) do
			local cres = callback(...)
			result = result or cres
		end
	end
	return result
end

function _M.observe( aspect, callback )
	table.insert(observers[aspect], callback)
end

register_aspect("progress")

function zapperApplicationDownloadProgress( step, total )
	if notify('progress', step, total) then
		log.debug("zapperApplicationDownloadProgress", "about to do a refresh")
		view.refresh()
	end
end

return _M
