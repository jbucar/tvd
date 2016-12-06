-- Testea que #drawRect dibuje correctamente un cuadrado .
local x,y = 0,0
local width,height = 60,60
canvas:attrColor("red")
canvas:drawRect("fill",x,y,width,height)
canvas:flush()