require 'colors'

local _M = {}

local common_color_table = colors_extend(menuColorTable, {bgColor = Color.Gray4, textColor = Color.White, selectedTextColor = Color.Orange})

_M.disable_color_table = colors_extend(common_color_table, {
	textColor = Color.Gray3,
	textHeaderColor = Color.Gray3,
	bgLabelColor = Color.Gray2
})

_M.disabled_list = {
	textColor = Color.Black,
}

_M.enable_color_table = colors_extend(common_color_table, {
	bgLabelColor = Color.Gray2,
	textColor = Color.Orange,
	textHeaderColor = Color.Orange,
	selectedTextColor = Color.Black
})

local function set( widget, color_table, color_table2 )
	wgt:setColorMap(widget, colors_extend(color_table, color_table2))
	wgt:repaint(widget)
end

function _M.enable( widget, colors )
	set(widget, _M.enable_color_table, colors or {})
end

function _M.disable( widget, colors )
	set(widget, _M.disable_color_table, colors or {})
end

return _M
