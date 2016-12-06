-------------------------------------------------------------------------------
-- EPG WINDOW
-------------------------------------------------------------------------------
EPG_BLOCK_TIME_GAP = 900

-- REL means 'relative' to corner of EPG Window

local EPG_WINDOW_X = 62 
local EPG_WINDOW_Y = 65
local EPG_WINDOW_W = 598
local EPG_WINDOW_H = 453
local EPG_NO_CHANNEL_AVAILABLE = "Sin canal"
local EPG_CURRENTLY_SEEING_X = 14
local EPG_CURRENTLY_SEEING_Y = 66
local EPG_CURRENTLY_SEEING_START = "Estás viendo: "
local EPG_CURRENTLY_SEEING_FONTSIZE = 13
local EPG_CURRENTLY_SEEING_FONTCOLOR = {r = 0, g = 0, b = 0, a = 255}
local EPG_CURRENTLY_SEEING_MAX_WIDTH = 285

local EPG_BACKGROUND_IMAGE = "FondoEPG.png"

local EPG_DEFAULT_LOGO_IMAGE = "INFO_logoCanal.png"

local EPG_SELECTED_CHANNEL_NAME_X = 337
local EPG_SELECTED_CHANNEL_NAME_Y = 109
local EPG_SELECTED_CHANNEL_DEFAULT_NAME = "Ningún canal seleccionado"
local EPG_SELECTED_CHANNEL_NAME_FONTSIZE = 16
local EPG_SELECTED_CHANNEL_NAME_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255}
local EPG_SELECTED_CHANNEL_MAX_WIDTH = 230
										  
local EPG_SELECTED_SHOW_SCHEDULE_LBL_X = 277 
local EPG_SELECTED_SHOW_SCHEDULE_LBL_Y = 160
local EPG_SELECTED_SHOW_SCHEDULE_DEFAULT_LBL = " "
local EPG_SELECTED_SHOW_SCHEDULE_LBL_FONTSIZE = 13
local EPG_SELECTED_SHOW_SCHEDULE_LBL_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255}

local EPG_DESCRIPTION_WIDTH = 292
local EPG_DESCRIPTION_HEIGHT = 60
local EPG_DESCRIPTION_TEXT_REL_X = 277 
local EPG_DESCRIPTION_TEXT_REL_Y = 184
local EPG_DESCRIPTION_TEXT_FONTSIZE = 12
local EPG_DESCRIPTION_TEXT_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255} 

local EPG_SCHEDULE_REL_X = 13
local EPG_SCHEDULE_REL_Y = 269
local EPG_SCHEDULE_WIDTH = 575
local EPG_SCHEDULE_HEIGHT = 169
local EPG_SCHEDULE_NBLOCKS_WIDTH = 6 -- Number of blocks per row
local EPG_SCHEDULE_NBLOCKS_HEIGHT = 6 --4 -- Number of blocks per column
local EPG_SCHEDULE_NBLOCKS_HOUR_DIVISION = 3

local EPG_CLOCK_REL_X = 528
local EPG_CLOCK_REL_Y = 23
local EPG_CLOCK_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255}
local EPG_CLOCK_FONTSIZE = 13

local EPG_DATE_REL_X = 334
local EPG_DATE_REL_Y = 23
local EPG_DATE_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255}
local EPG_DATE_FONTSIZE = 13

local EPG_AGE_RATING_REL_X = 537
local EPG_AGE_RATING_REL_Y = 160
local EPG_AGE_RATING_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255} 
local EPG_AGE_RATING_FONTSIZE = 13

local EPG_DEFAULT_DESCRIPTION_TEXT = "No hay descripción disponible para este programa"
local EPG_DEFAULT_AGE_RATE_TEXT = " " 

local EPG_SELECTED_SHOW_NAME_X = 277
local EPG_SELECTED_SHOW_NAME_Y = 144
local EPG_SELECTED_SHOW_NAME_FONTSIZE = 14
local EPG_SELECTED_SHOW_NAME_FONTCOLOR = {r = 255, g = 255, b = 255, a = 255}
local EPG_SELECTED_SHOW_DEFAULT_NAME = "Sin información"
local EPG_SELECTED_SHOW_MAX_WIDTH = 285

local EPG_VIDEO_CAPTION_RECT = {x = 14, y = 77, w = 190, h = 152} -- Relative to EPG corner

