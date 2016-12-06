-- Testea que #drawEllipse no dibuje fuera de los límites del canvas.
local canvasImage = canvas:new(500,500)
local xc,yc = 0,0
local width,height = 320,320
local ang_start, ang_end = 0,360
canvasImage:attrColor("green")
canvasImage:drawEllipse("fill",xc,yc,width,height,ang_start,ang_end)
canvas:compose(200,200,canvasImage)
canvas:flush()
