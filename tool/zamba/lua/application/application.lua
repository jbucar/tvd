-- /*******************************************************************************

--   Copyright (c) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata
--   All rights reserved.

-- ********************************************************************************

--   Redistribution and use in source and binary forms, with or without 
--   modification, are permitted provided that the following conditions are met:
--     * Redistributions of source code must retain the above copyright
--       notice, this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--       notice, this list of conditions and the following disclaimer in the
--       documentation and/or other materials provided with the distribution.
--     * Neither the name of the author nor the names of its contributors may
--       be used to endorse or promote products derived from this software
--       without specific prior written permission.

--   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
--   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
--   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
--   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
--   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
--   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
--   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
--   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-- ********************************************************************************

--   La redistribución y el uso en las formas de código fuente y binario, con o sin
--   modificaciones, están permitidos siempre que se cumplan las siguientes condiciones:
--     * Las redistribuciones del código fuente deben conservar el aviso de copyright
--       anterior, esta lista de condiciones y el siguiente descargo de responsabilidad.
--     * Las redistribuciones en formato binario deben reproducir el aviso de copyright
--       anterior, esta lista de condiciones y el siguiente descargo de responsabilidad
--       en la documentación y/u otros materiales suministrados con la distribución.
--     * Ni el nombre de los titulares de derechos de autor ni los nombres de sus
--       colaboradores pueden usarse para apoyar o promocionar productos derivados de
--       este software sin permiso previo y por escrito.

--   ESTE SOFTWARE SE SUMINISTRA POR LIFIA "COMO ESTÁ" Y CUALQUIER GARANTÍA EXPRESA
--   O IMPLÍCITA, INCLUYENDO, PERO NO LIMITADO A, LAS GARANTÍAS IMPLÍCITAS DE
--   COMERCIALIZACIÓN Y APTITUD PARA UN PROPÓSITO DETERMINADO SON RECHAZADAS. EN
--   NINGÚN CASO LIFIA SERÁ RESPONSABLE POR NINGÚN DAÑO DIRECTO, INDIRECTO, INCIDENTAL,
--   ESPECIAL, EJEMPLAR O CONSECUENTE (INCLUYENDO, PERO NO LIMITADO A, LA ADQUISICIÓN
--   DE BIENES O SERVICIOS; LA PÉRDIDA DE USO, DE DATOS O DE BENEFICIOS; O INTERRUPCIÓN
--   DE LA ACTIVIDAD EMPRESARIAL) O POR CUALQUIER TEORÍA DE RESPONSABILIDAD, YA SEA
--   POR CONTRATO, RESPONSABILIDAD ESTRICTA O AGRAVIO (INCLUYENDO NEGLIGENCIA O
--   CUALQUIER OTRA CAUSA) QUE SURJA DE CUALQUIER MANERA DEL USO DE ESTE SOFTWARE,
--   INCLUSO SI SE HA ADVERTIDO DE LA POSIBILIDAD DE TALES DAÑOS.

-- *******************************************************************************/

local log = require 'log'
local l_util = require 'l_util'
local application_window = require 'application.window'

----------------------------------------------------------------------------------

-- Internal vars
local M = {}
local status = { enabled = 0, disabled = 1, running = 2, setuping = 3 }

----------------------------------------------------------------------------------
-- Application Internals
----------------------------------------------------------------------------------

local function abort_app( app )
	closeWgt(app._window)
	app._window:warn("ERROR EN LA APLICACIÓN", "Oh, esto es vergonzoso, parece que la aplicación no funciona correctamente")
end

local function protected_call_in_env( app, fnc, args, abort )
	--Fix args
	if type(args) ~= "table" then args = {args} end

	local methodResult = nil
	local result, errorMsg = pcall( function() methodResult = fnc(unpack(args)) end)

	if result then
		if methodResult == nil then
			methodResult = true
		end
		return methodResult
	else
		log.warn("Application", "ZaMBA application generated an error: " .. errorMsg)
		if abort then
			abort_app(app)
		end
		return true
	end
end


-- dispatchMethod(app, method,[ [arguments], [abort] ])
-- arguments is a table with values that are going to pass to the method as arguments
-- abort specify if the app must be stopped when the method fails or return false
local function dispatchMethod( app, method, args, abort )
	local result = false

	-- If args is missing ( args must be a table ) then abort takes the value
	if type(args) == "boolean" then
		abort = args
	end

	if method ~= nil then
		result = protected_call_in_env(app, method, args, abort)
	else
		local msg = "ZaMBA application does not implement the method"
		if abort then
			log.warn( "Application", msg )
		else
			log.debug( "Application", msg )
		end
	end

	return result
end

local function check_description( desc )
	local result = type(desc) == "table"

	if result then
		result = (result and (desc.name ~= nil))
		result = (result and (type(desc.interactive) == "boolean"))
		result = (result and (type(desc.methods) == "table"))

		if result then
			local methods = desc.methods
			result = (result and (type(methods.start) == "function"))
			result = (result and (type(methods.stop) == "function"))
		end
	end

	return result
end

-- Net aux methods
local function set_net_req( app, request, st )
	app._requests[request] = st
end

local function wrap_callback( app, cbk )
	return function( ... )
		local response = l_util.table.pack(...)
		-- response[2] must be the id argument
		set_net_req(app, response[2], false)
		if app.status == status.running then
			dispatchMethod(app, cbk, response, true)
		end
	end
