-- Testea #attrCrop con valorers de “x” y “y” que están fuera de los límites del canvas
local subCanvas = canvas:new("resources/lifia.png")
local subSubCanvas = canvas:new(200,200)
subSubCanvas:drawRect("fill",0,0,200,200)
subCanvas:attrCrop(1480,1120,200,200)
subCanvas:compose(0,0,subSubCanvas)
canvas:compose(0,0,subCanvas)
canvas:flush()