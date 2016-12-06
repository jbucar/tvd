-- Testea #attrClip cuando se le pasa como parámetro un height negativo
local subCanvas = canvas:new(500,500)
subCanvas:attrColor("red")
subCanvas:drawRect("fill",0,0,500,500)
canvas:attrClip(20,20,200,1200)
canvas:compose(0,0,subCanvas)
canvas:flush()