-- Testea que #drawRoundRect dibuje correctamente un cuadrado de color verde
local x,y = 0,0
local width,height = 60,60
local arcWidth, arcHeight = 10,10
canvas:attrColor("green")
canvas:drawRoundRect("fill",x,y,width,height,arcWidth, arcHeight)
canvas:flush()