-- Testea #attrFlip con ambos valores true
local image = canvas:new("resources/lifia.png")
image:attrFlip(true,true)
canvas:compose(0,0,image)
canvas:flush()