-- Testea que #compose funcione correctamente con canvas anidados,
local subCanvas = canvas:new("resources/lifia.png")
local subSubCanvas = canvas:new(50,50)
subSubCanvas:drawRect("fill",0,0,50,50)
subCanvas:compose(0,0,subSubCanvas)
canvas:compose(0,0,subCanvas)
canvas:flush()
