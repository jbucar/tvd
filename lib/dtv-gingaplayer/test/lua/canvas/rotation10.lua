-- Testea que #attrRotation no tenga efecto sobre el canvas principal
local image = canvas:new("resources/lifia.png")
canvas:compose(0,0,image)
canvas:flush()

local rotation = 90
canvas:attrRotation(rotation)
canvas:flush()