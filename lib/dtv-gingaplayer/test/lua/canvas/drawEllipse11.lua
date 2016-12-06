-- Testea #drawEllipse pasándole como parámetro un número negativo como ángulo de fin de la elipse
local xc,yc = 360,260
local width,height = 320,320
local ang_start, ang_end = 0,-350
canvas:attrColor("red")
canvas:drawEllipse("fill",xc,yc,width,height,ang_start,ang_end)
canvas:flush()