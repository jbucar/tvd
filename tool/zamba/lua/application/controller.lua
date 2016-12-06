local log = require 'log'
local channel = require 'models.channel'

-- Applications running count
local count = 0

-- Application launching right now ?
local launching = false

local _M = {}

channel.observe('change', _M.stop_launch)

function _M.start_run()
	count = count + 1
	_M.stop_launch()
end

function _M.stop_run()
	count = count - 1
end

function _M.start_launch()
	launching = true
end

function _M.stop_launch()
	launching = false
end

function _M.count()
	return count
end

function _M.running_app()
	return count > 0
end

function _M.app_actived()
	log.debug( "AppCtrol", "app_actived", string.format("launching=%s, running=%i", launching  and "true" or "false", count ))
	return launching or _M.running_app()
end

return _M
