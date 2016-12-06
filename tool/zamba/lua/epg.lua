local log = require "log"
local timer = require "timer"
require "basewidget"

----------------------------------------------------------

---- Layout

local rowH        = 26
local rowSep      = 3
local elementSep  = 10
local textPadding = 6

local EPG_layout = {
	  header = {
		  x = safe_x
		, y = safe_y
		, width  = safe_w
		, height = rowH
		, bg_color = Blue
	}
	, current_video = {}

	, schedule = {
	  	  x = safe_x
		, y = safe_y + rowH + elementSep + rowH + rowSep + rowH + rowSep + 45 + rowSep + 106 + elementSep
	  	, width  = safe_w
	  	, height = 170
	  	, row_height = 20

		, visible_channels = 6
		, visible_slots = 3
		, channel_width = 150
		, max_days = 3
		, slot_minutes = 30

		, scrollbar = {
			width = 20
		}
	}

	, footer = {
	}
}


local EPGView = {
	  view = nil
	, time = nil
	, show_description = {
		  channel = {
			  logo   = nil
			, number = nil
			, name   = nil
		}
		, fav     = nil
		, blocked = nil
		, parental_content = nil
		, parental_age     = nil
		, name        = nil
		, description = nil
	}
	, current_video = {
		  frame = nil
		, title = nil
	}

	, footer = nil
}

local EPGColorTable = {
	bgColor = Color.Gray4,
	textColor = Color.Black
}

local EPGHeader1ColorTable = {
	bgColor = Color.Gray6,
	bgLabelColor = Color.Gray6,
	borderColor = Color.Gray6,
	textColor = Color.White
}

local EPGHeader2ColorTable = {
	bgColor = Color.Gray3,
	borderColor = Color.Gray3,
	textColor = Color.Black
}

local EPGTextAreaColorTable = {
	bgColor = Color.Gray3,
	textColor = Color.Black
}

local EPGTextAreaColorTable2 = {
	textColor = Color.Black
}

local scheduleColorTable = {
	-- Horizontal header colors
	headerColor = Color.Gray6,
	headerTitleColor = Color.None,
	textHeaderColor = Color.White,

	-- Row title colors
	titleColor = Color.Gray6,
	selectedTitleColor = Color.Orange,
	textTitleColor = Color.White,
	selectedTextTitleColor = Color.Black,

	-- Row cell colors
	cellColor = Color.Gray3,
	selectedCellColor = Color.Orange,
	textCellColor = Color.Black,
	selectedTextCellColor = Color.Black,

	-- ScrollBar colors
	scrollColor = Color.Gray3,
	scrollBgColor = Color.Gray6,
}

function EPGView.create()
	local canvasW, canvasH = canvas:size()
	EPGView.view = window:new(0, 0, canvasW, canvasH, menuZIndex, EPGColorTable)

-- Header
	local header_window = window:new(safe_x, safe_y, safe_w, rowH, menuZIndex, EPGHeader1ColorTable)
	local title  = label:new(safe_x, safe_y, safe_w/2, rowH, EPGHeader1ColorTable, "GUÍA DE PROGRAMACIÓN", 16, Alignment.vCenter_hLeft, 0)
	local currentTime = mainWindow.getTime('%d/%m/%Y - %H:%Mhs')
	EPGView.time = label:new(safe_x + safe_w/2, safe_y, safe_w/2, rowH, EPGHeader1ColorTable, currentTime, 16, Alignment.vCenter_hRight, 0)

	window:addChild(header_window, title)
	window:addChild(header_window, EPGView.time)
	window:addChild(EPGView.view, header_window)

