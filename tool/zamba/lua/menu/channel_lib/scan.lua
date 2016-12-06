local log = require 'log'
local config = require "menu.config_page"
local scan = require "models.scan"

local _M = {}

local window_color_table = {
	bgColor = Color.Gray3,
	borderColor = Color.Gray3,
	textColor = Color.Black
}

local bar_color_table = {
	bgColor = Color.Gray4,
	borderColor = Color.White,
	textColor = Color.Black,
}

-- View
local win = nil
local text = nil
local pbar = nil

-- API

function _M.create( x, y, w, h )
	win = window:new(x, y, w, h, menuZIndex, window_color_table, 0)
	wgt:setVisible(win, FALSE)

	local height = h * 0.2
	local x_pos = w * 0.05
	local y_pos = h * 0.2

	text = label:new(x_pos, y_pos, w * 0.9, height, window_color_table, "Buscando canales... ", 15, Alignment.vTop_hLeft)
	window:addChild(win, text)

	local barFrame = window:new(x_pos, y_pos + height * 2, w * 0.9, 10, menuZIndex, bar_color_table)
	window:addChild(win, barFrame)

	local margin = 3
	pbar = bar:new(margin, margin, w * 0.9 - margin * 2, 4, barColorTable, 0, 0, 100)
	window:addChild(barFrame, pbar)

	return win
end

function _M.open()
	bar:setValue(pbar, 0)
	wgt:setVisible(win, TRUE)
end

local function onScanNetwork( net, perc )
	label:setText(text, 'Buscando canales... '.. perc ..'% ('.. net ..'Mhz)'..'  -  '.. channels.count() ..' encontrados')
	bar:setValue(pbar, perc)
end

scan.observe('scanNetwork', onScanNetwork)
scan.observe('endScan', function() wgt:setVisible(win, FALSE) end)

return _M
