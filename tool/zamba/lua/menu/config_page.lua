l_util = require 'l_util'
require 'zindex'
require 'colors'

local function safe_coords()
	canvas_w, canvas_h = canvas.size()
	return l_util.math.round(canvas_w/10), l_util.math.round(canvas_h/10), l_util.math.round(canvas_w/10*8), l_util.math.round(canvas_h/10*8)
end

local safe_x = nil
local safe_y = nil
local safe_w = nil
local safe_h = nil

safe_x, safe_y, safe_w, safe_h = safe_coords()

local separation = 5

config = {
	button_h = 60,
	button_w = 60,
	icon_h = 27,
	icon_container = 40,
	sep = 3,
	menu_separation = separation,
	help_height = 60
}

config.buttons_color_table = {
	bgColor = Color.Gray4,
	bgButtonColor = Color.Gray4,
	selectedButtonColor = Color.Gray4,
	disabledColor = Color.Gray4,
	textColor = Color.White,
	selectedTextColor = Color.Orange,
	disabledTextColor = Color.Gray7
}

local config_menu_button_w = 96

config.menu = {
	x = safe_x,
	y = safe_y - 23,
	w = safe_w,
	h = safe_h,
	button_w = config_menu_button_w,
	button_h = 65,
	buttons_h = safe_h - config.help_height - separation,
	icon_h = 40,
	icon_w = 40,
	icon_sep = 0,
	title = {
		x = 0,
		y = 5,
		h = 26,
		margin = separation,
	}
}

config.logo = {}
config.logo.width = 135
config.logo.height = 23
config.logo.x = config.menu.w - config.logo.width
config.logo.y = config.menu.y - config.logo.height

config.container = {
	x = config.menu.button_w + separation,
	y = 0,
	w = config.menu.w - config.menu.button_w - config.menu_separation,
	h = config.menu.h - separation - config.help_height
}

-- config.menu.title.x = config.container.x


config.page_x = 0
config.page_margin = 17
config.page_border = 4

config.page_y = config.menu.title.y + config.menu.title.h
config.page_h = config.container.h - (config.page_margin + config.page_border) * 2 - config.page_y
config.win_h = config.page_y + config.page_h + config.menu_separation * 2 + config.help_height
config.help_y = config.page_y + config.page_h + config.menu_separation

config.page = {
	x = config.page_margin + config.page_border,
	-- x = config.container.x + config.page_margin + config.page_border,
	width = config.container.w - 2 * (config.page_margin + config.page_border),
	height = config.page_h,
	margin = 17,
	border = 4
}

config.page.list = {
	width = config.page.width - 22, -- The 22 is for the scrollbar
	height = config.page.height * 0.75
}

config.page.buttons = {
	x = 0,
	y = config.page.list.height + (config.page.height * 0.25 - config.button_h) / 2,
	w = config.page.width
}

function config.center( w, h )
	local x = config.menu.x + config.container.x + config.page_margin + config.menu_separation + (config.page.width - w) / 2
	local y = config.menu.y + config.container.y + config.page_margin + config.menu_separation + (config.page.height - h) / 2
	return x, y
end

return config
