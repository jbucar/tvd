local Widget = require "basewidget"
local Tab = require 'menu.content_lib.tab'

local _M = {} -- Module

local ImagesTab = {}
setmetatable(ImagesTab, Tab.mt) -- Inherits behaviour from Widget
ImagesTab_mt = { __index = ImagesTab }

local list_description = {
	visible_columns = {"name"},
	headers = {'Imágenes'},
	index_key = "id",
	columns_sizes = {config.page.list.width}
}

-- Implementation

local ImageViewer = {}
setmetatable(ImageViewer, Widget.mt) -- Inherits behaviour from Widget
ImageViewer_mt = { __index = ImageViewer }

local usbColorTable = {
	bgColor = Color.Gray4,
	textColor = Color.Black,
	selectedTextColor = Color.Black,
	cellColor = Color.Gray3,
	selectedCellColor = Color.Orange,
	headerColor = Color.Gray6,
	textHeaderColor = Color.Orange,
	scrollColor = Color.Gray3,
	scrollBgColor = Color.Gray6,
}

function ImageViewer.new()
	local self = Widget.new("ImageViewer", true, true)
	setmetatable(self, ImageViewer_mt)

	image_viewer = imageviewer:new(0, 0, 720, 576, dialogZIndex, usbColorTable, {})
	wgt:setVisible(image_viewer, FALSE)
	self:addWgt('viewer', image_viewer)

	self._oldTransparency = nil

	return self
end

function ImageViewer:add_images( images )
	local res = {}
	for i=1, #images do
		res[i] = images[i].path
	end

	imageviewer:images(self:getWgt('viewer'), res)
end

function ImageViewer:select( ix )
	local image = self:getWgt('viewer')
	imageviewer:selectedImage(image, ix)
	wgt:setVisible(image, TRUE)
end

function ImageViewer:onShow()
	self._oldTransparency = canvas.getTransparency()
	canvas.setTransparency(10)
	return true
end

function ImageViewer:onHide()
	canvas.setTransparency(self._oldTransparency)
	return true
end

function ImageViewer:processKeys( key )
	if (key == KEY_ESCAPE) then
		canvas.setTransparency(self._oldTransparency)
	end

	return wgt:processKey(self:getWgt('viewer'), key)
end

-- API

function _M.new( win )
	local self = Tab.new(list_description, "Imagenes", "images", "Muestra las imágenes del dispositivo USB", "Ver imagen")
	setmetatable(self, ImagesTab_mt)

	self._viewer = ImageViewer.new()
	window:addChild(win, self.content)

	return self
end

function ImagesTab:onOk()
	self._viewer:add_images(self:values())
	openWgt(self._viewer)
	self._viewer:select(list:selected(self.content))
end

function ImagesTab:values()
	return player.getImageFiles()
end

return _M
