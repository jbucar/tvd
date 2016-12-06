--- Module Event Manager

-- Local vars
local timeDelay   = 90
local can_process = true
local stage       = nil
local map         = nil
local timersCallback = nil

function EventManager_Init()
	-- Instance Attributes Initialization
	stage = nil
	map   = nil
	timersCallback = {}
end

local function EventManager_processTimeOut( aTimeOutID )
	if timersCallback[ aTimeOutID ] then
		timersCallback[ aTimeOutID ]()
	end
	timersCallback[ aTimeOutID ] = nil
	timer.cancel( aTimeOutID )
end

function EventManager_timerRegister( callBackFunction, delay )
	local id = timer.register( delay )
	timersCallback[ id ] = function () callBackFunction() end
	return id
end

function EventManager_timerCancel( id )
	timer.cancel( id )
	timersCallback[ id ] = nil
end

--- Set the stage from this Event Manager.
-- @param stage the Event Manager will send events to this stage.
function EventManager_setStage(stage)
	can_process = (stage ~= nil)
	stage = stage
	map = stage.keysMap
end

local function EventManager_process_key( key )
	local key_processed = false
	if map[key] then
		map[key]()
		key_processed = true
	end
	can_process = true
	return key_processed
end

local function EventManager_dispatchPressedKeyEvent()
	if can_process then
		can_process = false
		local id = timer.register( timeDelay )
		timersCallback[ id ] = function() EventManager_enableKeys() end
	end
end

--- Handler from key events
-- @param evt key event.
function EventManager_handler( evt )
	local result = false
	if ( evt.type == 'key' and can_process ) and ( evt.isUp == 0 ) then
		result = EventManager_process_key( evt.value )
		EventManager_dispatchPressedKeyEvent()
	end
	if ( evt.type == 'timeOut' ) then
		EventManager_processTimeOut( evt.value )
		result = true
	end
	return result
end

--- Enable listen key events
function EventManager_enableKeys()
	can_process = true
end

--- Disable listen key events
function EventManager_disableKeys()
	can_process = false
end