local log = require 'log'
local Widget = require "basewidget"
local config = require "menu.config_page"

local About = {}
setmetatable(About, Widget.mt) -- Inherits behaviour from Widget
About_mt = { __index = About }

local aboutColorTable = {
	bgColor = Color.Gray3,
	textColor = Color.Black,
}

local titleAboutColorTable = {
	bgColor = Color.Gray6,
	bgLabelColor = Color.Gray6,
	textColor = Color.White,
}

function About:onSelected()
	log.trace("LuaMenu::About", "onSelected")
	closeHelp()
	return true
end

function About:processKeys( key )
	log.trace("LuaMenu::About", "processKeys")
	return false
end

function About:title()
	return "ACERCA DE"
end

function About:icon()
	return "Menu-4-AcercaDe"
end

local _M = {}

function _M.new( win )
	local page = Widget.new("MenuAbout", false, false)
	setmetatable(page, About_mt)

	page.deny_activation = true

	local titlewidth = 80
	local separation = 5
	local valuewidth = config.page.width - (titlewidth + separation)
	local zapper_desc_height = 90
	local textPadding = 10

	local img_w = 432
	local img_x = (config.page.width - img_w) / 2
	local logo_img    = image:new(0, 0, img_w, 60, menuZIndex, menuColorTable, "AcercaDe.png")
	window:addChild(win, logo_img)

	local zamba_label = textarea:new(0, 70, titlewidth, 160
                                     , titleAboutColorTable
                                     ,"¿Qué es ZaMBA?"
                                     , 16, 10, textPadding, Alignment.vTop_hLeft)
	window:addChild(win, zamba_label)

	local zamba_value = textarea:new(titlewidth + separation, 70, valuewidth, 160
                                     , aboutColorTable
                                     , "Zapper Multifunción Básico Argentino (ZaMBA) es el componente de software del decodificador mediante el cual el usuario interactua para ver, buscar, seleccionar y sintonizar canales. Permite además configurar el decodificador, administrar contenido multimedia y navegar la Guía de Programación Electrónica (EPG). "
                                     , 14, 10, 4, Alignment.vTop_hLeft)
	window:addChild(win, zamba_value)

	local zapper_label = label:new(0, 234, titlewidth, zapper_desc_height, titleAboutColorTable, "ZaMBA", 16, Alignment.vTop_hLeft, textPadding)
	window:addChild(win, zapper_label)

	local zapper_value = textarea:new(titlewidth + separation, 234, valuewidth, zapper_desc_height
                                     , aboutColorTable
                                     , "Versión de Producto: " ..zapperVersion.." ("..toolBuildVersion.. ")\nVersión de ZaMBA: "..toolVersion.. "\nVersión de Ginga: " .. middleware.getVersion() .. "\nOS: " .. platformVersion
                                     , 14, 10, 4, Alignment.vTop_hLeft)
	window:addChild(win, zapper_value)

	return page
end

return _M
