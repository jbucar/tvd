-- Testea #attrCrop
local subCanvas = canvas:new("resources/lifia.png")
local subSubCanvas = canvas:new(50,50)
subSubCanvas:drawRect("fill",0,0,200,200)
subCanvas:attrCrop(50,50,200,200)
subCanvas:compose(0,0,subSubCanvas)
canvas:compose(0,0,subCanvas)
canvas:flush()