local log = require 'log'
local timer = require 'timer'
local l_util = require 'l_util'
local download_bar = require 'download'
local app_ctrl = require 'application.controller'
local app_download = require 'models.applications_download'
local gWindow = require 'ginga_window'
local muteWgt = require "mute"
local scan = require "models.scan"

require "colors"
require "zindex"
require "volume"

zmb_apps = require 'application.zmb_apps'

--Define bool constants
FALSE = 0
TRUE  = 1

-- Widgets banish
banish_timeout = 5000

zmb_apps.enable(appsEnabled) -- Enble/Disable ZaMBA Apps
timer.settimeout(5000) -- Set default timeout for timers

-- Cache fonts
canvas:cacheFonts( 'Tiresias' )

---------- Aux functions ------------
function updateBar( bar, step, total )
	win = CurrentWindow()
	if win.name ~= 'MainMenu' and win.name ~= 'EPG' then
		local value = (step * 100) / total
		openWgt(bar)
		bar:updateValue(value)
	else
		closeWgt(bar)
	end

	return true
end

function safe_coords()
	canvas_w, canvas_h = canvas.size()
	return l_util.math.round(canvas_w/10), l_util.math.round(canvas_h/10), l_util.math.round(canvas_w/10*8), l_util.math.round(canvas_h/10*8)
end

---------- Init vars ------------

-- Downloads bars
download_app_bar = download_bar.new('appBar', 'Descargando App', 440, download_app_zindex)
download_firmware_bar = download_bar.new('firmwareBar', 'Descargando firmware', 480, download_firmware_zindex)

app_download.observe('progress', function(step, total) return updateBar(download_app_bar, step, total) end)

-- Coordinates
safe_x, safe_y, safe_w, safe_h = safe_coords()

local lua_menu = require "lua_menu"

function restoreMenu()
	lua_menu:restoreMenu()
end

require "nav"
require "rootnav"
require "chs"
require "download"
require "pin"
require "toggles"
require "caption"

require "info"
require "info_boxes.info_zapping"
require "info_boxes.info_extended"

--------------------------------------------------------------------------------------------------
-- Zapper callbacks
--------------------------------------------------------------------------------------------------

function zapperApplicationLayerEnabled( isEnabled )
	if isEnabled == 1 then
		openWgt(gWindow)
	else
		closeWgt(gWindow)
	end
	view.refresh()
	appEnabled( isEnabled )
end

function appEnabled( isEnabled )
	if isEnabled == 1 then
		closeWgt(download_app_bar)
		view.refresh()
		app_ctrl.start_run()
	else
		app_ctrl.stop_run()
	end
end

function zapperShowUpdated( chID, current )
	if EPG.visible then
		EPG.channelChanged(chID)
		view.refresh()
	end
	updateInfo()
end

function zapperShowSubtitle( str )
	showCaption( str )
end

-------------------------------------------------------------------------------------
require "firmupdate"

function zapperUpdateReceived( updateInfo )
	log.debug("zamba", "zapperUpdateReceived", "updateID " .. updateInfo.id)
	updateReceived(updateInfo)
	view.refresh()
end

function zapperUpdateDownloadProgress( step, total )
	log.debug("zamba", string.format("zapperUpdateDownloadProgress step=%d total=%d", step, total))
	if not mainWindow.inStandby() then
		updateBar(download_firmware_bar, step, total)
		view.refresh()
	end
end

function zapperCurrentShowBlocked( blockedState )
	log.debug("zamba", "zapperCurrentShowBlocked", "blockedState=" .. tostring(blockedState))
	if blockedState and not PINDlg.visible then
		closeWgt(Info)
		PINDlg.init("Ingrese la clave para ver los canales bloqueados", dialogZIndex, nil, nil)
		openWgt(PINDlg)
	elseif not blockedState and PINDlg.visible then
		closeWgt(PINDlg)
	end
	view.refresh()
end

-------------------------------------------------------------------------------------

require "epg"

function zapperOnStandby( isOff )
	log.info("zamba", "On standby: off=" .. tostring(isOff) )
	if isOff == 1 then
		closeToRoot()
		if channels.count() == 0 then
			mainWindow.showBackground(0)
		end
	else
		checkUpdates()
		initBg()
		if needsChannelRestore() then
			resetChannel()
		end
	end
	view.refresh()
end

function OnTimerEvent( tID )
	log.debug("zamba", "OnTimerEvent", "Timer " .. tID .. " expired")

	timer.dispatch(tID)

	view.refresh()
end

function OnKeyEvent( key, isUp )
	log.debug("zamba", "OnKeyEvent, key == " .. key .. ' isUp == ' .. isUp)

	-- Dispatch event to zamba apps module
	local evt = {type="key", value=key, isUp=isUp}
	zmb_apps.dispatchEvent(evt)

	-- Process key
	if isUp == 0 then
		processKeys( key )
		view.refresh()
	end
end

function OnButtonEvent( id, isPress, x, y )
end

scan.observe("beginScan", function()
	mainWindow.showBackground(TRUE)
end)

scan.observe("endScan", function()
	if channels.count() > 0 then
		mainWindow.showBackground(FALSE)
		closeToRoot()
		changeFactor(false,1)
	end
end)

function initBg()
	if (channels.count() == 0) then
		openWgt(lua_menu)
		mainWindow.showBackground(1)
	end
	muteWgt.checkInitStatus()
end

-- Main

log.info("zamba", "init")

initBg()
view.refresh()

zmb_apps.init()

Info_Zapping.create()
createExtendedInfoBoxes()

mainWindow.run()

closeToRoot()

zmb_apps.fin()

log.info("zamba", "exit")
