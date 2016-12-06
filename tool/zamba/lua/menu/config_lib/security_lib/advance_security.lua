local log = require 'log'
local style = require 'style'
local widget = require 'widget'
local Alert = require 'widgets.alert'
local Confirmation = require 'widgets.confirmation'
local security_lib = require 'security'

require "menu.config_lib.security_lib.parental"

advance_security = {}

-- Vars

local delete_confirmation = nil
local close_alert = nil

local times = {
	{ desc = "5 minutos", seconds = 300 },
	{ desc = "30 minutos", seconds = 1800 },
	{ desc = "1 hora", seconds = 3600 },
	{ desc = "2 horas", seconds = 7200 },
	{ desc = "No expira", seconds = -1 }
}

-- Internals

local function help( msg, action )
	return string.format("%s.\nOK = %s   EXIT = Salir del menú    MENU = Menú ppal.", msg, action)
end

local function findKey( seconds )
	local position = -1
	for i=1, #times, 1 do
		if times[i].seconds == seconds then
			position = i
		end
	end
	return position
end

local function update_buttons()
	verticalset:setActiveItem( advance_security.verticalSet, 0 )
	combobox:setSelected( advance_security.combo, findKey( control.sessionExpireTime() ) - 1 )
	updateHelp(wgt:getHelp(advance_security.verticalSet))
end

local function center( x, w )
	return x / 2  - w / 2
end

local function expire()
	control.expireSession()
	openWgt(close_alert)
end

local function create_alert()
	local alert = Alert.new()
	alert:callback(function() openSubWin(menu) end)
	alert:text("Se ha cerrado la sesión correctamente")
	return alert
end

local function createConfirmDelete()
	local confirm = Confirmation.new()
	confirm:onYes(deletePIN)
	confirm:text("¿Está seguro que desea borrar la clave del sistema? Si lo hace, perderá las marcas de canales bloqueados y la configuración de control parental.", 4)
	return confirm
end

function deletePIN()
	control.resetPIN()
	openSubWin(security)
end

local function unblockPINPage()
	do -- Hack: Same that openSubWin but bypassing the select method
		closeCurrent()
		selectWin(advance_security)
	end

	update_buttons()
end

local function change_time()
	local timeIndex = combobox:getSelected( advance_security.combo ) + 1
	local seconds = times[timeIndex].seconds
	control.setExpireTime( seconds )
end

function timeChanged( changed )
	if changed == TRUE then
		security_lib.protected(change_time, function() openSubWin(menu) end)
	end
end

-- Callbacks

function open_pin()
	openSubWin(set_pin_win)
end

local function confirm_delete()
	openWgt(delete_confirmation)
end

local function open_parental()
	openSubWin(parental)
end

-- API

function advance_security.create(x, y, w, h)
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)
	advance_security.verticalSet = verticalset:new(0, 0, w, h, menuColorTable)

	local button_w = 290
	advance_security.setButton = button:new( center(w, button_w), h/2 - 125, button_w, style.button.height, buttonColorTable)
	button:label(advance_security.setButton, "Modificar clave")
	button:fontSize(advance_security.setButton, 15)
	button:action(advance_security.setButton, open_pin)
	wgt:setHelp(advance_security.setButton, help("Modifica la clave de seguridad para el sistema ", "Modificar clave"))
	
	advance_security.removeButton = button:new( center(w, button_w), h/2 - 75, button_w, style.button.height, buttonColorTable)
	button:label(advance_security.removeButton, "Eliminar clave")
	button:fontSize(advance_security.removeButton, 15)
	button:action(advance_security.removeButton, confirm_delete)
	wgt:setHelp(advance_security.removeButton, help("Elimina la clave de seguridad para el sistema", "Eliminar clave"))

	local subTable = {}
	for i=1, #times, 1 do
		table.insert( subTable, times[i].desc )
	end
	advance_security.combo = widget.makeCombo( center(w, button_w), h/2 - 25, button_w, style.button.height, "Duración de sesión", subTable, 140)
	combobox:action( advance_security.combo, "timeChanged" )
	wgt:setHelp( advance_security.combo, help("Modifica el tiempo de la sesión", "Modificar"))
	
	advance_security.expireButton = button:new( center(w, button_w), h/2 + 25, button_w, style.button.height, buttonColorTable)
	button:label(advance_security.expireButton, "Cerrar sesión")
	button:fontSize(advance_security.expireButton, 15)
	button:action(advance_security.expireButton, expire)
	wgt:setHelp(advance_security.expireButton, help("Cierra la sesión", "Cerrar"))

	advance_security.parentalButton = button:new( center(w, button_w), h/2 + 100, button_w, style.button.height, buttonColorTable)
	button:label(advance_security.parentalButton, "Control parental")
	button:fontSize(advance_security.parentalButton, 15)
	button:action(advance_security.parentalButton, open_parental)
	wgt:setHelp(advance_security.parentalButton, help("Configura el control parental", "Configurar"))
	
	verticalset:circular(advance_security.verticalSet, TRUE)
	verticalset:addWidget(advance_security.verticalSet, advance_security.setButton)
	verticalset:addWidget(advance_security.verticalSet, advance_security.removeButton)
	verticalset:addWidget(advance_security.verticalSet, advance_security.combo)
	verticalset:addWidget(advance_security.verticalSet, advance_security.expireButton)
	verticalset:addWidget(advance_security.verticalSet, advance_security.parentalButton)
	verticalset:setActiveItem(advance_security.verticalSet, 0)

	delete_confirmation = createConfirmDelete()
	close_alert = create_alert()

	window:addChild(win, advance_security.verticalSet)
	advance_security.win = win

	wgt:setVisible(advance_security.win, FALSE)

	return advance_security.win
end

function advance_security.onUnselected()
	combobox:close( advance_security.combo )
	return true
end

function advance_security.handleKey(key)
	local handled = wgt:processKey(advance_security.verticalSet, key)
	if handled and key ~= KEY_OK and key ~= KEY_ENTER then
		updateHelp(wgt:getHelp(advance_security.verticalSet))
	elseif not handeled and (key == KEY_EXIT or key == KEY_ESCAPE) then
		openSubWin(menu)
		handled = true
	end
	return handled
end

function advance_security:onSelected()
	PINDlg.init( "Por favor, ingrese la clave", dialogZIndex, unblockPINPage, nil )
	openWgt(PINDlg)
	return false
end

