-- Testea que #clear coloree solo el canvas al que se le envi­a el mensaje y no el canvas principal
local x,y = 0,0
local width,height = 60,60
local subCanvas = canvas:new(width,height)
subCanvas:attrColor("blue")
subCanvas:drawRect("fill",x,y,width,height)
canvas:attrColor("white")
canvas:drawRect("fill",x,y,720,576)
subCanvas:attrColor("red")
subCanvas:clear()
canvas:compose(0,0,subCanvas)
canvas:flush()