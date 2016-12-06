-- Testea valor “x” negativo para #attrCrop
local subCanvas = canvas:new("resources/lifia.png")
local subSubCanvas = canvas:new(200,200)
subSubCanvas:drawRect("fill",0,0,200,200)
subCanvas:attrCrop(-100,100,100,100)
subCanvas:compose(0,0,subSubCanvas)
canvas:compose(0,0,subCanvas)
canvas:flush()