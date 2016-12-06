-- Testea #attrScale con un width negativo
local subCanvas = canvas:new(720,560)
subCanvas:attrColor("red")
subCanvas:drawRect("fill",0,0,700,540)
subCanvas:attrScale(-72,56)
canvas:compose(0,0,subCanvas)
canvas:flush()