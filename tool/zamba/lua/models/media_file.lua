local _M = {}

-- TODO: Move this to a Model object
local observers = {}

function register_aspect(aspect)
	observers[aspect] = {}
end

function notify(aspect, new_value)
	if observers[aspect] then
		for _, callback in pairs(observers[aspect]) do
			callback(new_value)
		end
	end
end

function _M.observe(aspect, callback)
	table.insert(observers[aspect], callback)
end

register_aspect("file_end")
register_aspect("mount_point_changed")

function mediaFileEnd()
	notify('file_end')
	view.refresh()
end

function zapperMountPointChanged(mountPoint, isMount)
	notify('mount_point_changed', mountPoint, isMount)
	view.refresh()
end

return _M