local log = require 'log'
local volume = require 'models.volume'
local lua_menu = require "lua_menu"
local change_channel = require "change_channel"
require 'info_boxes.info_standard'

require "nav"

RootNav = {
	  isRoot = true
	, IgnoreChKeys = false --TODO: Eliminarlo y pasarlo a usar ignoredkeys de basewidget
}

function RootNav.init()
	RootNav = BaseWidget:new( RootNav, "RootNav", false, true)
	openWgt(RootNav)
end

function ignoreChKeys(b)
	RootNav.IgnoreChKeys = b
end

function RootNav:onHide()
	mainWindow.stop( 0 )
	return false
end

function RootNav:processKeys(key)
	return false
end

function RootNav.onMenu()
	openWgt(lua_menu)
	closeWgt(download_app_bar)
	closeWgt(download_firmware_bar)
	return true
end

function RootNav.onOk()
	changeChannelVirtual()
	return true
end

function RootNav.onVolUp()
	volume.up()
	return true
end

function RootNav.onVolDown()
	volume.down()
	return true
end

function RootNav.onDigit(digit)
	openWgt(change_channel)
	change_channel:processKeys(digit+1)
	return true
end

function RootNav.onChannelDown()
	if (not RootNav.IgnoreChKeys) then
		channelDown(false)
	end
	return true
end

function RootNav.onChannelUp()
	if (not RootNav.IgnoreChKeys) then
		channelUp(false)
	end
	return true
end

function RootNav.onFav()
	if (not RootNav.IgnoreChKeys) then
		nextFav()
	end
	return true
end

function RootNav.onRecall()
	if (not RootNav.IgnoreChKeys) then
		previousChannel()
	end
	return true
end

function RootNav.onDown()
	channelDown( true )
	return true
end

function RootNav.onUp()
	channelUp( true )
	return true
end

function RootNav.onMute()
	volume.toggleMute()
	return true
end

function RootNav.onTVmode()
	toggleVideoMode()
	return true
end

function RootNav.onAspect()
	toggleAspectMode()
	return true
end

function RootNav.onSub()
	toggleCaption()
	return true
end

function RootNav.onAudio()
	toggleAudio()
	return true
end

function RootNav.onInfo()
	openWgt(Info_Standard)
	return true
end

function RootNav.onEPG()
	if channels.count() ~= 0 then
		openEPG()
	end
	return true
end

function RootNav.onChannelList()
	openWgt(Channel_List_Window)
	return true
end

function RootNav.onToggleFullscreen()
	local value = not mainWindow.isFullscreen()
	log.debug( "RootNav", "onToggleFullscreen", "fullscreen " .. tostring(value) )
	mainWindow.fullscreen(value)
	return true
end

RootNav.init()