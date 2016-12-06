-- Testea que #attrRotation funcione correctamente con 0 como parámetro.
local rotation = 0
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()