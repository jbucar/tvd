-- Testea que #attrRotation funcione correctamente con 90 como parámetro.
local rotation = 90
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()