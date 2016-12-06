-- Testea #drawRect pasándole como mode un string vacío
local x,y = 10,20
local width,height = 60,60
canvas:attrColor("red")
canvas:drawRect("",x,y,width,height)
canvas:flush()