local EPG_CHANNEL_LOGO_RECT = { x = 277, y = 71, w = 36, h = 44  }

local EPG_LABEL_FILLER = "..."

EPGWindow = wndNew( 'EPG window', RootWindow, EPG_WINDOW_X, EPG_WINDOW_Y )

function lblTextWithMaxWidth(aLabel, aText, aMaxWidth)
	local fittingWidth = string.len(aText)
	local w,h,a = measureText(aText, aLabel.fontSize)
	if ( w <= aMaxWidth) then
		lblText(aLabel, aText)
		return
	end
	while (fittingWidth > 0) do
		fittingWidth = fittingWidth - 1
		local textToFit = string.sub( aText, 1, fittingWidth ) .. EPG_LABEL_FILLER 
		local w,h,a = measureText(textToFit, aLabel.fontSize)
		if (w <= aMaxWidth) then
			lblText(aLabel,textToFit)
			return
		end
	end
	lblText(aLabel, " ")
end



function EPGWindow.actions.onCreate( win )
	logMethod( TRACE, 'onCreate', win['name'] )

	win.actions.onShow = EPGOnShow
	win.actions.onHide = EPGOnClose
	win.actions.goUp = EPGWindow.goUp
	win.actions.goDown = EPGWindow.goDown
	win.actions.goLeft = EPGWindow.goLeft
	win.actions.goRight = EPGWindow.goRight
	win.actions.tuneCurrentChannel = EPGWindow.tuneCurrentChannel
	buildEPGBackground( win )
	--	Forward keys
	--	Setup keys: Forward toggle list actions
	win.keys.Release = {
		[KEY_CURSOR_UP] = 'goUp',
		[KEY_CURSOR_DOWN] = 'goDown',
		[KEY_CURSOR_LEFT] = 'goLeft',
		[KEY_CURSOR_RIGHT] = 'goRight',
		[KEY_SMALL_E] = 'onMenuClose',
		[KEY_EXIT] = 'onMenuClose',
		[KEY_ESCAPE] = 'onMenuClose',
		[KEY_ENTER] = 'tuneCurrentChannel'
	}

	local channelManager = {}
	for k,v in pairs(channels) do
		channelManager[k] = v
	end
	channelManager.currentChannel = channel.current
	win.schedule = EPGScheduleClass.new(win.x + EPG_SCHEDULE_REL_X,
										win.y + EPG_SCHEDULE_REL_Y,
										EPG_SCHEDULE_WIDTH,
										EPG_SCHEDULE_HEIGHT,
										EPG_SCHEDULE_NBLOCKS_WIDTH,
										EPG_SCHEDULE_NBLOCKS_HEIGHT,
										EPG_SCHEDULE_NBLOCKS_HOUR_DIVISION,
										win.bg,
										channelManager,
										{getTime = getCurrentTime},
										{createSurfaceFromPath = createSurfaceFromPath})


	win.currentlySeeingLbl = lblNew( "Label for 'currently seeing'",
									 win.x + EPG_CURRENTLY_SEEING_X,
									 win.y + EPG_CURRENTLY_SEEING_Y,
									 EPG_CURRENTLY_SEEING_START,
									 EPG_CURRENTLY_SEEING_FONTSIZE,
									 EPG_CURRENTLY_SEEING_FONTCOLOR
								   )
	wndAddWidget( win, win.currentlySeeingLbl )

	
	win.descriptionText = lblNew( "Show description text",
								  win.x + EPG_DESCRIPTION_TEXT_REL_X ,
								  win.y + EPG_DESCRIPTION_TEXT_REL_Y,
								  " ",
								  EPG_DESCRIPTION_TEXT_FONTSIZE,
								  EPG_DESCRIPTION_TEXT_FONTCOLOR)
	wndAddWidget( win, win.descriptionText )

	setEPGDescriptionText( "No hay show seleccionado" )
	
	win.currentDateLbl = lblNew( "Date for user in EPG",
                                 win.x + EPG_DATE_REL_X,
                                 win.y + EPG_DATE_REL_Y,
                                 " ",
                                 EPG_DATE_FONTSIZE,
                                 EPG_DATE_FONTCOLOR)
	wndAddWidget( win, win.currentDateLbl )

	win.currentTimeClock = lblNew( "Clock for user in EPG",
                                   win.x + EPG_CLOCK_REL_X,
                                   win.y + EPG_CLOCK_REL_Y,
                                   " ",
                                   EPG_CLOCK_FONTSIZE,
                                   EPG_CLOCK_FONTCOLOR
                                   )

	wndAddWidget( win, win.currentTimeClock )

	win.showAgeRateLabel = lblNew( "Label for selected show age rating",
                                   win.x + EPG_AGE_RATING_REL_X,
                                   win.y + EPG_AGE_RATING_REL_Y,
                                   " ",
                                   EPG_AGE_RATING_FONTSIZE, 
                                   EPG_AGE_RATING_FONTCOLOR
                                   )
	wndAddWidget( win, win.showAgeRateLabel )
	setEPGSelectedShowAgeRate()

	win.selectedShowNameLabel = lblNew( "Label with name of selected show",
                                        win.x + EPG_SELECTED_SHOW_NAME_X,
                                        win.y + EPG_SELECTED_SHOW_NAME_Y,
                                        EPG_SELECTED_SHOW_DEFAULT_NAME,
                                        EPG_SELECTED_SHOW_NAME_FONTSIZE,
                                        EPG_SELECTED_SHOW_NAME_FONTCOLOR
                                        )
	wndAddWidget( win, win.selectedShowNameLabel )

	win.selectedChannelNameLabel = lblNew( "Label for the name of channel",
                                           win.x + EPG_SELECTED_CHANNEL_NAME_X,
                                           win.y + EPG_SELECTED_CHANNEL_NAME_Y,
                                           EPG_SELECTED_CHANNEL_DEFAULT_NAME,
                                           EPG_SELECTED_CHANNEL_NAME_FONTSIZE,
                                           EPG_SELECTED_CHANNEL_NAME_FONTCOLOR
                                           )
	wndAddWidget( win, win.selectedChannelNameLabel )

	win.selectedShowScheduleLabel = lblNew(
                                    "Label for start and end of program",
                                    win.x + EPG_SELECTED_SHOW_SCHEDULE_LBL_X,
                                    win.y + EPG_SELECTED_SHOW_SCHEDULE_LBL_Y,
                                    EPG_SELECTED_SHOW_SCHEDULE_DEFAULT_LBL,
                                    EPG_SELECTED_SHOW_SCHEDULE_LBL_FONTSIZE,
                                    EPG_SELECTED_SHOW_SCHEDULE_LBL_FONTCOLOR
                                    )
	wndAddWidget( win, win.selectedShowScheduleLabel)

	win.channelLogo = logoContainerNew("Channel logo",
                                       win.x + EPG_CHANNEL_LOGO_RECT.x,
                                       win.y + EPG_CHANNEL_LOGO_RECT.y,
                                       EPG_DEFAULT_LOGO_IMAGE)
	wndAddWidget( win, win.channelLogo)

