-- Testea #attrClip cuando se le pasa como parámetro un punto y negativo
local subCanvas = canvas:new(500,500)
subCanvas:attrColor("red")
subCanvas:drawRect("fill",0,0,500,500)
canvas:attrClip(0,-100,200,200)
canvas:compose(0,0,subCanvas)
canvas:flush()