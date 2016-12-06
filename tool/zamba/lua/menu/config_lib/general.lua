local log = require 'log'
local timer = require 'timer'
local widget = require 'widget'
local Confirmation = require 'widgets.confirmation'
local style = require 'style'

general = {
	tvOutChanged = false,
	needsRestore = false,
	CompositeVideoContents = {
		'PAL-N'
		,'NTSC'
	},
	CompositeVideoIndex = -1,
	confirmChangeVideoMode = {},
	confirm_counter = nil,
}

local change_video_mode = {
	timeout = 15,
	txt = "Ha cambiado la configuración de video. La misma se revertirá en %d segundos. \n¿Desea conservar los cambios?"
}

local change_canceled_confim = nil

local function help( help_msg )
	return help_msg .. "\nOK = Editar    EXIT = Salir del menú    MENU = Menú ppal."
end

local function revertChangeVideoMode()
	log.info("Menu::Config::AudioVideo", "revertChangeVideoMode", "WorkingTVOut=".. general.WorkingTVOut .. ", WorkingVideoMode=" .. general.WorkingVideoMode)

	combobox:setSelected(general.tvOutCombo, general.WorkingTVOut)

	combobox:setSelected(general.videoCombo, general.WorkingVideoMode)
	changeVideoMode(general.WorkingTVOut, general.WorkingVideoMode)
	general.fillVideoCombo()
end

function change_video_mode.close()
	log.info("Menu::Config::AudioVideo", "close")
	general.WorkingTVOut = combobox:getSelected(general.tvOutCombo)
	general.WorkingVideoMode = combobox:getSelected(general.videoCombo)
end

function change_video_mode.cancel()
	revertChangeVideoMode()
	closeWgt(general.confirmChangeVideoMode)
end


function change_video_mode.create()
	local confirm = Confirmation.new()
	confirm:onYes(change_video_mode.close)
	confirm:onNo(revertChangeVideoMode)
	confirm:onHide(function() general.confirm_counter:cancel() end)

	return confirm
end

local function cancel_tv_out_change()
	general.tvOutChanged = false
	combobox:hideTempItem(general.videoCombo)
	revertChangeVideoMode()
	openSubWin(menu)
end


function general.create(x, y, w, h)
	local win = window:new(x, y, w, h, menuZIndex, menuColorTable)

	general.createCombos(0, 0, w, h)

	change_canceled_confim = Confirmation.new()
	change_canceled_confim:text("No se ha podido realizar el cambio de salida de video ya que no ha seleccionado una resolución. ¿Desea salir de todos modos?")
	change_canceled_confim:onYes(cancel_tv_out_change)
	change_canceled_confim:onNo(function() verticalset:setActiveItem(general.verticalSet, 1) end)


	local connectors,_ = display.getConnectors()
	for ix,c in pairs(connectors) do
		if c=='Video Compuesto' then
			general.CompositeVideoIndex = ix-1
		end
	end

	window:addChild(win, general.verticalSet)
	general.win = win
	wgt:setVisible(general.win, FALSE)

	general.confirmChangeVideoMode = change_video_mode.create()

	return general.win
end

function general.createCombos(x, y, w, h)
	local combo_h = style.combobox.height
	local combo_y = 0
	general.verticalSet = verticalset:new(x, y, w, h, menuColorTable)

	general.tvOutCombo = widget.makeComboWithMaxLines( 0, combo_y, w, combo_h, "Salida de video", {"dummy"})
	combobox:action(general.tvOutCombo, "tvOutChanged")
	combo_y = combo_y + combo_h + 5

	general.videoCombo = widget.makeComboWithMaxLines( 0, combo_y, w, combo_h, "Resolución", {"dummy"})
	combobox:action(general.videoCombo, "videoModeChanged")
	combo_y = combo_y + combo_h + 5

	general.aspectCombo, combo_y = general.createConfigCombo( 0, combo_y, w, combo_h, "Relación de aspecto", display.getAspectModes() )
	combobox:action(general.aspectCombo, "aspectChanged")

	general.audioCombo = widget.makeComboWithMaxLines( 0, combo_y, w, combo_h, "Audio", {"dummy"})
	combobox:action(general.audioCombo, "audioChanged")
	combo_y = combo_y + combo_h + 5

	local title = label:new(10, 0, 175, combo_h, style.combobox.color, "Opacidad del menú", style.font.size, Alignment.vCenter_hLeft)
	general.opacityBar = slidebar.new(0, combo_y, w, combo_h, title, 1, 10, style.combobox.color, 168, 1)
	slidebar:action(general.opacityBar, "opacityChanged")

	wgt:setHelp(general.tvOutCombo, help("Modifica la salida de video."))
	wgt:setHelp(general.videoCombo, help("Modifica la resolución de la salida de video"))
	wgt:setHelp(general.aspectCombo, help("Modifica la relación de aspecto"))
	wgt:setHelp(general.audioCombo, help("Modifica la salida de audio"))
	wgt:setHelp(general.opacityBar, "Modifica la opacidad del menú.\nIzquierda/Derecha = Editar    EXIT = Salir del menú    MENU = Menú ppal.")

	verticalset:addWidget(general.verticalSet, general.tvOutCombo)
	verticalset:addWidget(general.verticalSet, general.videoCombo)
	verticalset:addWidget(general.verticalSet, general.audioCombo)
	verticalset:addWidget(general.verticalSet, general.opacityBar)
	verticalset:circular(general.verticalSet, TRUE)

	general.fillConfigCombos()
