-- Testea #drawEllipse pasándole como mode un string inválido
local xc,yc = 360,260
local width,height = 320,320
local ang_start, ang_end = 0,360
canvas:attrColor("red")
canvas:drawEllipse("invalidValue",xc,yc,width,height,ang_start,ang_end)
canvas:flush()