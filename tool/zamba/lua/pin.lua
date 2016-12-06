local log = require 'log'
local l_util = require 'l_util'
local alert = require 'widgets.alert'
local config = require 'menu.config_page'

require 'info_boxes.common_info'

local PINLength = 4
local typedPIN = ""

function PINSet()
	return control.isSet()
end

function isInRange(number)
	return number >= 0 and number <=9
end

local dw = 450
local dh = 200
local dx, dy = l_util.center_in_canvas(dw, dh)

function PINSize()
	return PINLength
end

PINDlg = {
	  okCbk = nil
	, cancelCbk = nil
	, pinIncorrect = false
}

local wrong_password_alert = nil

local function onWrongPassword()
	closeWgt(PINDlg)
	openWgt(wrong_password_alert)
end

function PINDlg.init(promptText, zIndex, okCbk, cancelCbk)
	PINDlg = BaseWidget:new( PINDlg, "PINDlg", true)
	PINDlg.wgt.dlg = pindialog:new(dx,dy, dw,dh, alertColorTable, zIndex, 4, PINLength)
	PINDlg:propagateEvts(false)
	PINDlg.okCbk = okCbk
	PINDlg.cancelCbk = cancelCbk
	pindialog:title( PINDlg.wgt.dlg, "CLAVE REQUERIDA" )
	pindialog:prompt( PINDlg.wgt.dlg, promptText )
	pindialog:icon( PINDlg.wgt.dlg, "Clave.png", 70, 61, "Clave_Gris.png" )
	pindialog:help( PINDlg.wgt.dlg, "Botón \'EXIT\' = Salir" )
	pindialog:callbacks(PINDlg.wgt.dlg, "onPINEntered", "onPINCanceled")

	pindialog:onWrongPassword(PINDlg.wgt.dlg, onWrongPassword)

	wrong_password_alert = alert.new(dx, dy, dw, dh)
	wrong_password_alert:text("La contraseña es incorrecta")
	wrong_password_alert:callback(function() openWgt(PINDlg) end)
end

function PINDlg.description(title, prompt, help)
	pindialog:title( PINDlg.wgt.dlg, title )
	pindialog:prompt( PINDlg.wgt.dlg, prompt )
	pindialog:help( PINDlg.wgt.dlg, help )
end

function PINDlg:processKeys(key)
	if PINDlg.pinIncorrect and (key == KEY_OK or key == KEY_ENTER) then
		typedPIN = ""
		PINDlg.pinIncorrect = false
	else
		if isInRange(key - 1) then
			typedPIN = typedPIN .. ( key - 1 )
		end
		if PINLength == string.len( typedPIN ) then
			if control.checkPIN( typedPIN ) then
				pindialog:pinCorrect(PINDlg.wgt.dlg)
			else
				pindialog:pinIncorrect(PINDlg.wgt.dlg)
				typedPIN = ""
				PINDlg.pinIncorrect = true
			end
		end
	end
	return wgt:processKey(PINDlg.wgt.dlg, key)
end

function PINDlg:onShow()
	typedPIN = ""
	if control.isSessionOpened() then
		if PINDlg.okCbk ~= nil then
			PINDlg.okCbk()
		end
		return false
	else
		close_current_infobox()
		view.refresh()
		return true
	end
end

function PINDlg.onExit(callback)
	closeWgt(PINDlg)
	if callback ~= nil then
		callback()
	end
end

function onPINEntered()
	log.debug("zamba", "onPINEntered", "callback = " .. tostring(okCbk))
	PINDlg.onExit(PINDlg.okCbk)
end

function onPINCanceled()
	log.debug("zamba", "onPINCanceled", "callback = " .. tostring(okCbk))
	PINDlg.onExit(PINDlg.cancelCbk)
end


