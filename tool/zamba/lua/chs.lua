local log = require 'log'
local timer = require 'timer'
local app_ctrl = require 'application.controller'
local channel_model = require 'models.channel'

require 'channel_list_window'
require "basewidget"

local VirtualChannel = -1
local RclChannel = -1
local _needsChannelRestore = false

function resetVirtualChannel()
	VirtualChannel = -1
end

function setVirtualChannel(c)
	VirtualChannel = c
end

local currentChannel = 0

function renderChannel()
	renderInfo(InfoMenu['visible'] == 2)
	if (Volume.mute == 1) then
		renderMute()
	end
end

local function refreshData(ix, isVirtual)
	if not Info_Zapping.visible then
		close_current_infobox()
		openWgt(Info_Zapping)
	else
		Info_Zapping:banish_restart()
	end

	Info_Zapping.change(ix, isVirtual)
end

function changeChannel(ix, isVirtual)
	if (ix ~= -1) then -- ix can be RclChannel, and it may be unset
		if ( Channel_List_Window.visible ) then
			closeWgt( Channel_List_Window )
		end
		if channels.isProtected(ix) then
			changeChannelCallback = function()
				refreshData(ix, isVirtual)
				doChangeChannel(ix, false)
			end
			PINDlg.init("Por favor, ingrese la clave", dialogZIndex, changeChannelCallback, nil)
			openWgt(PINDlg)
		else
			if (not isVirtual) then
				doChangeChannel( ix );
			else
				VirtualChannel = ix
			end
			refreshData(ix, isVirtual)
		end
	else
		log.info("Chs", "nextFav", "no favourite channel to change")
	end
end

function doChangeChannel(ix)
	log.debug("Chs", "change channel to: " .. tostring(ix))
	RclChannel = channel.current()
	ignoreChKeys(true)
	closeCC()
	view:refresh() -- in order to draw the zapping info before changing the channel
	channel.change(ix)
	resetVirtualChannel()

	channel_model.change(ix)
end

function changeChannelVirtual()
	changeChannel( VirtualChannel, false )
	resetVirtualChannel()
end

function getCurrentChannel( isVirtual )
	if (isVirtual and VirtualChannel ~= -1) then
		return VirtualChannel
	else
		return channel.current()
	end
end

function changeFactor( isVirtual, factor )
	local ch = channel.next( getCurrentChannel(isVirtual), factor )
	changeChannel(ch,isVirtual)
end


function channelUp( isVirtual )
	changeFactor(isVirtual,1)
end

function channelDown( isVirtual )
	changeFactor(isVirtual,-1)
end

function previousChannel()
	-- print('[lua] previousChannel: ch='..RclChannel)
	changeChannel(RclChannel,false)
end

function nextFav()
    local nextChannel = channel.nextFavorite( channel.current(),1)
	changeChannel( nextChannel, false );
end

function channelDeleted(chId)
	if RclChannel == chId then
		RclChannel = -1
	end
end

function stopChannel( isVideo )
	if (channels.count()>0) then
		local tmp = channel.current()
		if tmp >= 0 then
			currentChannel = tmp
			_needsChannelRestore = true
		end
		channel.change( -1 )
	end
	if not isVideo then
		mainWindow.showBackground(1)
	else
		mainWindow.showBackground(0)
	end
end

function resetChannel()
	if (channels.count()>0) then
		mainWindow.showBackground(0)
		channel.change( currentChannel )
		_needsChannelRestore = false
	else
		mainWindow.showBackground(1)
	end
end

function needsChannelRestore()
	return _needsChannelRestore
end

