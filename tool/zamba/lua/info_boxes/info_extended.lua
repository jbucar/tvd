require "info_boxes.common_info"
require "info_boxes.info_standard"
local log = require "log"
local timer = require 'timer'
local progressbar = require "widgets.progressbar"
require "basewidget"

Info_Extended = {
		w = 620,
		h = 420
}

local x = 50
local y = 50
local label_h = 14
local fontSize = 14

local function parseParentalStr( str )
	local description = ""
	if string.find(str, "S") then
		description = "Sexo "
	end
	if string.find(str, "D") then
		description = description .. "Droga "
	end
	if string.find(str, "V") then
		description = description .. "Violencia "
	end

	if description == "" then
		return getText(str)
	else
		description = description:sub( 1, description:len() - 1 )
		description = description:gsub( " ", ", " )
		return description
	end
end

function createExtendedInfoBoxes()
	Info_Extended.create()
	Info_Standard.create()
end

local function update()
	Info_Extended.refreshData(channel.current())
	updateSignalData()
end

function Info_Extended.create()
	Info_Extended = BaseWidget:new( Info_Extended, "Info_Extended", true, true )
	Info_Extended:ignore(info_ignored_actions)

	createExtended(Info_Extended, true)
	createDescription(Info_Extended, 344, 5)

	Info_Extended.timerInterval = timer.newInterval(update, 1000 )

	local init_x = 74
	local init_y = 119
	local sep = 29
	local w = Info_Extended.w

	Info_Extended.contentLabel = Info_Extended.second_layout:add_field("CONTENIDO", 84)

	local qualityHeaderLabel = label:new( w - 183, 125, 50, 23, menuColorTable, "Calidad", fontSize, Alignment.vCenter_hLeft )
	local intensityHeaderLabel = label:new( w - 100, 125, 70, 23, menuColorTable, "Intensidad", fontSize, Alignment.vCenter_hLeft )
	local servicesLabel = label:new( w - 183, 234, 171, 23, labelsColorTable, "SERVICIOS EXTRAS", fontSize, Alignment.vCenter_hLeft )
	
	Info_Extended.signalLabel = label:new( w - 183, 102, 165, 20, labelsColorTable, "", fontSize, Alignment.vTop_hLeft )
	Info_Extended.qualityLabel = label:new( w - 183, 150, 50, 23, menuColorTable, "", fontSize, Alignment.vCenter_hCenter )
	Info_Extended.intensityLabel = label:new( w - 100, 150, 69, 23, menuColorTable, "", fontSize, Alignment.vCenter_hCenter )
	

	local qualityBarView = progressbar.views.Vertical.new( w - 179 + x, 170 + y, 42, 52, 2, 2)
	local intensityBarView = progressbar.views.Vertical.new( w - 86 + x, 170 + y, 42, 52, 2, 2)
	local qualityBar = progressbar.new(qualityBarView)
	local intensityBar = progressbar.new(intensityBarView)

	Info_Extended:addWgt( 'qualityBar', qualityBar )
	Info_Extended:addWgt( 'intensityBar', intensityBar )
	
	local win = Info_Extended:getWgt( 'win' )
	
	Info_Extended.createServices( win )
	
	window:addChild( win, qualityHeaderLabel )
	window:addChild( win, intensityHeaderLabel )
	window:addChild( win, servicesLabel )
	window:addChild( win, Info_Extended.signalLabel )
	window:addChild( win, Info_Extended.qualityLabel )
	window:addChild( win, Info_Extended.intensityLabel )

	Info_Extended.createHelp( win )

	wgt:fixOffsets( win )
end