-- Show Description
	local descriptionX = safe_x + 240 + elementSep
	local descriptionY = safe_y + rowH + elementSep

	EPGView.show_description.channel.logow    =   window:new(descriptionX,                           descriptionY,                  55, 55, menuZIndex, EPGHeader1ColorTable)
	EPGView.show_description.channel.logo     =    image:new(descriptionX,                       descriptionY, 55, 55, menuZIndex, EPGHeader1ColorTable, "INFO_logoCanal.png")
	EPGView.show_description.channel.number   =    label:new(descriptionX + 2*rowH + 2*rowSep,       descriptionY,                  96,                  rowH, EPGHeader1ColorTable, "", 16, Alignment.vCenter_hLeft, 0)
	EPGView.show_description.channel.name     = textarea:new(descriptionX + 2*rowH + 2*rowSep,       descriptionY + rowH + rowSep, 116 + 4*(rowSep +35), rowH, EPGHeader1ColorTable, "", 17, 1, 0, Alignment.vCenter_hLeft)
	EPGView.show_description.favw             =   window:new(descriptionX + 2*rowH + 3*rowSep +  96, descriptionY,                  38, rowH, menuZIndex, EPGHeader1ColorTable)
	EPGView.show_description.fav              =    image:new(descriptionX + 2*rowH + 3*rowSep +  96 + (38-24)/2, descriptionY + (rowH-20)/2, 24, 20, menuZIndex, EPGHeader1ColorTable, "channels_list_fav_white.png")
	EPGView.show_description.blockedw         =   window:new(descriptionX + 2*rowH + 4*rowSep + 134, descriptionY,                  38, rowH, menuZIndex, EPGHeader1ColorTable)
	EPGView.show_description.blocked          =    image:new(descriptionX + 2*rowH + 4*rowSep + 134 + (38-24)/2, descriptionY + (rowH-20)/2, 24, 20, menuZIndex, EPGHeader1ColorTable, "channels_list_bloq_white.png")
	EPGView.show_description.parental_content =    label:new(descriptionX + 2*rowH + 5*rowSep + 172, descriptionY, 42, rowH, EPGHeader1ColorTable, "", 14, Alignment.vCenter_hCenter, 0)
	EPGView.show_description.parental_age     =    label:new(descriptionX + 2*rowH + 6*rowSep + 214, descriptionY, 42, rowH, EPGHeader1ColorTable, "", 14, Alignment.vCenter_hCenter, 0)
	EPGView.show_description.name             = textarea:new(descriptionX, descriptionY + 2*rowH + 2*rowSep,      55 + rowSep + 104 + 4*(rowSep +38),  45, EPGTextAreaColorTable, "", 14, 2, textPadding, Alignment.vBottom_hLeft)
	EPGView.show_description.description      = textarea:new(descriptionX, descriptionY + 2*rowH + 3*rowSep + 45, 55 + rowSep + 104 + 4*(rowSep +38), 106, EPGTextAreaColorTable, "", 14, 5, textPadding, Alignment.vBottom_hLeft)

	window:addChild(EPGView.show_description.channel.logow, EPGView.show_description.channel.logo)
	window:addChild(EPGView.show_description.favw,          EPGView.show_description.fav)
	window:addChild(EPGView.show_description.blockedw,      EPGView.show_description.blocked)

	window:addChild(EPGView.view, EPGView.show_description.channel.logow)
	window:addChild(EPGView.view, EPGView.show_description.channel.number)
	window:addChild(EPGView.view, EPGView.show_description.channel.name)
	window:addChild(EPGView.view, EPGView.show_description.favw)
	window:addChild(EPGView.view, EPGView.show_description.blockedw)
	window:addChild(EPGView.view, EPGView.show_description.parental_content)
	window:addChild(EPGView.view, EPGView.show_description.parental_age)
	window:addChild(EPGView.view, EPGView.show_description.name)
	window:addChild(EPGView.view, EPGView.show_description.description)

-- Resized video of the current show
	local video_frame_h = rowH + rowSep + rowH + rowSep + 45 + rowSep + 106
	EPG_layout.current_video = {
			  x = safe_x + rowSep
			, y = safe_y + rowH + elementSep + rowSep
			, width  = 240 - 2*rowSep
			, height = rowH + rowSep + rowH + rowSep + 45 + rowSep + 106 - 20 - 2*rowSep
	}
	current_video_frame = window:newFrame(safe_x, safe_y + rowH + elementSep, 240, video_frame_h
	                                    , EPG_layout.current_video.x, EPG_layout.current_video.y, EPG_layout.current_video.width, EPG_layout.current_video.height
	                                    , EPGHeader2ColorTable)
	EPGView.current_video.title = textarea:new(safe_x, safe_y + rowH + elementSep + video_frame_h + 4 - rowH, EPG_layout.current_video.width, rowH, EPGTextAreaColorTable2, "", 14, 1, 0, Alignment.vCenter_hCenter)
	window:addChild(current_video_frame, EPGView.current_video.title)
	window:addChild(EPGView.view, current_video_frame)