end

function general.createConfigCombo( x, y, w, h, text, list)
	local combo = widget.makeComboWithMaxLines( x, y, w, h, text, list)
	local combo_y = y
	if #list > 1 then
		verticalset:addWidget( general.verticalSet, combo )
		combo_y = y + h + 5
	end
	return combo, combo_y
end

function general.fillConfigCombos()
	general.fillConfig( general.tvOutCombo, display.getConnectors() )
	general.fillVideoCombo()
	general.fillConfig( general.aspectCombo, display.getAspectModes() )
	general.fillConfig( general.audioCombo, mixer.getAudioChannels() )
	slidebar:setValue( general.opacityBar, canvas.getTransparency() )

	general.WorkingTVOut = combobox:getSelected(general.tvOutCombo)
	general.WorkingVideoMode = combobox:getSelected(general.videoCombo)
	general.WorkingOpacity = canvas.getTransparency()
end

function general:onSelected()
	log.debug("Menu::Config::General", "onSelected")
	if general.needsRestore then
		general.fillConfigCombos()
		general.needsRestore = false
	end
	combobox:setSelected( general.tvOutCombo, display.getConnector() )
	general.fillVideoCombo()
	combobox:setSelected( general.aspectCombo, display.getAspectMode() )
	combobox:setSelected( general.audioCombo, mixer.getAudioChannel() )

	verticalset:setActiveItem(general.verticalSet, 0)
	updateHelp(wgt:getHelp(general.verticalSet))

	return true
end

function general.onUnselected()
	if general.tvOutChanged then
		openWgt(change_canceled_confim)
		return false
	else
		wgt:processKey(general.verticalSet, KEY_ESCAPE)
		return true
	end
end

function general.fillConfig( combo, list, selected )
	combobox:setItems( combo, list)
	combobox:setSelected( combo, selected )
end

function general.fillVideoCombo()
	local conn = combobox:getSelected(general.tvOutCombo)
	local list, selected = display.listVideoModes(conn)
	if conn == general.CompositeVideoIndex then
		list = general.CompositeVideoContents
	end
	general.fillConfig( general.videoCombo, list, selected )
end

function general.handleKey(key)
	local handled = wgt:processKey(general.verticalSet, key)
	if handled then
		updateHelp(wgt:getHelp(general.verticalSet))
	elseif key == KEY_EXIT or key == KEY_ESCAPE then
		openSubWin(menu)
		handled = true
	end

	return handled
end

function general.confirmSetVideoMode()
	general.confirmChangeVideoMode:text(string.format(change_video_mode.txt, change_video_mode.timeout), 3)
	openWgt(general.confirmChangeVideoMode)
	general.confirm_counter:start()
end

-----------------------------Callbacks---------------------------------------------

function tvOutChanged(changed)
	if changed == TRUE then
		local nextTvOut = combobox:getSelected(general.tvOutCombo)
		log.info("Menu::Config::AudioVideo", "tvOutChanged", "selected=" .. nextTvOut)
		general.fillVideoCombo()
		combobox:setTempItem(general.videoCombo, "Debe configurar esta opción")
		general.tvOutChanged = true
	end
end

function videoModeChanged(changed)
	if general.tvOutChanged or changed==TRUE then
		log.debug("general", "videoModeChanged", "videomode=" .. changed)
		local tv    = combobox:getSelected(general.tvOutCombo)
		local video = combobox:getSelected(general.videoCombo)
		changeVideoMode( tv, video )
		general.confirmSetVideoMode()
	end
	general.tvOutChanged = false
end

function audioChanged(changed)
	if changed == TRUE then
		local selected = combobox:getSelected(general.audioCombo)
		mixer.setAudioChannel(selected)
	end
end

function aspectChanged(changed)
	if changed == TRUE then
		local selected = combobox:getSelected(general.aspectCombo)
		display.setAspectMode( selected )
	end
end

function opacityChanged()
	local transparency = slidebar:getValue(general.opacityBar)
	canvas.setTransparency(transparency)
end

function changeVideoMode( tv, video )
	log.debug("general", "videoModeChanged", "tv=" .. tv .. " video=" .. video)
	display.setVideoMode(tv, video)
end

-- VidoChange Countdown

local function onCountDown( count )
	log.debug("general", "onCountDown", "time remaining: " .. tostring(count))
	general.confirmChangeVideoMode:text(string.format(change_video_mode.txt, count), 3)
end

general.confirm_counter = timer.newCountdown(onCountDown, timer.SECOND, change_video_mode.timeout, change_video_mode.cancel)
