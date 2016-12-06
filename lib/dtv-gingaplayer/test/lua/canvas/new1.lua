-- Testea #new() para crear un nuevo canvas a partir de una imagen.
-- Testea #compose() para componer el canvas principal de lua con el nuevo canvas y así mostrar la imagen.
local pathImage = "resources/cuadrado_rojo.jpg"
local canvasImage = canvas:new(pathImage)
canvas:compose(0,0,canvasImage)
canvas:flush()