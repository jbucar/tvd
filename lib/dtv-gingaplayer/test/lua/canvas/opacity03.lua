-- Testea que #attrOpacity funcione con 255
local opacity = 255
local image = canvas:new("resources/lifia.png")
image:attrOpacity(opacity)
canvas:compose(0,0,image)
canvas:flush()