-- Testea #attrFlip con valor horizontal = false y vertical = false
local image = canvas:new("resources/lifia.png")
image:attrFlip(true,false)
canvas:compose(0,0,image)
canvas:flush()