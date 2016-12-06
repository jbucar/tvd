local Tab = require 'menu.content_lib.tab'
require 'widgets.videoplayer'

local _M = {} -- Module

local VideoTab = {}
setmetatable(VideoTab, Tab.mt) -- Inherits behaviour from Widget
VideoTab_mt = { __index = VideoTab }

local list_description = {
	visible_columns = {"name"},
	headers = {'Videos'},
	index_key = "id",
	columns_sizes = {config.page.list.width}
}

function _M.new( win )
	local self = Tab.new(list_description, "Videos", "video", "Muestra los videos del dispositivo USB", "Reproducir video")
	setmetatable(self, VideoTab_mt)

	VideoPlayer.create()

	window:addChild(win, self.content)

	return self
end

function VideoTab:onOk()
	VideoPlayer.playScannedFile(list:selected(self.content))
end

function VideoTab:values()
	return player.getVideoFiles()
end

return _M
