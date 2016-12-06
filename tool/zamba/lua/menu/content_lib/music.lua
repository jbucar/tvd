local volume = require 'models.volume'
local media_file = require 'models.media_file'
local l_util = require 'l_util'
local Widget = require "basewidget"
local Tab = require 'menu.content_lib.tab'

local _M = {} -- Module

local MusicTab = {}
setmetatable(MusicTab, Tab.mt) -- Inherits behaviour from Widget
MusicTab_mt = { __index = MusicTab }

local list_description = {
	visible_columns={"status", "name"},
	alignments = {Alignment.vCenter_hCenter, Alignment.vCenter_hLeft},
	headers = {'', 'Música'},
	index_key = "id",
	columns_sizes = {config.page.list.width * 0.1, config.page.list.width * 0.893},
}

local musicColorTable = {
	bgColor = Color.Gray6,
	textColor = Color.Orange,
}

-- API

function _M.new( win )
	local self = Tab.new(list_description, "Musica", "music", "Muestra la lista de MP3 del dispositivo USB", "Reproducir canción", musicEnabled == TRUE)
	setmetatable(self, MusicTab_mt)

	self._playing = -1
	self._audioPaused = TRUE
	self._titleBar = window:new(0, 0, 390, 22, menuZIndex, musicColorTable)
	self._volume = label:new(357, 0, 30, 22, musicColorTable, "", 14, Alignment.vBottom_hCenter)

	list:setCellAlignments(self.content, {Alignment.vCenter_hCenter, Alignment.vCenter_hLeft})
	list:mapColumn(self.content, 0, "||", "img:PausaBlanco.png", "img:PausaNegro.png")
	list:mapColumn(self.content, 0, ">", "img:PlayBlanco.png", "img:PlayNegro.png")

	local music_label = label:new(4, 0, 30, 22, musicColorTable, "Música", 14, Alignment.vBottom_hLeft)
	local vol_label = label:new(326, 0, 30, 22, musicColorTable, "Volumen", 14, Alignment.vBottom_hRight)

	window:addChild(self._titleBar, self._volume)
	window:addChild(self._titleBar, music_label)
	window:addChild(self._titleBar, vol_label)

	wgt:setVisible(self._titleBar, FALSE)

	volume.observe("down", function() self:update_volume() end)
	volume.observe("up", function() self:update_volume() end)
	volume.observe("unmute", function() self:update_volume() end)
	volume.observe("mute", function() self:update_volume() end)
	media_file.observe("file_end", function()
		if not mainWindow.inStandby() then
			resetChannel()
		end
		self:stop()
	end)

	window:addChild(win, self.content)
	window:addChild(win, self._titleBar)

	return self
end

function MusicTab:update_volume( vol )
	if self._volume then
		label:setText(self._volume, vol or volume.value())
	end
end

function MusicTab:play( index )
	self._audioPaused = FALSE
	self._playing = index
	local song = self:selected_item()
	song.status = "||"
	player.playAudioFile(song.id)
	self:update_row(index, song)
	self:update_volume()
	stopChannel(false)
end

function MusicTab:onKey()
	self:update_volume()
end

function MusicTab:stop()
	if self._playing ~= -1 then
		local song = self._values[self._playing + 1]
		song.status = ">"
		player.stopFile()
		self:update_row(self._playing, song)
		self._playing = -1

		if channels.count() > 0 then
			channel.mute(0)
		end
		self:update_volume()
	end
end

function MusicTab:togglePause( index )
	local song = self:selected_item()
	self._audioPaused = (self._audioPaused + 1) % 2

	if self._audioPaused == TRUE then
		song.status = ">"
		resetChannel()
	else
		song.status = "||"
		stopChannel(false)
	end

	player.pauseFile(self._audioPaused)
	self:update_row(index, song)
	self:update_volume()
end

function MusicTab:onOk()
	local index = list:selected(self.content)
	if self._playing == -1 then
		stopChannel(false)
		self:play(index)
	elseif self._playing == index then
		self:togglePause(index)
	else
		self:stop()
		self:play(index)
	end
end

function MusicTab:values()
	return player.getAudioFiles()
end

function MusicTab:onSelected()
	wgt:setVisible(self._titleBar, TRUE)
	self:update_volume()
end

function MusicTab:onUnselected()
	wgt:setVisible(self._titleBar, FALSE)
	self:stop()
end

return _M