function Info_Extended.createHelp( win )
	Info_Extended.exitLabel = label:new( 56, 430, 98, 20, labelsColorTable, "Botón \'INFO\' =", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.exitValue = label:new( 144, 430, 30, 20, menuColorTable, "Salir", fontSize, Alignment.vCenter_hLeft )
	window:addChild( win, Info_Extended.exitLabel )
	window:addChild( win, Info_Extended.exitValue )

	Info_Extended.ccHelpLabel = label:new( 0, 0, 82, 20, labelsColorTable, "Botón \'CC\' =", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.ccHelpValue = label:new( 0, 0, 206, 20, menuColorTable, "Mostrar/Ocultar Closed Caption", fontSize, Alignment.vCenter_hLeft )
	wgt:setVisible( Info_Extended.ccHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.ccHelpValue, FALSE )
	window:addChild( win, Info_Extended.ccHelpLabel )
	window:addChild( win, Info_Extended.ccHelpValue )

	Info_Extended.audioHelpLabel = label:new( 0, 0, 104, 20, labelsColorTable, "Botón \'Audio\' =", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.audioHelpValue = label:new( 0, 0, 94, 20, menuColorTable, "Cambiar audio", fontSize, Alignment.vCenter_hLeft )
	wgt:setVisible( Info_Extended.audioHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.audioHelpValue, FALSE )
	window:addChild( win, Info_Extended.audioHelpLabel )
	window:addChild( win, Info_Extended.audioHelpValue )

	Info_Extended.videoHelpLabel = label:new( 0, 0, 84, 20, labelsColorTable, "Botón \'AV\' =", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.videoHelpValue = label:new( 0, 0, 100, 20, menuColorTable, "Cambiar video", fontSize, Alignment.vCenter_hLeft )
	wgt:setVisible( Info_Extended.videoHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.videoHelpValue, FALSE )
	window:addChild( win, Info_Extended.videoHelpLabel )
	window:addChild( win, Info_Extended.videoHelpValue )

	Info_Extended.separator = {
		current = 0
	}
	Info_Extended.separator[1] = label:new( 0, 0, 15, 20, dateColorTable, "|", fontSize, Alignment.vCenter_hLeft )
	window:addChild( win, Info_Extended.separator[1] )
	wgt:setVisible( Info_Extended.separator[1], FALSE )

	Info_Extended.separator[2] = label:new( 0, 0, 15, 20, dateColorTable, "|", fontSize, Alignment.vCenter_hLeft )
	window:addChild( win, Info_Extended.separator[2] )
	wgt:setVisible( Info_Extended.separator[2], FALSE )
end

function Info_Extended.createServices( win )
	local w = Info_Extended.w
	Info_Extended.emptyServicesLabel = label:new( w - 183, 270, 171, 20, menuColorTable, "- NO ENCONTRADOS", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.gingaLabel = label:new( w - 183, 270, 171, 20, menuColorTable, "- APLICACIÓN GINGA.AR", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.ccLabel = label:new( w - 183, 291, 171, 20, menuColorTable, "- CLOSED CAPTION", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.audioServiceLabel = label:new( w - 183, 312, 171, 20, menuColorTable, "- MULTIPLES AUDIOS", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.videoServiceLabel = label:new( w - 183, 334, 171, 20, menuColorTable, "- MULTIPLES VIDEOS", fontSize, Alignment.vCenter_hLeft )
	Info_Extended.subsLabel = label:new( w - 183, 354, 171, 20, menuColorTable, "- SUBTíTULOS", fontSize, Alignment.vCenter_hLeft )
	wgt:setVisible( Info_Extended.emptyServicesLabel, FALSE )
	wgt:setVisible( Info_Extended.gingaLabel, FALSE )
	wgt:setVisible( Info_Extended.ccLabel, FALSE )
	wgt:setVisible( Info_Extended.audioServiceLabel, FALSE )
	wgt:setVisible( Info_Extended.videoServiceLabel, FALSE )
	wgt:setVisible( Info_Extended.subsLabel, FALSE )
	window:addChild( win, Info_Extended.emptyServicesLabel )
	window:addChild( win, Info_Extended.gingaLabel )
	window:addChild( win, Info_Extended.ccLabel )
	window:addChild( win, Info_Extended.audioServiceLabel )
	window:addChild( win, Info_Extended.videoServiceLabel )
	window:addChild( win, Info_Extended.subsLabel )
end

local function showSep( x, y )
	local i = Info_Extended.separator.current + 1
	wgt:position( Info_Extended.separator[i], x, y )
	wgt:setVisible( Info_Extended.separator[i], TRUE )
	local w, _ = wgt:size( Info_Extended.separator[i] )
	Info_Extended.separator.current = i
	return x+w, y
end

local function updateExitLabel( help_x, help_y, needSep )
	local sw, sh = wgt:size( Info_Extended.separator[1] )
	local lw, lh = wgt:size( Info_Extended.exitLabel )
	local vw, vh = wgt:size( Info_Extended.exitValue )
	local totalWidth = help_x + sw + lw + vw
	if needSep then
		if totalWidth<Info_Extended.w then
			help_x, help_y = showSep( help_x, help_y )
		else
			help_y = help_y + sh
			help_x = 56
		end
	end
	wgt:position( Info_Extended.exitLabel, help_x, help_y )
	wgt:position( Info_Extended.exitValue, help_x+lw, help_y )
end

local function updateLabels( show, label, x, y, hlabel, hvalue, hx, hy, needSep )
	local tmp_y = y
	local visible = show and TRUE or FALSE
	if visible == TRUE then
		local ww, wh = wgt:size( label )
		wgt:position( label, x, y )
		tmp_y = y + wh

		if hlabel and hvalue then
			local lw, lh = wgt:size( hlabel )
			local vw, vh = wgt:size( hvalue )

			if lw+vw+hx < Info_Extended.w then
				if needSep then
					hx, hy = showSep( hx, hy )
				end
			else
				hx = 56
				hy = hy + lh
			end
			needSep = true

			wgt:position( hlabel, hx, hy )
			hx = hx + lw
			wgt:position( hvalue, hx, hy )
			hx = hx + vw
		end
	end

	wgt:setVisible( label, visible )
	if (hlabel ~= nil) and (hvalue ~= nil) then
		wgt:setVisible( hlabel, visible )
		wgt:setVisible( hvalue, visible )
	end

	return tmp_y, hx, hy, needSep
end

local function updateServices()
	local init_x = Info_Extended.w - 183 + x
	local init_y = 260 + y
	local help_x = 56
	local help_y = 430

	Info_Extended.separator.current = 0
	wgt:setVisible( Info_Extended.separator[1], FALSE )
	wgt:setVisible( Info_Extended.separator[2], FALSE )
	wgt:setVisible( Info_Extended.ccHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.ccHelpValue, FALSE )
	wgt:setVisible( Info_Extended.audioHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.audioHelpValue, FALSE )
	wgt:setVisible( Info_Extended.videoHelpLabel, FALSE )
	wgt:setVisible( Info_Extended.videoHelpValue, FALSE )

	local hasCC = channel.haveSubtitle() == "CC"
	local hasAudio = channel.getAudioCount() > 1
	local hasVideo = channel.getVideoCount() > 1
	local hasGinga = middleware.haveApplications() == TRUE
	local hasServices = not (hasCC or hasAudio or hasVideo or hasGinga)

	local needSep = false
	init_y, help_x, help_y, needSep = updateLabels( hasServices, Info_Extended.emptyServicesLabel, init_x, init_y, nil, nil, help_x, help_y, needSep )
	init_y, help_x, help_y, needSep = updateLabels( hasGinga, Info_Extended.gingaLabel, init_x, init_y, nil, nil, help_x, help_y, needSep )
	init_y, help_x, help_y, needSep = updateLabels( hasCC, Info_Extended.ccLabel, init_x, init_y, Info_Extended.ccHelpLabel, Info_Extended.ccHelpValue, help_x, help_y, needSep )
	init_y, help_x, help_y, needSep = updateLabels( hasAudio, Info_Extended.audioServiceLabel, init_x, init_y, Info_Extended.audioHelpLabel, Info_Extended.audioHelpValue, help_x, help_y, needSep )
	init_y, help_x, help_y, needSep = updateLabels( hasVideo, Info_Extended.videoServiceLabel, init_x, init_y, Info_Extended.videoHelpLabel, Info_Extended.videoHelpValue, help_x, help_y, needSep )
	updateExitLabel( help_x, help_y, needSep )
end

function updateSignalData()
	log.debug("Info_Extended", "updateSignalData")
	local quality, snr = channel:status()
	label:setText( Info_Extended.qualityLabel, tostring(snr) .. "%" )
	label:setText( Info_Extended.intensityLabel, tostring(quality) .. "%" )

	local qualityBar  = Info_Extended:getWgt( 'qualityBar' )
	local intensityBar  = Info_Extended:getWgt( 'intensityBar' )
	qualityBar:value(snr / 100)
	intensityBar:value(quality / 100)
end

function Info_Extended.onShow()
	log.debug("Info_Extended", "refresh")

	local ch = channel.current()
	if ch ~= -1 then
		Info_Extended.refreshData(ch)
		label:setText( Info_Extended.signalLabel, "SEÑAL (" .. channel:currentNetworkName() .. "Mhz)" )
		updateSignalData()
		Info_Extended.timerInterval:start()
		return true
	else
		return false
	end
end

function Info_Extended.onHide()
	Info_Extended.timerInterval:stop()
	return true
end

function Info_Extended.refreshData(ch)
	local info = channels.getInfo(ch)
	refreshCommonData( Info_Extended, info )
	refreshCommonExtended( Info_Extended, info )
	updateServices()

	label:setText( Info_Extended.contentLabel, parseParentalStr( info.parentalContent ) )
	view:refresh()
end

function Info_Extended:processKeys(key)
	return false
end

function Info_Extended.onInfo()
	closeWgt( Info_Extended )
	return true
end
