--- Class Object
Object = {
	-- Class Atributes
	super = nil
	-- Instance Atributes
}
Object.__index = Object
_G.Object = Object

--- Class constructor
-- @param ... are used by the <a href="#">Object:Init(...)</a> function. For more information about this param, see Lua 5.1 manual.
-- @return an instance of Object.
function Object:new(...)
	local temp = {}
	setmetatable(temp,self)
	temp:Init(...)
	return temp
end

--- Class initializer
--- Parameters must be send in Object:new() as declared in Object:Init()
function Object:Init(...)
	-- Instance Atributes Initialization
end
