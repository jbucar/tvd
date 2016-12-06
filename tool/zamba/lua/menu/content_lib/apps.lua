local log = require 'log'
local app_ctrl = require 'application.controller'
local apps_download = require 'models.applications_download'
local channel = require 'models.channel'
local gWindow = require 'ginga_window'
local Confirmation = require 'widgets.confirmation'
local Tab = require 'menu.content_lib.tab'
local l_util = require 'l_util'

local _M = {} -- Module

local AppsTab = {}

setmetatable(AppsTab, Tab.mt) -- Inherits behaviour from Widget
AppsTab_mt = { __index = AppsTab }

local list_description = {
	visible_columns = {"name"},
	headers = {'Aplicaciones'},
	index_key = "id",
	columns_sizes = {config.page.list.width}
}

-- Internals

local function execute_ginga( id )
	if middleware.runApplication(id) then
		closeToRoot()
		openWgt(gWindow)
		app_ctrl.start_launch()
		return true
	end
	return false
end

local function add_apps( list, callback, values )
	for _, file in pairs(list) do
		file.onExecute = callback
		table.insert(values, file)
	end
end

-- API

function _M.new( win )
	local self = Tab.new(list_description, "Aplicaciones", "application", "Muestra las aplicaciones", "Ejecutar aplicación")
	setmetatable(self, AppsTab_mt)

	self._close_app_dialog = Confirmation.new()
	self._close_app_dialog:onYes(function() self:close_app() end)
	self._close_app_dialog:text("¿Desea que se cierre la aplicación en curso y se ejecute la aplicación seleccionada?", 5)

	window:addChild(win, self.content)
	wgt:setName(self.content, string.format("Tab(%s)", "Apps"))

	apps_download.observe('progress', function(...) return self:update_if_complete(...) end)
	channel.observe('change', function(...) return self:update(...) end)

	return self
end

function AppsTab:onOk()
	if app_ctrl.app_actived() then
		local appInfo = middleware.current()
		local selected = list:getCurrentValue(self.content)
		if appInfo and appInfo.id ~= selected then
			openWgt(self._close_app_dialog)
		else
			self:run_app()
		end
	else
		self:run_app()
	end
end

function AppsTab:update_if_complete( step, total )
	if self._open then
		log.debug("AppsTab", "update_if_complete", string.format("step=%d, total=%d", step, total))
		if step == total then
			self:update()
			return true
		end
	end
	return false
end

function AppsTab:run_app()
	local app, id = self:selected_item()
	setRestorePage(true)
	app.onExecute(id)
end

function AppsTab:close_app()
	setRestorePage(false)
	local appInfo = middleware.current()
	if appInfo ~= nil then
		middleware.stopApplication(appInfo.id)
	end
	self:run_app()
end

function AppsTab:values()
	local values = {}
	add_apps(middleware.getApplications(), execute_ginga, values)
	add_apps(zmb_apps.list(true), zmb_apps.start, values)
	return values
end

return _M
