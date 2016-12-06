--  Muestra una imagen a partir de la posición (0,0) de 108x108 pixels.
-- Testea #compose pasándole sólo algunos de los parámetros opcionales.
local newCanvas = canvas:new("resources/lifia.png")
canvas:compose(0,0,newCanvas,0,0,128,128)
canvas:flush()