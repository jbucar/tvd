-- Testea #attrCrop
local subCanvas = canvas:new(500,500)
subCanvas:attrColor( "red" )
subCanvas:drawRect("fill",0,0,200,200)

local subSubCanvas = canvas:new("resources/lifia.png")
subCanvas:compose(50,50,subSubCanvas)

subCanvas:attrCrop(50,50,200,200)

canvas:compose(0,0,subCanvas)
canvas:flush()
