local log = require 'log'
local l_util = require 'l_util'
require "basewidget"

local updates = {}
local applying_update = false
local apply_update = false

local Dialog = {}

local colors = {
	bgColor = Color.Gray,
	textColor = Color.White
}

local updateColorTable = {
	bgColor = Color.Gray,
	borderColor = Color.Blue,
	textColor = Color.White
}

local function accept_update()
	apply_update = true
	closeWgt(Dialog)
end

local function deny_update()
	apply_update = false
	closeWgt(Dialog)
	updates = {}
end

local function applyUpdates( apply )
	log.debug("firmupdate", "applyUpdates(".. tostring(apply) ..")")
	for _, update_info in pairs(updates) do
		update.apply(update_info.id, apply)
		if apply then
			mainWindow.stop( 2210 )
		end
	end
	applying_update = false
end

local function canShowConfirmUpdate()
	return CurrentWindow() and (CurrentWindow().name == "RootNav")
end

local function confirmUpdate()
	log.debug("firmupdate", "confirmUpdate()")
	if not mainWindow.inStandby() and canShowConfirmUpdate() then
		Dialog.initialize()
		openWgt(Dialog)
	else
		log.debug("firmupdate", "cannot open dialog")
	end
end

-- Dialog widget

function Dialog.initialize()
	log.debug("firmupdate", "Dialog.initialize")

	Dialog = BaseWidget:new( Dialog, "UpdateDlg", true)
	Dialog:propagateEvts(false)

	local w = 554
	local h = 221
	local x, y = l_util.center_in_canvas(w, h)
	local margin = 10

	local update_warn = yesnodialog:new(x, y, w, h, alertColorTable, dialogZIndex, 4, 175, 36, 5)

	yesnodialog:title(update_warn, "ACTUALIZACIONES DISPONIBLES")
	yesnodialog:icon(update_warn, "actualizacion.png", 70, 61)

	local description = updates[#updates].description ~= "" and "Descripción: " .. updates[#updates].description .. "." or ""
	description = description .. "¿Desea instalarla ahora?"

	local text_x, text_y, _, text_h = yesnodialog:text(update_warn, description, 3, 22 * 3, 16, margin)

	local msg = label:new(text_x + margin, text_y + text_h + 10, 454, 18, alertColorTable, "El decodificador se reiniciará automáticamente luego de la instalación.", 14, Alignment.vCenter_hLeft)
	yesnodialog:addChild(update_warn, msg)

	yesnodialog:yesAction(update_warn, accept_update)
	yesnodialog:noAction(update_warn, deny_update)

	yesnodialog:yesLabel(update_warn, "INSTALAR Y REINICIAR", 14)
	yesnodialog:noLabel(update_warn, "NO AHORA", 14)

	Dialog:addWgt("warn", update_warn)
end

function Dialog:onHide()
	log.debug("UpdateDlg", "onHide")
	applying_update = true
	applyUpdates(apply_update)
	return true
end

function Dialog:processKeys(key)
	log.debug("UpdateDlg", "processKeys", "key=" .. tostring(key))
	return wgt:processKey(Dialog:getWgt('warn'), key)
end

-- API

function checkUpdates()
	log.debug("firmupdate", "checkUpdates")
	if (#updates > 0) and (not applying_update) then
		confirmUpdate()
	end
end

function updateReceived( update_info )
	log.debug("firmupdate", "updateReceived", "id=" .. update_info.id .. " description=" .. update_info.description)
	table.insert(updates, update_info)
	confirmUpdate()
end
