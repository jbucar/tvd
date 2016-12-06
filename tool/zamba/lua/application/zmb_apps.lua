-- Modules required
local log = require 'log'
local channel = require 'models.channel'

-- Modules lazy
local AppManager = nil

-- Local vars
local manager = nil

local M = {}

----------------------------------------------------------------------------------
-- Zmb Apps Internals
----------------------------------------------------------------------------------

local function module_enabled()
	return manager ~= nil and AppManager ~= nil
end

----------------------------------------------------------------------------------
-- Zmb Apps API
----------------------------------------------------------------------------------

function M.enable( enable )
	if enable then
		log.info("Zmb_Apps", "ZaMBA applications are enabled")
		AppManager = require "application.manager"
		manager = AppManager.new()
	else
		log.info("Zmb_Apps", "ZaMBA applications are disabled")
	end
end

function M.init()
	if module_enabled() then
		AppManager.initialize(manager)
	end
end

function M.fin()
	if module_enabled() then
		AppManager.finalize(manager)
	end
end

function M.dispatchEvent( evt )
	log.trace("Zmb_Apps", "dispatchEvent", "type=" .. tostring(evt.type) .. " value=" .. tostring(evt.value))
	if module_enabled() then
		AppManager.dispatchEvent(manager, evt)
	end
end

function M.list( interactive )
	local result = {}
	if module_enabled() then
		result = AppManager.list(manager, interactive)
	end
	return result
end

function M.start( appid )
	if module_enabled() then
		closeToRoot()
		return AppManager.startApp(manager, appid)
	end
	return false
end

function M.start_setup( appid )
	if module_enabled() then
		AppManager.startSetupApp(manager, appid)
	end
end

function M.enable_app( appid, enable )
	if module_enabled() then
		if enable then
			AppManager.enableApp(manager, appid)
		else
			AppManager.disableApp(manager, appid)
		end
	end

	return false
end

function M.restore()
	if module_enabled() then
		AppManager.restoreApps(manager)
	end
end

function M.enabled()
	-- TODO: Remove "and TRUE or FALSE"
	-- NOTE: first we need to replace all usage of TRUE and FALSE by true and false
	return module_enabled() and TRUE or FALSE
end

channel.observe('change', function(ix) M.dispatchEvent({type="channelChanged", value=ix}) end)

return M