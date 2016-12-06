-- Testea #new creando un nuevo canvas determinando el ancho y el alto del mismo .
-- El nuevo canvas debería ser un cuadrado rojo
canvas:compose(0,0,canvas:new("resources/lifia.png"))
local canvasWidth = 100 --size in px
local canvasHeight = 100 --size in px
local newCanvas = canvas:new(canvasWidth,canvasHeight)
newCanvas:attrColor("red")
newCanvas:clear()
canvas:compose(130,130,newCanvas)
canvas:flush()