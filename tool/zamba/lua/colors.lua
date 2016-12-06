--Cómo personalizar el logo de zamba:
--  Para reemplazar el icono de la barra de herramientas: Reemplazar la imagen presente en: instalación/share/zamba/imgs/icon.png
--  Para reemplazar la imagen que aparece de fondo cuando la lista de canales esta vacia: instalación/share/zamba/imgs/inicio.png
--  Para reemplazar la imagen que aparece arriba del menú: instalación/share/zamba/imgs/Logo.png

local l_util = require 'l_util'

Color = {
	None     = wgt:newColor( -1,  -1,  -1),
	White    = wgt:newColor(255, 255, 255),
	Black    = wgt:newColor(  0,   0,   0),
	Blue     = wgt:newColor( 53,  95, 145),
	DarkBlue = wgt:newColor( 66,  63,  56),
	Gray     = wgt:newColor(180, 180, 180),
	Gray2    = wgt:newColor( 80,  80,  80),
	Gray3    = wgt:newColor(150, 150, 150),
	Gray4    = wgt:newColor( 40,  40,  40),
	Gray5    = wgt:newColor( 96,  96,  96),
	Gray6    = wgt:newColor( 75,  75,  75),
	Gray7    = wgt:newColor(122,  122,  122),
	Red      = wgt:newColor(255,   0,   0),
	Orange   = wgt:newColor(255, 160,   0),
}

menuColorTable = {
	bgColor = Color.Gray4,
	selectedColor = Color.Orange,
	activeTabBgColor = Color.Gray4,
	unselectedTabBgColor = Color.Gray5,
	disabledColor = Color.Gray2,
	bgButtonColor = Color.Gray3,
	selectedButtonColor = Color.Orange,
	bgLabelColor = Color.None,
	bgBarColor = Color.Orange,
	borderColor = Color.Gray,
	textColor = Color.White,
	selectedTextColor = Color.Orange,
	cellColor = Color.Gray3,
	selectedCellColor = Color.Orange,
	headerColor = Color.Gray6,
	textHeaderColor = Color.Orange,
	scrollColor = Color.Gray3,
	scrollBgColor = Color.Gray6,
}

comboboxColorTable = {
	bgColor = Color.Gray3,
	bgLabelColor = Color.None,
	bgComboColor = Color.Gray4,
	borderColor = Color.Gray3,
	selectedColor = Color.Orange,
	disabledColor = Color.Gray2,
	textColor = Color.Black,
	selectedTextColor = Color.Black,
	bgBarColor = Color.White,
	textHeaderColor = Color.Orange,
	comboTextColor = Color.White,
	comboSelTextColor = Color.White,
}

barColorTable = {
	bgColor = Color.Gray3,
	bgProgressBarColor = Color.Gray4,
	bgBarColor = Color.Orange,
	textColor = Color.Black,
}

buttonColorTable = {
	bgColor = Color.Gray4,
	disabledColor = Color.Gray2,
	bgButtonColor = Color.Gray3,
	selectedButtonColor = Color.Orange,
	textColor = Color.Black,
	selectedTextColor = Color.White,
	borderColor = Color.Gray,
}

alertColorTable = {
	bgColor = Color.Gray4,
	disabledColor = Color.Gray2,
	bgButtonColor = Color.Gray3,
	selectedButtonColor = Color.Orange,
	textColor = Color.White,
	selectedTextColor = Color.White,
	buttonTextColor = Color.Black,
	borderColor = Color.Orange,
	titleColor = Color.Orange,
}

whiteLabelColorTable = {
	bgLabelColor = Color.White,
	textColor = Color.Black,
}

labelsColorTable = {
	textColor = Color.Orange
}

dateColorTable = {
	textColor = Color.Gray3
}

function colors_extend( to_extend, extension )
	local ct = l_util.table.shallow_copy(to_extend)
	ct = l_util.table.extend(ct, extension)
	return ct
end
