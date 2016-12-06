-- Testea que #attrOpacity funcione correctamente con 0.
local opacity = 0
local image = canvas:new("resources/lifia.png")
image:attrOpacity(opacity)
canvas:compose(0,0,image)
canvas:flush()