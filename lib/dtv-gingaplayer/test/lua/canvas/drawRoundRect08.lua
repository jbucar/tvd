-- Testea #drawRoundRect pasándole como parámetro un número negativo en la coordenada y
local x,y = 10,-20
local width,height = 60,60
local arcWidth, arcHeight = 10,10
canvas:attrColor("red")
canvas:drawRoundRect("frame",x,y,width,height,arcWidth, arcHeight)
canvas:flush()
