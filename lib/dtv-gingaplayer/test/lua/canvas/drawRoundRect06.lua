-- Testea que #drawRoundRect no dibuje un cuadrado fuera de los límites del canvas
local pathImage = "resources/cuadrado_rojo.jpg"
local canvasImage = canvas:new(pathImage)
local x,y = 0,0
local width,height = 320,320
local arcWidth, arcHeight = 10,10
canvasImage:attrColor("green")
canvasImage:drawRoundRect("fill",x,y,width,height,arcWidth, arcHeight)
canvas:compose(0,0,canvasImage)
canvas:flush()