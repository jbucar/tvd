local _M = {}

-- TODO: Move this to a Model object
local observers = {}

local function register_aspect( aspect )
	observers[aspect] = {}
end

local function notify( aspect, new_value )
	for _, callback in pairs(observers[aspect]) do
		callback(new_value)
	end
end

function _M.observe( aspect, callback )
	table.insert(observers[aspect], callback)
end

register_aspect("up")
register_aspect("down")
register_aspect("mute")
register_aspect("unmute")

function _M.value()
	return mixer.getVolume()
end

function _M.maxValue()
	return mixer.maxVolume()
end

function _M.down()
	mixer.volumeDown()
	notify('down', mixer.getVolume())
end

function _M.up()
	mixer.volumeUp()
	notify('up', mixer.getVolume())
end

function _M.toggleMute()
	local res = mixer.toggleMute() == TRUE

	if res then
		notify('mute')
	else
		notify('unmute')
	end

	return res
end

function _M.unmute()
	mixer.mute(false)
	notify('unmute')
end

function _M.mute()
	mixer.mute(true)
	notify('mute')
end

return _M
