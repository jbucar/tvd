-- Testea que #drawLine funcione en otros canvas además del principal
-- Testea que #drawLine no dibuje una linea más allá de los límites del canvas en que opera.
local pathImage = "resources/cuadrado_rojo.jpg"
local canvasImage = canvas:new(pathImage)
local x1,y1 = 0,0
local x2,y2 = 200,200
canvasImage:drawLine(x1,y1,x2,y2)
canvas:compose(0,0,canvasImage)
canvas:flush()