end

-- In the current version this only loads an image and uses it as bg.
-- In previous versions, this would draw every color rectangle that 
-- composed the EPG.
function buildEPGBackground( win )
	local background = bgNew("EPG Background", 0, 0, EPG_BACKGROUND_IMAGE )
	wndAddWidget( win, background )
	win.bg = background
	wndResize( win, EPG_WINDOW_W, EPG_WINDOW_H ) 
end


----------------------------------------------------------
-- Epg show selection 
----------------------------------------------------------

function setEPGDescriptionText( aDescriptionText )
	if ( aDescriptionText == nil or aDescriptionText == "" ) then
		aDescriptionText = EPG_DEFAULT_DESCRIPTION_TEXT 
	end
	setFittingTextToLabel(EPGWindow.descriptionText, EPG_DESCRIPTION_WIDTH, EPG_DESCRIPTION_HEIGHT, aDescriptionText )
end

function setEPGSelectedShowAgeRate( anAgeRate )
	if ( anAgeRate == nil or anAgeRate == "" ) then
		anAgeRate = EPG_DEFAULT_AGE_RATE_TEXT  
	end
	lblText(EPGWindow.showAgeRateLabel, anAgeRate)
end

function setEPGSelectedShowName( aName )
	if ( aName == nil or aName == "" ) then
		aName = EPG_SELECTED_SHOW_DEFAULT_NAME
	end
	lblTextWithMaxWidth(EPGWindow.selectedShowNameLabel, aName, EPG_SELECTED_SHOW_MAX_WIDTH)
end

