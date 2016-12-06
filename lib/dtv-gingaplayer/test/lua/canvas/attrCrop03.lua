-- Testea #attrCrop
local subCanvas = canvas:new(500,500)
subCanvas:attrColor( "red" )
subCanvas:drawRect("fill",0,0,200,200)
subCanvas:attrColor( "green" )
subCanvas:drawRect("fill",50,50,200,200)


subCanvas:attrCrop(0,0,100,100)

canvas:compose(0,0,subCanvas)
canvas:flush()
