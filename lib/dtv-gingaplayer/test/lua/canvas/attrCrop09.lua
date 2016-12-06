-- Testea #attrCrop con valores de width y height más grandes que el canvas
canvas:attrCrop(0,0,1480,1120)
local subCanvas = canvas:new("resources/lifia.png")
canvas:compose(0,0,subCanvas)
canvas:flush()