function setEPGSelectedShowSchedule( aStart, anEnd )
	if ( aStart == nil or aStart == "" ) then
		lblText(win.selectedShowScheduleLabel," ")
	else
		lblText(win.selectedShowScheduleLabel, aStart.time .. " a " .. anEnd.time ) 
	end
end

function setShowSelectedBySchedule( aShow ) 
	if (aShow == nil) then aShow = {} end
	setEPGDescriptionText( aShow.description )
	setEPGSelectedShowAgeRate( aShow.parentalAge )
	setEPGSelectedShowName( aShow.name )
	setEPGSelectedShowSchedule( aShow.startDate, aShow.stopDate )
end

----------------------------------------------------------
-- Channel selection
----------------------------------------------------------

function setEPGSelectedChannel ( aChannel )
	if (aChannel == nil) then 
		aChannel = {channel = "", name = EPG_SELECTED_CHANNEL_DEFAULT_NAME, channelID = -1}
	end
	refreshEPGBackground( EPGWindow )

	lblTextWithMaxWidth(EPGWindow.selectedChannelNameLabel, aChannel.channel .. "   " .. aChannel.name, EPG_SELECTED_CHANNEL_MAX_WIDTH)
	local channelInfo
	if (aChannel.channelID ~= -1) then
		channelInfo = channels.getInfo(aChannel.channelID)
	else
		channelInfo = { logo = "" }
	end
	if (channelInfo.logo ~= "") then 
		logoContainerChangeImage( win.channelLogo, channelInfo.logo )
	else
		logoContainerChangeImage( win.channelLogo, getImgsPath() .. "/" .. EPG_DEFAULT_LOGO_IMAGE )
	end
end


----------------------------------------------------------
-- View callbacks
----------------------------------------------------------


function refreshEPGBackground( epg )
	lblText(epg.currentTimeClock, os.date("%H:%M", getCurrentTime()))
	lblText(epg.currentDateLbl, firstToUpper(os.date("%A", getCurrentTime())) ..
											 os.date(", %d de ", getCurrentTime()) ..
											 firstToUpper(os.date( "%b. %Y", getCurrentTime())))
	local currentChannel = {}
	if (channels.count() == 0) then
		currentChannel = { name = EPG_NO_CHANNEL_AVAILABLE }
	else
		currentChannel = channels.getInfo(channel.current())
	end
	lblTextWithMaxWidth(epg.currentlySeeingLbl,
						EPG_CURRENTLY_SEEING_START --[[.. currentChannel.channel .. " "]] .. currentChannel.name,
						EPG_CURRENTLY_SEEING_MAX_WIDTH) 
end


function EPGOnShow ( epg )
	refreshEPGBackground( epg ) 
	winOnShow( epg )
	epg.schedule:resetBlockSelection()
	epg.schedule:refresh()
	Timers.epg.id = mainWindow.registerTimer (Timers.epg.timeout)
	local rect = EPG_VIDEO_CAPTION_RECT 
	channel.resize( epg.x + rect.x, epg.y + rect.y, rect.w, rect.h)
end

function EPGOnClose( epg )
	channel.resize( 0, 0, RootWindow.w, RootWindow.h )
	epg.schedule:onHide()
	winOnHide(epg)
	mainWindow.cancelTimer(Timers.epg.id)
end


----------------------------------------------------------
-- Controller callbacks
----------------------------------------------------------

function EPGWindow.actions.onMenuClose( wdt )
	wdtSetVisible( wdt, false )
end

function EPGWindow.goUp()
	refreshEPGBackground( EPGWindow )
	EPGWindow.schedule:goOneBlockUp()
	EPGWindow.schedule:draw()
end

function EPGWindow.goDown()
	refreshEPGBackground( EPGWindow )
	EPGWindow.schedule:goOneBlockDown()
	EPGWindow.schedule:draw()
end

function EPGWindow.goLeft()
	refreshEPGBackground( EPGWindow )
	EPGWindow.schedule:goOneBlockLeft()
	EPGWindow.schedule:draw()
end

function EPGWindow.goRight()
	refreshEPGBackground( EPGWindow )
	EPGWindow.schedule:goOneBlockRight()
	EPGWindow.schedule:draw()
end

function EPGWindow.tuneCurrentChannel()
	EPGWindow.schedule:tuneSelectedChannel()
end


function toogleEPG()
	EPGWindow.actions['onClose'] = nil
	wdtSetParent( EPGWindow, RootWindow )
	wdtToggleVisible( EPGWindow )
end
