-- Testea el método #new creando un nuevo canvas cuyo atributo height es negativo
local canvasWidth = 100 --size in px
local canvasHeight = -100 --size in px
local newCanvas = canvas:new(canvasWidth,canvasHeight)
newCanvas:drawRect("frame",0,0,canvasWidth,canvasHeight)
canvas:compose(0,0,newCanvas)
canvas:flush()