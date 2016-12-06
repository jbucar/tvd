-- Testea que #drawRoundRect dibuje correctamente un recuadro de color rojo
local x,y = 0,0
local width,height = 60,60
local arcWidth, arcHeight = 10,10
canvas:attrColor("red")
canvas:drawRoundRect("frame",x,y,width,height,arcWidth, arcHeight)
canvas:flush()