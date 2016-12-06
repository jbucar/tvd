local style = require 'style'

-- Constants
local ITEM_WIDTH = 240
local COMBO_MAX_LINES = 5

local M = {}

function M.makeCombo(x, y, w, h, title, items, items_width)
	local combo = combobox:new(x, y, w, h, style.combobox.color, title, style.font.size, items, style.font.size, items_width or ITEM_WIDTH)
	return combo
end

function M.makeComboWithMaxLines(x, y, w, h, title, items, max_lines)
	local combo = M.makeCombo(x, y, w, h, title, items)
	combobox:setMaxLines(combo, max_lines or COMBO_MAX_LINES)
	combobox:scrollImages(combo, "arrow_up_naranja.png", "arrow_down_naranja.png")
	return combo
end

return M
