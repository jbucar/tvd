-- Testea #compose() intentando componer un nuevo canvas en una posición fuera de los límites del canvas
local pathImage = "resources/cuadrado_rojo.jpg"
local canvasImage = canvas:new(pathImage)
canvas:compose(1000000,0,canvasImage)
canvas:flush()