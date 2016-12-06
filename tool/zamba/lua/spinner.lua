local log = require 'log'
local timer = require 'timer'
require "basewidget"

local M = {} -- Module API

local Sprite = {}
local Spinner = BaseWidget:new( {}, "Spinner", false)

-- By default spin at 30 fps
local DEFAULT_INTERVAL = 33

function M.newSprite( src, frames, width, height, fps )
	obj = {
		index = 0,
		frames = frames,
		surface = canvas.createSurfaceFromPath(src),
		width = width,
		height = height,
		interval = DEFAULT_INTERVAL
	}

	if fps ~= nil then
		obj.interval = math.floor(1000 / fps + 0.5)
	end
	setmetatable(obj, {__index = Sprite})

	return obj
end

function M.newSpinner( name, x, y, sprite, onRenderCb )
	local obj = {
		sprite = sprite,
		surface = canvas.createSurface(x, y, sprite.width, sprite.height),
		running = false,
		onRender = onRenderCb,
		timerId = -1
	}

	setmetatable(obj, {__index = Spinner})

	return obj
end

function Sprite:next()
	self.index = (self.index+1) == self.frames and 0 or self.index + 1
end

function Sprite:draw( surface )
	surface:clear()
	surface:blit(0, 0, self.surface, self.index * self.width, 0, self.width, self.height)
end

function Spinner:open()
	log.debug("Spinner", "open", self.name)

	self.running = true
	self:registerTimer()
end

function Spinner:close()
	log.debug("Spinner", "close", self.name)
	if self.timerId ~= -1 then
		timer.cancel( self.timerId )
		self.timerId = -1
	end
	self.running = false
	self.surface:clear()
end


function Spinner:refresh()
	log.debug("Spinner", "refresh", "running=" .. tostring(self.running))

	if self.running then
		self.sprite:draw(self.surface)
		self.sprite:next()
		self.onRender( self.surface )
		self:registerTimer()
	end
end

function Spinner:registerTimer()
	self.timerId = timer.register(function() self:refresh() end, self.sprite.interval)
end

return M
