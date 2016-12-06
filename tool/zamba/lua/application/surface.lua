local proxy = require 'proxy'

local M = {}

function M.create( surface )
	local sp = proxy.createProxy(surface)
	sp.deny("autoFlush")

	function sp:destroy()
		canvas.destroy(self.__obj)
	end

	M.reset(sp)
	return sp
end

function M.reset( surface )
	surface:setColor(0,0,0,255)
	surface:setFont("Tiresias", 10)
end

return M
