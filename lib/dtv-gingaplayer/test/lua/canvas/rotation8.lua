-- Testea #attrRotation con un parámetro múltipĺo de 90 pero mayor que 360
local rotation = 810
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()