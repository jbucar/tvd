-- Testea que no se #attrFlip no cambie el atributo flip del canvas principal
-- prueba horizontal = false, verical = true
local image = canvas:new("resources/lifia.png")
canvas:compose(0,0,image)
canvas:flush()

canvas:attrFlip(false,true)
canvas:flush()
