-- Testea que #attrRotation no tenga ningún efecto si se le pasa como parámetro un número negativo
local rotation = -90
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()