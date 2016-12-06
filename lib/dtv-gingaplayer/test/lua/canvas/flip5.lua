-- Testea que no se #attrFlip no cambie el atributo flip del canvas principal
-- prueba ambos valores true
local image = canvas:new("resources/lifia.png")
canvas:compose(0,0,image)
canvas:flush()

canvas:attrFlip(true,true)
canvas:flush()