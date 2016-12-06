local _M = {}

function _M.getIcon( val )
	return {normal = string.format( "content_%02X.png", val ), selected = string.format( "content_%02X_selected.png", val )}
end

_M.values = {
	{name = "news", width = 61, pos = 0x01},
	{name = "documentary", width = 87, pos = 0x02},
	{name = "sports", width = 57, pos = 0x03},
	{name = "art", width = 38, pos = 0x12},
	{name = "educative", width = 74, pos = 0x04},
	{name = "series", width = 44, pos = 0x05},
	{name = "musical", width = 51, pos = 0x06},
	{name = "reality", width = 53, pos = 0x07},
	{name = "information", width = 94, pos = 0x08},
	{name = "cooking", width = 49, pos = 0x09},
	{name = "fashion", width = 56, pos = 0x0A},
	{name = "travel", width = 34, pos = 0x0B},
	{name = "comical", width = 52, pos = 0x0C},
	{name = "children", width = 59, pos = 0x0D},
	{name = "erotic", width = 55, pos = 0x0E},
	{name = "movie", width = 59, pos = 0x0F},
	{name = "discussion", width = 47, pos = 0x10},
	{name = "soap_opera", width = 81, pos = 0x11},
	{name = "unknown", width = 93, pos = 0x00}
}

return _M
