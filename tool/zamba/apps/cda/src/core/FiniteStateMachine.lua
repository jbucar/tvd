--- Module Finite State Machine

-- Local vars
local	current_stage						= nil
local	state_transition_table	= nil
local	initial_stage						= nil

--- Module initialization method
function FiniteStateMachine_Init()
	state_transition_table =	{}
end

--- Change to aStage
local function FiniteStateMachine_changeStageTo(aStage)
	current_stage = aStage
	EventManager_setStage(current_stage)
end

--- Start method, change to initial Stage
function FiniteStateMachine_start()
	FiniteStateMachine_changeStageTo(initial_stage)
	current_stage:show() --HACK!!!
end

--- Set the initial Stage
function FiniteStateMachine_setInitialStage(aStage)
	initial_stage = aStage
end

--- @usage  FiniteStateMachine_addGlobalTransition('toGreen',GreenStage)
function FiniteStateMachine_addTransition(idTransition,toStage)
	if (state_transition_table == nil) then
	state_transition_table = {}
	end
	state_transition_table[idTransition] = function () return toStage end
end

--- Execute a transition
function FiniteStateMachine_doTransition( idTransition, ... )
	FiniteStateMachine_changeStageTo( state_transition_table[idTransition]() )
	current_stage:show( ... )
end