end

-- Status aux methods

local function set_stop_st( app, value )
	if app.interactive then
		app.status = status.enabled
	else
		app.status = value
	end
end

local function stop( app )
	log.debug( "Application", "Stop", tostring(app) )

	if app.status == status.setuping then
		dispatchMethod(app, app.methods.stopSetup)
		set_stop_st(app, status.running)
	else
		dispatchMethod(app, app.methods.stop)
		set_stop_st(app, status.disabled)
	end

	-- TODO: Check this
	-- M.cancel_all_requests(app)
end

-- Window

local function onKey( app, key )
	local evt = {type="key", value=key, isUp=FALSE}
	return M.handle_event(app, evt)
end

----------------------------------------------------------------------------------
-- Application API
----------------------------------------------------------------------------------

function M.new( id, manager, root_path )
	local app = {}

	app.id = id
	app._manager = manager
	app._root_path = root_path

	app._env = nil
	app._requests = {}

	setmetatable(app, {__tostring = function(app) return app.id end})

	app.status = status.disabled
	app._tmp_directory = ""

	app._window = application_window:new()
	app._window:listen('onKey', function(window, key) return onKey(app, key) end)
	app._window:listen('onHide', function(window, ...) stop(app) end)

	return app
end

function M.initialize( app )
	if app._env then
		local desc = protected_call_in_env(app._env, app._env.describe, {})
		if check_description(desc) then
			app.methods = desc.methods
			app.name = desc.name
			app.version = desc.version
			app.desc = desc.desc
			app.icon = desc.icon
			app.selectedIcon = desc.selectedIcon
			app.interactive = desc.interactive
			app.enableDefault = desc.enableDefault or false and true

			log.trace("Application", "initialized: name= "  .. app.name)

			return true
		end
	else
		log.debug("Application", "Cannot initialize application. environment missing")
	end

	return false
end

function M.finalize( app )
	-- Do nothing
end

-- Setters // Getters
function M.name( app, name )
	if name then
		app.name = name
	end
	return app.name
end

function M.file( app, file )
	if file then
		app.file = file
	end
	return app.file
end

function M.environment( app, env )
	if env then
		app._env = env
	end
	return app._env
end

function M.tmp_directory( app, directory)
	if directory then
		app._tmp_directory = directory
	end
	return app._tmp_directory
end

function M.root_path( app, path)
	if path then
		app._root_path = path
	end
	return app._root_path
end

-- Net

function M.fetch( app, opts )
	opts.callback = wrap_callback(app, opts.callback)

	local id = netlib.fetch(opts)
	set_net_req(app, id, true)

	return id
end

function M.http_request( app, opts )
	opts.callback = wrap_callback(app, opts.callback)

	local id = netlib.httpRequest(opts)
	set_net_req(app, id, true)

	return id
end

function M.cancel_request( app, request )
	log.debug( "Application", "cancel_request", string.format("cancel application request(%s)", tostring(request)))
	netlib.cancel(request)
	set_net_req(app, request, false)
end

function M.cancel_all_requests( app )
	for request, active in pairs(app._requests) do
		log.debug( "Application", "cancel_all_requests", string.format("cancel application request(%s)", tostring(request)))
		if active then
			log.debug( "Application", "cancel_all_requests", string.format("cancel application request(%s)", tostring(request)))
			netlib.cancel(request)
		end
	end
	app._requests = {}
end

-- Status
function M.enable( app )
	local result = true

	if app.interactive then
		app.status = status.enabled
	else
		result = M.start(app)
	end

	return result
end

function M.disable( app )
	if app.interactive then
		app.status = status.disabled
	else
		stop(app)
	end
end

function M.start( app )
	local result = false

	if app.interactive then
		openWgt(app._window)
		result = dispatchMethod(app, app.methods.start, {app._window:getSurface()}, true)
	else
		result = dispatchMethod(app, app.methods.start, true)
	end

	if result then
		app.status = status.running
	end

	return result
end

function M.start_setup( app )
	openWgt(app._window)
	if dispatchMethod(app, app.methods.startSetup, {app._window:getSurface()}) then
		app.status = status.setuping
	end
end

function M.handleStatus( app )
	return type(app._env.status) == "function"
end

function M.status( app )
	local state = -1
	if M.handleStatus(app) then
		state = protected_call_in_env(app._env, app._env.status, {})
	else
		state = storage.loadInt("appManager." .. app.id, -1)
	end
	return state
end

function M.updateStatus( app )
	app.status = (M.status(app) ~= 0) and status.enabled or status.disabled
end

function M.is_running( app )
	return (app.status == status.running) or (app.status == status.setuping)
end

function M.is_enable( app )
	return app.status ~= status.disabled
end

function M.is_interactive( app )
	return app.interactive
end

function M.is_service( app )
	return not M.is_interactive(app)
end

function M.is_setupable( app )
	return app.methods.startSetup ~= nil
end

function M.handle_event( app, evt )
	log.debug("Application", "handle_event by="  .. app.name .. " key=" .. tostring(evt.value))
	if app.methods.handleEvent ~= nil then
		return dispatchMethod( app, app.methods.handleEvent, {evt} )
	end
end

function M.call( app, fnc, ... )
	return dispatchMethod(app, fnc, {...}, true)
end

return M
