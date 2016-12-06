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

local l_util = require 'l_util'
local log = require 'log'
local application = require 'application.application'
local app_surface = require 'application.surface'
local sandbox = require 'application.sandbox'
local timer_module = require 'timer'

local M = {}

-- Consts
local TMP_DIR = "apps"

----------------------------------------------------------------------------------
-- Application Manager Internals
----------------------------------------------------------------------------------

local function make_app_id( file )
	return string.gsub(file, "/", ".")
end

local function save_state( app, st )
	log.debug("ApplicationManager", string.format("Save application status, app=%s status=%s", application.file(app), tostring(st)))
	storage.saveInt("appManager." .. app.id, st)

	if not storage.commit() then
		log.warn( "ApplicationManager", string.format("Cannot save the state of %s application", application.name(app)))
	end
end

local function config_env( global_env, manager, app )
	-- Global rootPath var
	rootPath = application.root_path(app)

	package.path = rootPath .. "/?.lua"

	-- Canvas lib
	canvas = {}
	setmetatable(canvas, {__index = global_env.canvas})

	canvas.createSurfaceFromPath = function(path)
		if not fs.path_is_absolute(path) then
			path = fs.make_path(application.root_path(app), path)
		end
		local s1 = global_env.canvas.createSurfaceFromPath(path)
		s1:setZIndex(canvas.zapperZIndex())
		return app_surface.create(s1)
	end

	canvas.createSurface = function(...)
		local s1 = global_env.canvas.createSurface(...)
		s1:setZIndex(canvas.zapperZIndex())
		return app_surface.create(s1)
	end

	-- Timer lib
	_G.timer = {}

	function timer.register( ms )
		return timer_module.register(function(tid) dispatch_timer_evt(manager, tid) end, ms)
	end

	function timer.cancel( timerID )
		timer_module.cancel(timerID)
	end

	-- Net lib
	_G.net = {}

	function net.fetch( opts )
		opts.temporaryDirectory = application.tmp_directory(app)
		return application.fetch(app, opts)
	end

	function net.httpRequest( opts )
		return application.http_request(app, opts)
	end

	function net.cancel( request_id )
		application.cancel_request(app, request_id)
	end

	function net.cancelAll()
		application.cancel_all_requests(app)
	end

end

local function tmp_directory()
	return fs.make_path(mainWindow.ramDisk(), TMP_DIR)
end

local function register_app( manager, file )
	log.trace( "ApplicationManager", "try to register application defined in "  .. file)

	local app_path = fs.split_path(file)

	local app = application.new(make_app_id(file), manager, app_path)

	local envOrErr = sandbox.sandbox(file, config_env, manager, app)
	local env = nil
	local errorMsg = nil

	if type(envOrErr) == "table" then
		env = envOrErr
	else
		errorMsg = envOrErr
	end

	local result = false

	if env then
		if type(env.describe) == "function" then

			application.environment(app, env)

			if application.initialize(app) then
				log.debug( "ApplicationManager", "register application defined in "  .. file)

				application.file(app, file)

				application.tmp_directory(app, tmp_directory())

				manager._apps[app.id] = app

				local state = application.status( app )
				if (state == 1) or (state == -1 and app.enableDefault) then
					result = application.enable(app)
				else
					result = true
				end

			else
				log.warn( "ApplicationManager", "ZaMBA application bad described: "  .. file)
			end
		else
			log.warn( "ApplicationManager", "ZaMBA application without describe method: " .. file)
		end
	else
		log.warn( "ApplicationManager", "ZaMBA application error: " .. errorMsg)
	end

	return result
end

----------------------------------------------------------------------------------
-- Application Manager API
----------------------------------------------------------------------------------

function M.new()
	manager = {}

	manager._apps = {}

	manager._current_app = nil

	return manager
end

function M.initialize( manager )
	log.info( "ApplicationManager", "initialize")

	fs.create_directory(tmp_directory())
	local files = fs.scan_directory_by_extension(application_helper.path(), ".zmb")

	if files then
		for _, file in ipairs(files) do
			register_app(manager, file)
		end
	else
		log.info( "ApplicationManager", "No zamba applications were found")
	end
end

function M.finalize( manager )
	log.info( "ApplicationManager", "finalize")
	for _, app in pairs(manager._apps) do
		application.finalize(app)
	end

	fs.remove_directory(tmp_directory())
end

-- Getters

function M.getApp( manager, appID )
	return manager._apps[appID]
end

function M.list( manager, interactive )
	interactive = interactive or false
	local result = {}

	for _, app in pairs(manager._apps) do
		if (not interactive or (interactive and application.is_interactive(app) and application.is_enable(app))) then
			log.trace( "ApplicationManager", "list application="  .. app.name .. " id=" .. app.id)
			table.insert(result, app)
		end
	end
	return result
end

-- Status

function M.startApp( manager, appID )
	log.debug( "ApplicationManager", "StartApp", "application " .. appID)
	local result = false
	local app = M.getApp(manager, appID)
	if app ~= nil then
		-- Create tmp directory
		fs.create_directory(application.tmp_directory(app))
		result = application.start(app)
	end
	return result
end

function M.startSetupApp( manager, appID )
	log.info( "ApplicationManager", "StartSetupApp", "application " .. appID )
	local app = M.getApp(manager, appID)
	if app ~= nil then
		application.start_setup(app)
	end
end

function M.enableApp( manager, appID )
	log.trace( "ApplicationManager", "EnableApp", "application " .. appID)
	local app = M.getApp(manager, appID)
	if app ~= nil then
		save_state(app, 1)
		return application.enable(app)
	end
	return false
end

function M.disableApp( manager, appID )
	log.debug( "ApplicationManager", "DisableApp", "application " .. appID)
	local app = M.getApp(manager, appID)
	if app ~= nil then
		save_state(app, 0)
		application.disable(app)
	end
end

function M.disableAll( manager )
	for _, app in pairs(manager._apps) do
		M.disableApp(manager, app.id)
	end
end

function M.restoreApps( manager )
	for _, app in pairs(manager._apps) do
		if (application.handleStatus(app)) then
			application.updateStatus(app)
		elseif (app.enableDefault) then
			M.enableApp( manager, app.id )
		else
			M.disableApp( manager, app.id )
		end
	end
end

-- Events

function M.dispatchEvent( manager, evt )
	log.trace("ApplicationManager", "dispatchEvent", "type=" .. tostring(evt.type) .. " value=" .. tostring(evt.value))
	for _, app in pairs(manager._apps) do
		if application.is_running(app) and (evt.type ~= "key" or application.is_service(app)) then
			application.handle_event(app, evt)
		end
	end
end

function dispatch_timer_evt( manager, tID )
	M.dispatchEvent(manager, {type="timeOut", value=tID})
end

return M

