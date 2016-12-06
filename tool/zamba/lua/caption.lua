local log = require 'log'

function toggleCaption()
	local next = channel.nextSubtitle()
	log.info('zamba', 'toggleCaption: ' .. tostring(next))
end

function closeCC()
--	channel.disableSubtitle()
end
