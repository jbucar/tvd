local widget = require 'widget'
local style = require 'style'
local security = require 'security'

local ages = {
	[0] = 0, -- ATP
	[1] = 18,
	[2] = 16,
	[3] = 14,
	[4] = 12,
	[5] = 10,
}

parental = {
	  vertical_set  = nil
	, ageCombo      = nil
	, violenceCombo = nil
	, sexCombo      = nil
	, drugsCombo    = nil
}

local function age_index()
	local age = control.getParentalAge()
	for index, item in pairs(ages) do
		if item == age then return index end
	end
end

local function makeCombo(x, y, w, h, title, items)
	local items_width = 200
	return widget.makeCombo(x, y, w, h, title, items, items_width)
end

function parental.create(x, y, w, h)
	local win = window:new( x, y, w, h, menuZIndex, menuColorTable )

	local COMBO_X = 21
	local COMBO_Y = 26
	local COMBO_W = w - 2 * ( config.page_border + config.page_margin)
	local COMBO_H = 26

	local ageTitle     = label:new(COMBO_X, COMBO_Y,                   200, COMBO_H, menuColorTable, "Por edad", style.font.size, Alignment.vBottom_hLeft)
	local contentTitle = label:new(COMBO_X, COMBO_Y + (COMBO_H + 5)*2, 200, COMBO_H, menuColorTable, "Por contenidos", style.font.size, Alignment.vBottom_hLeft)

	parental.ageCombo      = makeCombo(COMBO_X, COMBO_Y + (COMBO_H + 5)  , COMBO_W, COMBO_H, "Bloquear por edad",       {"No", "Para mayores de 18 años", "Para mayores de 16 años", "Para mayores de 14 años", "Para mayores de 12 años", "Para mayores de 10 años"})
	combobox:action(parental.ageCombo, "ageChanged")
	parental.violenceCombo = makeCombo(COMBO_X, COMBO_Y + (COMBO_H + 5)*3, COMBO_W, COMBO_H, "Contenidos de Violencia", {"Permitir","Bloquear"})
	combobox:action(parental.violenceCombo, "parentalChanged")
	parental.sexCombo      = makeCombo(COMBO_X, COMBO_Y + (COMBO_H + 5)*4, COMBO_W, COMBO_H, "Contenidos de Sexo",      {"Permitir","Bloquear"})
	combobox:action(parental.sexCombo, "parentalChanged")
	parental.drugsCombo    = makeCombo(COMBO_X, COMBO_Y + (COMBO_H + 5)*5, COMBO_W, COMBO_H, "Contenidos de Drogas",    {"Permitir","Bloquear"})
	combobox:action(parental.drugsCombo, "parentalChanged")

	local set = verticalset:new(0, 0, 412, 250, menuColorTable)
	verticalset:circular(set, TRUE)

	verticalset:addChild(set, parental.ageCombo)
	verticalset:addChild(set, parental.violenceCombo)
	verticalset:addChild(set, parental.sexCombo)
	verticalset:addChild(set, parental.drugsCombo)

	-- Not selectable
	verticalset:addChild(set, ageTitle, FALSE)
	verticalset:addChild(set, contentTitle, FALSE)

	window:addChild(win, set)

	verticalset:setActiveItem(set, 0)

	wgt:setVisible( win, FALSE )

	parental.vertical_set = set
	parental.win = win

	return parental.win
end

function parental:onSelected()
	updateHelp("Modifica la configuración del filtro parental.\nOK = Editar   EXIT/MENU = Salir del menú")
	-- Set combobox values
	combobox:setSelected(parental.ageCombo, age_index()) -- Parental Age
	-- Parental Content
	combobox:setSelected(parental.violenceCombo, control.isViolenceRestricted() and 1 or 0)
	combobox:setSelected(parental.sexCombo, control.isSexRestricted() and 1 or 0)
	combobox:setSelected(parental.drugsCombo, control.isDrugsRestricted() and 1 or 0)

	verticalset:setActiveItem(parental.vertical_set, 0)

	return true
end

function parental:onUnselected()
	wgt:processKey(parental.vertical_set, KEY_ESCAPE)
	return true
end

function parental.handleKey(key)
	local handled = wgt:processKey(parental.vertical_set, key)
	if (not handled) and (key == KEY_EXIT or key == KEY_ESCAPE) then
		openSecurity()
		handled = true
	end
	return handled
end

local function change_parental()
	local violence = combobox:getSelected(parental.violenceCombo) == 1
	local sex = combobox:getSelected(parental.sexCombo) == 1
	local drugs = combobox:getSelected(parental.drugsCombo) == 1

	control.setViolenceRestriction(violence)
	control.setDrugsRestriction(drugs)
	control.setSexRestriction(sex)
end

local function change_parental_age()
	local parental_age = ages[combobox:getSelected(parental.ageCombo)]
	control.setParentalAge(parental_age)
end

function parentalChanged(changed)
	if changed == TRUE then
		security.protected(change_parental, function() openSubWin(menu) end)
	end
end

function ageChanged(changed)
	if changed == TRUE then
		security.protected(change_parental_age, function() openSubWin(menu) end)
	end
end
