local timer = require 'timer'
local log = require 'log'

local M = {}

function M:mixin( class )

	function class:banishOn( ms )
		self._banish_timer = nil
		self._ms = ms
		log.trace( "BanishMixin", "banishOn", "widget = " .. tostring(self.name) )

		self.wrappedOnShow = self.onShow
		function self:onShow()
			log.trace( "BanishMixin", "onShow", "widget = " .. tostring(self.name) )
			self:banish_restart()
			if self.wrappedOnShow then
				return self:wrappedOnShow()
			end
			return true
		end

		self.wrappedOnHide = self.onHide
		function self:onHide()
			log.trace( "BanishMixin", "onHide", "widget = " .. tostring(self.name) )
			timer.cancel(self._banish_timer)
			if self.wrappedOnHide then
				return self:wrappedOnHide()
			end
			return true
		end
	end

	function class:banish_restart()
		log.trace( "BanishMixin", "banish_restart", "widget = " .. tostring(self.name) )
		timer.cancel(self._banish_timer)
		self._banish_timer = timer.register(function() closeWgt(self) end, self._ms)
	end

end

return M
