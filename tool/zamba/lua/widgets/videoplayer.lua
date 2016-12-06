local volume = require 'models.volume'
local media_file = require 'models.media_file'

VideoPlayer = {
	playing = 0,
	_cbk = nil,
	notifyStop = false
}

function VideoPlayer.create()
	VideoPlayer = BaseWidget:new( VideoPlayer, "VideoPlayer", true)
	VideoPlayer.wgt.video_viewer = videoviewer:new(0, 0, 720, 576, dialogZIndex)
	wgt:setVisible(VideoPlayer.wgt.video_viewer, FALSE)

	media_file.observe("file_end", VideoPlayer.mediaEnd)
end

local function vol_key(key)
	local ret  = ((key == KEY_MUTE) or (key == KEY_SMALL_M))
	ret  = ret or ((key == KEY_VOLUME_UP) or (key == KEY_SMALL_W))
	ret  = ret or ((key == KEY_VOLUME_DOWN) or (key == KEY_SMALL_X))
	return ret
end

function VideoPlayer:processKeys(key)
	if (key == KEY_ESCAPE) then
		closeWgt(VideoPlayer)
		return true
	elseif vol_key(key) then
		return false
	elseif (key == KEY_OK) or (key == KEY_ENTER) then
		player.pauseFile( VideoPlayer.playing )
		VideoPlayer.playing = (VideoPlayer.playing + 1) % 2
	elseif (key == KEY_F11) then
		return wgt:processKey(VideoPlayer.wgt.video_viewer, key)
	end
	return true
end

local function initPlay()
	stopChannel( true )
	openWgt(VideoPlayer)
	VideoPlayer.playing = 1
end

function VideoPlayer.playFile( file )
	initPlay()
	player.playFile( file )
end

function VideoPlayer.playScannedFile( file )
	initPlay()
	player.playVideoFile( file )
end

function VideoPlayer.mediaEnd()
	VideoPlayer.notifyStop = true
	closeWgt( VideoPlayer )
	VideoPlayer.notifyStop = false
end

function VideoPlayer.onStop( cbk )
	VideoPlayer._cbk = cbk
end

function VideoPlayer.stop()
	VideoPlayer.playing = 0
	if not mainWindow.inStandby() then
		resetChannel()
	end
	view.refresh()
	if VideoPlayer._cbk ~= nil then
		VideoPlayer._cbk()
		VideoPlayer._cbk = nil
	end
end

function VideoPlayer:onHide()
	if VideoPlayer.notifyStop then
		VideoPlayer.stop()
		return true
	else
		player.stopFile()
		return false
	end
end
