-- Testea que #attrRotation funcione correctamente con 180 como parámetro.
local rotation = 180
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()