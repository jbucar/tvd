local l_util = require 'l_util'
local log = require 'log'
local timer = require 'timer'

_G.name = "GLOBAL"

function get_chunk_module( modname, loader_env, dest_env )
	local loader = debug.setfenv(application_helper.get_module_loader, loader_env)(modname)
	return debug.setfenv(loader, dest_env or loader_env)
end

function require_in_environment( modname )
	-- Cache
	if package.loaded[modname] then
		return package.loaded[modname]
	end

	local chunk = get_chunk_module(modname, _G)

	package.loaded[modname] = chunk() or package.loaded[modname]
	return package.loaded[modname]
end

function module_in_environment( modname, env )
	newgt = application_helper.module(modname)
	setmetatable(newgt, {__index = env})
	setfenv(3, newgt)

	env.package.loaded[modname] = newgt
end

function create_env( configure, ... )
	local env = {}
	setmetatable(env, {__index = _G})

	env.name = "SANDBOX"
	env._G = env

	env.module = function( modname ) module_in_environment(modname, env) end

	function load_module( modname )
		return get_chunk_module(modname, _G, env)()
	end

	env.package = {
		loaded = {
			-- Standard libs
			string = string,
			table = table,
			io = io,
			math = math,
			os = os,
			debug = debug,

			-- Zamba libs
			timer = timer,
			spinner = load_module("spinner"),
			log = log
		},
		path = "",
		loaders = _G.package.loaders,
		preload = {},
		cpath = _G.package.cpath,
		seeall = function(...) return nil end,
		loadlib = function(...) return nil end
	}

	-- Require is made in the SANDBOX environment
	env.require = function( modname ) return setfenv(require_in_environment, env)(modname) end

	if configure ~= nil then
		setfenv(configure, env)(_G, ...)
	end

	return env
end

function load_chunk(file, env)
	local result = false
	local chunk, err = loadfile(file)

	-- If not error at compiled time
	if err == nil then
		local sandboxed = setfenv(chunk, env)
		local success = false
		sucess, result = pcall( sandboxed )
		-- if error at runtime
		if not sucess then
			err = result
			result = false
		end
	end
	return result, err
end

function sandbox( file, configure, ... )
	local env = create_env( configure, ... )

	local result, err = load_chunk(file, env)
	if err ~= nil then
		return err
	end
	return env
end

function wrap( fnc, new_env )
	return function(...)
		local old_env = _G
		setfenv(0, new_env)
		local r = fnc(...)
		setfenv(0, old_env)
		return r
	end
end

return { sandbox = sandbox, wrap = wrap }