-- Schedule
	local schedule_x = safe_x
	local schedule_y = safe_y + rowH + elementSep + rowH + rowSep + rowH + rowSep + 45 + rowSep + 106 + elementSep
	local schedule_width  = safe_w + 4
	local schedule_height = 170
	local schedule_row_height = 20

	EPGView.schedule_window = schedule:new(schedule_x-1,schedule_y, schedule_width, schedule_height, scheduleColorTable)
	schedule:setRowHeight(EPGView.schedule_window, schedule_row_height)
	schedule:setWidths(   EPGView.schedule_window, 150, 133)
	schedule:setRowSep(   EPGView.schedule_window, rowSep)
	schedule:setShowSelectedCallback(   EPGView.schedule_window, "showSelected")
	schedule:setChannelSelectedCallback(EPGView.schedule_window, "channelSelected")
	schedule:setMoreShowsCallback(EPGView.schedule_window, "needMoreShows")
	schedule:scrollImages(EPGView.schedule_window, "arrow_up.png", "arrow_down.png");

	window:addChild(EPGView.view, EPGView.schedule_window)

-- Footer
	EPGView.footer = label:new(safe_x, safe_y + (safe_h - rowH), safe_w, rowH, EPGHeader1ColorTable, "", 14, Alignment.vCenter_hLeft, 0)
	window:addChild(EPGView.view, EPGView.footer)

	wgt:setVisible(EPGView.view, FALSE)
end

EPGView.create()

---- Rendering
local function updateLogo( chID )
	local chInfo = channels.getInfo( chID )
	image:setImage(EPGView.show_description.channel.logo, getLogo(chInfo.logo))
end

function EPG_layout.current_video.render()
	if channel.current() ~= -1 then
		local currCh = channel.current()
		local channel = channels.get(currCh)
		textarea:setText(EPGView.current_video.title, 'Estás viendo: '.. channel.channel .. ' - ' ..channel.name)
		updateLogo(currCh)
	else
		textarea:setText(EPGView.current_video.title, '')
	end
	channel.resize(EPG_layout.current_video.x, EPG_layout.current_video.y, EPG_layout.current_video.width, EPG_layout.current_video.height)
end

function EPG_layout.current_video.hide()
	channel.setFullScreen()
end

local minute_interval = timer.newInterval(function() label:setText(EPGView.time, mainWindow.getTime('%d/%m/%Y - %H:%Mhs')) end, timer.MINUTE)

function renderEPG()
	EPG_layout.footer.update()

	EPG_layout.current_video.render()

	minute_interval:start()
end

function hideEPG()
	minute_interval:stop()
	EPG_layout.current_video.hide()
end

function EPG_layout.footer.update()
	local ch = EPG.schedule.selectedChannel()
	if (ch and channel.current() == ch.id) then
		label:setText(EPGView.footer, 'EXIT = Salir')
	else
		label:setText(EPGView.footer, 'OK = Cambia al canal seleccionado  EXIT = Salir')
	end
end

local ShowBlockedChannels = false

-- Data

EPG = {
	  show_description = {}
	, schedule = {
		  channels = {
			    showsByTime = {
--				  {name = '', startTime='', stopTime=''}
			    } -- end shows of channel
			    , showsByIndex = {
			    } -- end shows of channel (lo mismo pero con otro orden)
		} -- end channels
		, channels_indices = {}
		, selected_channel = 1
		, selected_show    = 1
	}
}

EPG = BaseWidget:new( EPG, "EPG", true)

function getCurrentDate(dayOffset)
	return mainWindow.getDatePlus('%Y-%m-%d', dayOffset)
end

function getFullTime()
	return mainWindow.getTime('%Y-%m-%d %H:%M')
end

local function init()
	log.info("EPG", "init")
	local chCount = 0
	local current_time = getFullTime()
	local titles = {}

	EPG.schedule.channels = {}

	if reset then
		EPG.schedule.selected_channel = 1
	end

	for _, ch in ipairs(channels.getAll()) do
		if canShowChannelData(ch) then
			chCount = chCount + 1
			EPG.schedule.channels[chCount] = {
				   showsByTime = {}
				 , showsByIndex = {}
				 , id = ch.channelID
			}
			EPG.schedule.channels_indices[ch.channelID] = chCount

			if channel.current() == ch.channelID then
				EPG.schedule.selected_channel = chCount
			end

			table.insert(titles, ch.name)
		end

	end

	local lenght = 60 * 24 * EPG_layout.schedule.max_days / EPG_layout.schedule.slot_minutes
	schedule:initSlots(EPGView.schedule_window, current_time, EPG_layout.schedule.slot_minutes, lenght)
	schedule:setRows(EPGView.schedule_window, titles, EPG_layout.schedule.visible_channels)
	schedule:resetSlots(EPGView.schedule_window)
	wgt:fixOffsets(EPGView.schedule_window)
