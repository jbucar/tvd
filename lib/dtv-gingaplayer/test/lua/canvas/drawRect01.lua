-- Testea #drawRect pasándole como mode un string inválido
local x,y = 10,20
local width,height = 60,60
canvas:attrColor("red")
canvas:drawRect("invalidValue",x,y,width,height)
canvas:flush()