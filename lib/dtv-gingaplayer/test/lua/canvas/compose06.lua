-- Testea #compose intentando componer en el canvas principal de lua con el nuevo
local newCanvas = canvas:new("resources/lifia.png")
canvas:compose(0,0,newCanvas,20,20,108,108)
canvas:flush()