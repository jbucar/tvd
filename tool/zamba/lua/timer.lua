
local log = require 'log'

local TIMEOUT = 5000

local callbacks_map = {}

local M = {
	timeout = nil,
}

-- Time consts
M.SECOND = 1000
M.MINUTE = 60 * M.SECOND
M.HOUR = 60 * M.MINUTE

local function delete_callback( tid )
	callbacks_map[tid] = nil
end

local function gettimeout( ms )
	return ms or M.timeout or TIMEOUT
end

function M.register( callback, milliseconds )
	log.trace("Timer", "register", "ms = " .. tostring(gettimeout(milliseconds)))

	local tid = mainWindow.registerTimer(gettimeout(milliseconds))

	log.trace("Timer", "register", "using tid= " .. tostring(tid))

	callbacks_map[tid] = callback
	return tid
end

function M.cancel( tid )
	log.trace("Timer", "cancel", "tid = " .. tostring(tid))

	if tid then
		mainWindow.cancelTimer(tid)
		delete_callback(tid)
	end
end

function M.dispatch( tid )
	log.debug("Timer", "dispatch", "tid = " .. tostring(tid))
	if callbacks_map[tid] then
		callbacks_map[tid](tid)
		delete_callback(tid)
	else
		log.debug("Timer", "No callback found")
	end
end

-- Sets de default timeout in milliseconds
function M.settimeout( milliseconds )
	M.timeout = milliseconds
end

-- Intervals

local Interval = {}

function M.newInterval( callback, ms )
	log.trace("Timer", "newInterval", "callback=" .. tostring(callback) .. " ms=" .. tostring(ms))
	obj = { 
		timer = nil ,
		callback = callback,
		ms = ms
	}

	setmetatable(obj, {__index = Interval})

	return obj
end

local function registerInterval( interval )
	log.trace("Timer", "registerInterval", "interval=" .. tostring(interval.timer))
	interval.timer = M.register(function() interval.callback(); interval:loop() end, interval.ms)
end

function Interval:loop()
	if self.running then
		registerInterval(self)
	end
end

function Interval:start()
	self.running = true
	registerInterval(self)
end

function Interval:stop()
	self.running = false
	M.cancel(self.timer)
end

-- Times

local Countdown = {}

function Countdown:start()
	self.count = 0
	self.interval:start()
end

function Countdown:cancel()
	self.interval:stop()
end

function Countdown:get_countdown()
	return self.total - self.count
end

function M.newCountdown( callback, ms, total, finish_cbk )
	local obj = {
		total = total,
		count = 0,
		finish_cbk = finish_cbk
	}

	local cbk = function ()
		if obj.count < obj.total then
			obj.count = obj.count + 1
			callback( obj:get_countdown() )
		else
			if finish_cbk then finish_cbk() end
			obj:cancel()
		end
	end

	obj.interval = M.newInterval(cbk, ms)

	setmetatable(obj, {__index = Countdown})

	return obj
end

return M
