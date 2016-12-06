-- Testea #attrFlip con valor horizontal = false y vertical = true
local image = canvas:new("resources/lifia.png")
image:attrFlip(false,true)
canvas:compose(0,0,image)
canvas:flush()