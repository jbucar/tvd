-- Testea #attrOpacity con valores negativos
local opacity = -20
local image = canvas:new("resources/lifia.png")
image:attrOpacity(opacity)
canvas:compose(0,0,image)
canvas:flush()