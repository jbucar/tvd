local alert = require 'widgets.alert'

local _M = {}

local required_dialog = nil

local function open_dialog()
	if required_dialog == nil then
		required_dialog = alert.new()
		required_dialog:text("Debe establecer una clave en el menú seguridad de la pestaña AJUSTES para habilitar esta opción.", 5)
	end
	openWgt(required_dialog)
end

local function process( callback, cancelCallback, required, msg )
	local res = false
	if PINSet() then
		if type(msg) == "string" then
			PINDlg.init(msg, dialogZIndex, callback, cancelCallback)
		else
			PINDlg.init("", dialogZIndex, callback, cancelCallback)
			PINDlg.description(unpack(msg))
		end
		openWgt(PINDlg)
	else
		if required then
			open_dialog()
		else
			callback()
			res = true
		end
	end
	return res
end

-- API

function _M.protected( callback, cancelCallback )
	local msg = {"La sesión ha expirado.", "Ingrese la clave para realizar ésta acción", "Botón \'EXIT\' = Menú de Ajustes"}
	return process(callback, cancelCallback, false, msg)
end

function _M.call( callback, cancelCallback )
	return process(callback, cancelCallback, false, "Por favor, ingrese la clave")
end

function _M.required( callback, cancelCallback )
	return process(callback, cancelCallback, true, "Por favor, ingrese la clave")
end

return _M
