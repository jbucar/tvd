-- Testea que #attrRotation no tenga ningún efecto si se le pasa como parámetro un número que no es múltiplo de 90
local rotation = 94
local image = canvas:new("resources/lifia.png")
image:attrRotation(rotation)
canvas:compose(0,0,image)
canvas:flush()