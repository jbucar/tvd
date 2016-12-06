-- Testea que cuándo no se le pase a #attrRotation ningún parámetro, este devuelva el valor de actual del atributo del canvas.
local myCanvas = canvas:new(200,200)
myCanvas:attrRotation(180)
myCanvas:attrFont("Verdana",20)
myCanvas:attrColor("blue")
local rotation = myCanvas:attrRotation()
myCanvas:drawText(10,0,rotation)
canvas:compose(0,0,myCanvas)
canvas:flush()