local log = require 'log'
local style = require 'style'
local widget = require 'widget'

update_cfg = {}

function update_cfg.create(x, y, w, h)
	local win = window:new( x, y, w, h, menuZIndex, menuColorTable )
	wgt:setVisible( win, FALSE )

	update_cfg.verticalSet = verticalset:new( 0, 0, w, h, menuColorTable )
	verticalset:circular( update_cfg.verticalSet, TRUE )

	update_cfg.otaUpdate = widget.makeComboWithMaxLines(0, (style.combobox.height + 5) * 2, w, style.combobox.height, "Actualización OTA", {"Deshabilitado", "Habilitado"})
	combobox:action(update_cfg.otaUpdate, "otaUpdateChanged")
	wgt:setHelp( update_cfg.otaUpdate, "Modifica la actualización por aire.\nOK = Editar    EXIT = Salir del menú    MENU = Menú ppal.")
	verticalset:addWidget( update_cfg.verticalSet, update_cfg.otaUpdate )

	update_cfg.netUpdate = widget.makeComboWithMaxLines(0, ((style.combobox.height + 5 ) * 3) + 15, w, style.combobox.height, "Actualización por red", {"Deshabilitado", "Habilitado"})
	combobox:action(update_cfg.netUpdate, "netUpdateChanged")
	wgt:setHelp( update_cfg.netUpdate, "Modifica la actualización por red.\nOK = Editar    EXIT = Salir del menú    MENU = Menú ppal.")
	verticalset:addWidget( update_cfg.verticalSet, update_cfg.netUpdate )

	window:addChild(win, update_cfg.verticalSet)

	update_cfg.win = win
	
	return update_cfg.win
end

function update_cfg:onSelected()
	verticalset:setActiveItem(update_cfg.verticalSet, 0)
	combobox:setSelected(update_cfg.otaUpdate, update.isEnabled("OTA") and 1 or 0)
	combobox:setSelected(update_cfg.netUpdate, update.isEnabled("Network") and 1 or 0)
	updateHelp(wgt:getHelp(update_cfg.verticalSet))
	return true
end

function update_cfg.handleKey(key)
	local handled = wgt:processKey(update_cfg.verticalSet, key)
	if handled and key ~= KEY_OK and key ~= KEY_ENTER then
		updateHelp(wgt:getHelp(update_cfg.verticalSet))
	elseif (not handled) and (key == KEY_EXIT or key == KEY_ESCAPE) then
		openSubWin(menu)
		handled = true
	end

	return handled
end

function update_cfg.onUnselected()
	wgt:processKey( update_cfg.verticalSet, KEY_ESCAPE )
	return true
end

function otaUpdateChanged( changed )
	if changed == TRUE then
		local option = combobox:getSelected( update_cfg.otaUpdate )
		update.enable( "OTA", option == 1 )
	end
end

function netUpdateChanged( changed )
	if changed == TRUE then
		local option = combobox:getSelected( update_cfg.netUpdate )
		update.enable( "Network", option == 1 )
	end
end
