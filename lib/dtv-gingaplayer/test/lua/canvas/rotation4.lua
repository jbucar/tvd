-- Testea que #attrRotation funcione correctamente con 360 como parámetro.
local rotation = 360
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()