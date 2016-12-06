-- Testea #compose() intentando componer un nuevo canvas en una posición inválida ( “x” negativo)
local pathImage = "resources/cuadrado_rojo.jpg"
local canvasImage = canvas:new(pathImage)
canvas:compose(-50,0,canvasImage)
canvas:flush()