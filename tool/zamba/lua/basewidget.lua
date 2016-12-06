local log = require 'log'

BaseWidget = {}

local function init_widget( name, lockKeys, acceptsFocus )
	return {
		name = name,
		visible = false,
		lockKeys = lockKeys,
		acceptsFocus = acceptsFocus or lockKeys,
		_ignored_actions = {},
		_propagateEvts = true,
		_bypass_escape = false,
		_not_close_on = nil,
		wgt = {}
	}
end

function BaseWidget:new( subTable, name, lockKeys, acceptsFocus )
	local obj = init_widget(name, lockKeys, acceptsFocus)

	setmetatable(subTable, {__index = obj})
	setmetatable(obj, {__index = self})
	return subTable
end

function BaseWidget:bypass_escape( value )
	if value ~= nil then
		self._bypass_escape = value
	end
	return self._bypass_escape
end

function BaseWidget:acceptFocus( accept )
	self.acceptsFocus = accept
end

function BaseWidget:propagateEvts(propagate)
	self._propagateEvts = propagate
end

function BaseWidget:ignore(actions)
	for _, action in pairs(actions) do
		self._ignored_actions[action] = true
	end
end

function BaseWidget:want_ignore(action)
	return self._ignored_actions[action]
end

function BaseWidget:want_propagate_event(action)
	return self._propagateEvts
end

function BaseWidget:close_on_all_events_except(events)
	self._not_close_on = events
end

function BaseWidget:want_to_be_closed(action)
	return self._not_close_on and not (self._not_close_on[action])
end

function BaseWidget:onOpened()
	log.trace("BaseWidget", "onOpened")
	for _,w in pairs(self.wgt) do
		if type(w) ==  'number' then
			wgt:open(w)
		else
			openWgt(w)
		end
	end
	self.visible = true
end

function BaseWidget:onClosed()
	log.trace("BaseWidget", "onClosed")
	for _,w in pairs(self.wgt) do
		if type(w) == 'number' then
			wgt:close(w)
		else
			closeWgt(w)
		end
	end
	self.visible = false
end

function BaseWidget:addWgt( name, widget )
	self.wgt[name] = widget
end

function BaseWidget:getWgt( name )
	return self.wgt[name]
end

-- Mixin
function BaseWidget:include( mixin_mod )
	mixin_mod:mixin(self)
end

------- Module

local M = {} -- API Module

M.mt = { __index = BaseWidget }

function M.new( name, lockKeys, acceptsFocus )
	return init_widget(name, lockKeys, acceptsFocus)
end

return M
