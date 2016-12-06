local log = require 'log'
local zmb_apps = require 'application.zmb_apps'
local application = require 'application.application'
local security = require 'security'

app_config_win = {}

local function help( msg, action_msg )
	return string.format("%s.\nOK=%s   EXIT = Salir del menú    MENU = Menú ppal.", msg, action_msg)
end

function app_config_win.create(x, y, w, h)
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)
	app_config_win.btnSet = verticalset:new(0, 0, 0, 0, menuColorTable)

	app_config_win.enableButton = button:new( w/2 - 145, y + h/2, 290, 26, buttonColorTable)
	button:label(app_config_win.enableButton, "Habilitar")
	button:fontSize(app_config_win.enableButton, 15)
	button:action(app_config_win.enableButton, app_config_win.enablePressed)
	button:selectedCallback(app_config_win.enableButton, function() updateHelp(help("Habilita/Deshabilita la aplicación", "Habilitar/Deshabilitar")) end)
	
	local configButton = button:new( w/2 - 145, y + h/2 + 50, 290, 26, buttonColorTable)
	button:label(configButton, "Configurar")
	button:fontSize(configButton, 15)
	button:action(configButton, app_config_win.configPressed)
	button:selectedCallback(configButton, function() updateHelp(help("Configura la aplicación", "Entrar al menú de configuración")) end)
	
	app_config_win.name = textarea:new(x, y, w, 30, menuColorTable, "", 16, 2, 0, Alignment.vTop_hCenter)
		  
	app_config_win.desc = textarea:new(x, y + 30, w, 50, menuColorTable, "", 16, 2, 0, Alignment.vTop_hCenter)
		  
	app_config_win.version = textarea:new(x, y + 60, w, 30, menuColorTable, "", 16, 2, 0, Alignment.vTop_hCenter)

	verticalset:circular(app_config_win.btnSet, TRUE)
	verticalset:addWidget(app_config_win.btnSet, app_config_win.enableButton)
	verticalset:addWidget(app_config_win.btnSet, configButton)
	verticalset:setActiveItem(app_config_win.btnSet, 0)
	
	window:addChild(win, app_config_win.name)
	window:addChild(win, app_config_win.version)
	window:addChild(win, app_config_win.desc)
	window:addChild(win, app_config_win.btnSet)
	app_config_win.win = win
	app_config_win.app = nil

	wgt:setVisible(app_config_win.win, FALSE)

	return app_config_win.win
end

function app_config_win.setApp(app)
	textarea:setText(app_config_win.name, app.name)
	textarea:setText(app_config_win.version, "Versión: " .. app.version)
	textarea:setText(app_config_win.desc, app.desc)
	app_config_win.app = app
	app_config_win.configureButtons()
end

function app_config_win.handleKey(key)
	if key == KEY_EXIT or key == KEY_ESCAPE then
		show_grid(false)
		return true
	else
		return wgt:processKey(app_config_win.btnSet, key)
	end
end

function app_config_win.configureButtons()
	local enabled = application.is_enable(app_config_win.app)
	log.debug("app_config_win", "configureButtons", "enabled=" .. tostring(enabled))
	if enabled then
		button:label(app_config_win.enableButton, "Deshabilitar")
		verticalset:enableWidget( app_config_win.btnSet, 1, application.is_setupable(app_config_win.app) and TRUE or FALSE)
	else
		button:label(app_config_win.enableButton, "Habilitar")
		verticalset:enableWidget( app_config_win.btnSet, 1, FALSE)
	end
	verticalset:setActiveItem(app_config_win.btnSet, 0)
end

------------------------------Callbacks----------------------------------------------

function app_config_win.enablePressed()
	security.protected(function() 
		zmb_apps.enable_app(app_config_win.app.id, not application.is_enable(app_config_win.app))
		app_config_win.configureButtons()
	end, function() openSubWin(menu) end)
end

function app_config_win.configPressed()
	security.protected(function() 
		zmb_apps.start_setup(app_config_win.app.id)
	end, function() openSubWin(menu) end)
end