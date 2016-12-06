-- Testea #compose() de una imagen con un canvas con transparencia
local canvasImage = canvas:new("resources/lifia.png")
local canvasTransparent = canvas:new(100,100)

canvasTransparent:attrColor("red")
canvasTransparent:clear()
canvasTransparent:attrColor("transparent")
canvasTransparent:clear(10,10,80,80)

canvasImage:compose(10,10,canvasTransparent)
canvas:compose(0,0,canvasImage)

canvas:flush()