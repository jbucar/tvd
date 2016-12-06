-- Testea que #attrRotation funcione correctamente con 270 como parámetro.
local rotation = 270
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()