end

function getVisibleShows(channelID, time_from, time_to)
	if not time_from then
		time_from, time_to = schedule:getVisibleTimeRange(EPGView.schedule_window)
	end
	log.trace( "EPG", "getVisibleShows", "id="..channelID .. ", from="..time_from..", to="..time_to)

	local channelIx = EPG.schedule.channels_indices[channelID]
	if channelIx then -- can be a One-Seg channel when they aren't visible
		local channel = channels.get(channelID)
		if canShowChannelData(channel) then -- only get data for unblocked channels
			local shows = channels.getShowsBetween( channel.channelID, time_from, time_to )
			for _,show in ipairs(shows) do
				log.trace( "EPG", "getVisibleShows", channel.name..", "..show.name.. ", " .. show.description .. " (".. fullTime(show.startDate) .. " - ".. fullTime(show.stopDate) ..")")
				local current_show = EPG.schedule.channels[channelIx].showsByTime[fullTime(show.startDate)]
				if not current_show then
					log.trace( "EPG", "getVisibleShows add new show")
					EPG.schedule.channels[channelIx].showsByTime[fullTime(show.startDate)] = show
					table.insert(EPG.schedule.channels[channelIx].showsByIndex, show)

					schedule:addCell(EPGView.schedule_window, channelIx-1, fullTime(show.startDate)..":00", fullTime(show.stopDate)..":00", show.name, #EPG.schedule.channels[channelIx].showsByIndex)
				elseif not l_util.table.equals(current_show, show) then
					log.trace( "EPG", "getVisibleShows EPG changed, needs reinit")
					init()
				end
			end

		end -- end not blocked
	end
end

local function populate()
	log.trace("EPG", "populate", string.format("time=%s", getFullTime()) )

	local sTimeFrom, sTimeTo = schedule:getVisibleTimeRange(EPGView.schedule_window)
	getShows(sTimeFrom, sTimeTo)

	schedule:moveSlotsTo(EPGView.schedule_window, sTimeFrom)
	schedule:setCurrent( EPGView.schedule_window, EPG.schedule.selected_channel-1)
end

local update = timer.newInterval(function()
	log.trace("EPG", "Update")
	populate()
end, 5 * timer.SECOND)

-- Data filling
function fillEPG()
	init()
	populate()

	EPG.show_description.clear() -- clear any previous data
	EPG.show_description.fill()
end

function zapperOnTimeChanged( isValid )
	log.trace("EPG", "zapperOnTimeChanged", string.format("isValid=%d", isValid))
	if isValid == TRUE then
		label:setText(EPGView.time, mainWindow.getTime('%d/%m/%Y - %H:%Mhs'))
		fillEPG()
	end
end

function zapperOnSessionChanged( isActive )
	log.trace("EPG", "zapperOnSessionChanged", string.format("isActive=%s, EPG.visible=%s", tostring(isActive), tostring(EPG.visible)))
	if EPG.visible then
		fillEPG()
	end
end

function canShowChannelData(channel)
	return (not channels.isProtected(channel.channelID)) or ShowBlockedChannels
end

function EPG.schedule.selectedChannel()
	return EPG.schedule.channels[EPG.schedule.selected_channel]
end

function EPG.show_description.fill()
	local channel = EPG.schedule.selectedChannel()
	if channel then
		local chInfo = channels.getInfo(channel.id)

		if canShowChannelData(chInfo) then
			wgt:setVisible(EPGView.show_description.channel.logo, TRUE)
			textarea:setText(EPGView.show_description.channel.name, chInfo.name)
			updateLogo(channel.id)
		else
			wgt:setVisible(EPGView.show_description.channel.logo, FALSE)
			textarea:setText(EPGView.show_description.channel.name, '')
		end
		label:setText(EPGView.show_description.channel.number, chInfo.channel)

		wgt:setVisible(EPGView.show_description.fav,     chInfo.favorite)
		wgt:setVisible(EPGView.show_description.blocked, chInfo.blocked)
		wgt:repaint(EPGView.show_description.favw)
		wgt:repaint(EPGView.show_description.blockedw)
		label:setText(EPGView.show_description.parental_content, chInfo.parentalContent)
		label:setText(EPGView.show_description.parental_age,     chInfo.parentalAge)

		if (0 < EPG.schedule.selected_show and EPG.schedule.selected_show <= #EPG.schedule.selectedChannel().showsByIndex) and canShowChannelData(chInfo) then
			local show = EPG.schedule.selectedChannel().showsByIndex[EPG.schedule.selected_show]
			textarea:setText(EPGView.show_description.name, show.name .. '\n' .. show.startDate.time .. ' a ' .. show.stopDate.time .. 'hs.')
			textarea:setText(EPGView.show_description.description, show.description)
		else
			textarea:setText(EPGView.show_description.name, '')
			textarea:setText(EPGView.show_description.description, '')
		end
	end
end

function EPG.show_description.clear()
	wgt:setVisible(EPGView.show_description.channel.logo, FALSE)
	label:setText(EPGView.show_description.channel.number, '')
	textarea:setText(EPGView.show_description.channel.name, '')
	wgt:setVisible(EPGView.show_description.fav,     FALSE)
	wgt:setVisible(EPGView.show_description.blocked, FALSE)
	wgt:repaint(EPGView.show_description.favw)
	wgt:repaint(EPGView.show_description.blockedw)
	label:setText(EPGView.show_description.parental_content, '')
	label:setText(EPGView.show_description.parental_age,     '')
	textarea:setText(EPGView.show_description.name, '')
	textarea:setText(EPGView.show_description.description, '')
end

function getShows(timeFrom, timeTo, chFrom, chTo)
	log.debug("EPG", "getShows", string.format("timeFrom=%s, timeTo=%s", timeFrom, timeTo))
	if not chFrom then
		chFrom,chTo = schedule:getVisibleRowRange(EPGView.schedule_window)
	end

	for ix = chFrom, chTo do
		if EPG.schedule.channels[ix+1] then -- if it's not an empty row
			getVisibleShows(EPG.schedule.channels[ix+1].id, timeFrom, timeTo)
		end
	end
end

function fullTime(obj)
	return obj.day..' '..obj.time
end

-- GUI

function unlockNeeded()
	for ix, ch in ipairs(channels.getAll()) do
		if channels.isProtected(ch.channelID) then
			return true
		end
	end
	return false
end

function openEPG()
	log.debug( "EPG", "openEPG")
	middleware.lock(1)
	if unlockNeeded() and not PINDlg.visible then
		log.debug( "EPG", "openEPG", "unlockNeeded")
		PINDlg.init("Ingrese la clave para ver los canales bloqueados", dialogZIndex, EPG.showBlocked, EPG.hideBlocked)
		openWgt(PINDlg)
	else
		EPG.hideBlocked() -- don't show data of blocked channels, just in case, because there aren't any
	end
end

function EPG:onShow()
	wgt:open(EPGView.view)
	update:start()

	fillEPG()
	renderEPG()
	return true
end

function EPG:onHide()
	wgt:close(EPGView.view)
	update:stop()
	hideEPG()
	middleware.lock(0)
	checkUpdates()
	return true
end

function EPG.showBlocked()
	ShowBlockedChannels = true
	openWgt(EPG)
end

function EPG.hideBlocked()
	ShowBlockedChannels = false
	openWgt(EPG)
end

function EPG.onOk()
	if #EPG.schedule.channels > 0 then
		local id = EPG.schedule.selectedChannel().id
		if (channel.current() ~= id) then
			closeWgt(EPG)
			view.refresh()
			changeChannel(id, false)
		end
	end
	return true
end

function EPG.onEPG()
	closeWgt(EPG)
	return true
end

function EPG.onGreen()
	closeWgt(EPG)
	return true
end

-- Callbacks
function showSelected(show_index)
	EPG.schedule.selected_show = show_index
	EPG.show_description.clear()
	EPG.show_description.fill()
end

function channelSelected(channel_index)
	EPG.schedule.selected_channel = channel_index+1
	EPG_layout.footer.update()
end

function needMoreShows(row_index)
	local time_from, time_to = schedule:getNextVisibleTimeRange(EPGView.schedule_window, 2)
	getShows(time_from, time_to)
end

-- End callbacks

EPG:ignore({
	'onInfo',
	'onChannelList',
	'onDown',
	'onLeft',
	'onUp',
	'onMenu',
	'onAudio',
	'onRecall',
	'onFav',
	'onSub',
	'onAspect',
	'onTVmode',
	'onChannel',
	'onDigit',
	'onChannelUp',
	'onChannelDown',
})

function EPG:processKeys(key)
	if (key == KEY_INFO) then
		schedule:dumpShows(EPGView.schedule_window)
		return true
	end

	return wgt:processKey(EPGView.schedule_window, key)
end
