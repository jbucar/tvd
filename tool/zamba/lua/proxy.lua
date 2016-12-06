local l_util = require 'l_util'

local M = {}

local function toObj( self )
	if type(self) == "table" then
		return self.__obj
	end
	return nil
end

function caller( self, method, ... )
	local args = {...}
	local newArgs = {}
	for _, arg in pairs(args) do
		local tmp = toObj(arg) or arg
		table.insert(newArgs, tmp)
	end

	status, res = pcall(function() return l_util.table.pack(self[method](unpack(newArgs))) end)

	if not status then
		error(res:reverse():gsub(':.*',''):reverse(), 3)
	end

	return unpack(res)
end

function methodAllowed( blackList, method )
	return blackList[method] == nil
end

pmt = {}

pmt.__index = function(t,k)
	if methodAllowed( rawget(t, "__blackList"), k ) then
		return function(...) return caller(rawget(t, "__obj"), k, ...) end
	end
	return nil
end

pmt.__tostring = function(t)
	return "Proxy object on " .. tostring(t.__obj)
end

local function deny( self, method )
	self.__blackList[method] = true
end

pmt.__metatable = {}


function M.createProxy( obj )
	proxy = {}
	proxy.__obj = obj
	proxy.__blackList = {}
	proxy.deny = function( method ) deny(proxy, method) end
	proxy.get = function() return proxy.__obj end
	setmetatable(proxy, pmt)
	return proxy
end

return M