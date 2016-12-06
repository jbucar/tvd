-- Testea que #attrOpacity funcione con un valor intermedio del rango (0,255)
local opacity = 125
local image = canvas:new("resources/lifia.png")
image:attrOpacity(opacity)
canvas:compose(0,0,image)
canvas:flush()