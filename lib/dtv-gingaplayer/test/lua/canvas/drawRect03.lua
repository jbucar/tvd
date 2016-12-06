--  Muestra un cuadrado rojo 100x100 pixels creando un nuevo canvas, rellenándolo con el color rojo y componiéndolo al canvas principal.
local canvasWidth = 100 --size in px
local canvasHeight = 100 --size in px
local newCanvas = canvas:new(canvasWidth,canvasHeight)
newCanvas:attrColor("red")
newCanvas:drawRect("fill",0,0,canvasWidth,canvasHeight)
canvas:compose(0,0,newCanvas)
canvas:flush()