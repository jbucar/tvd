-- Testea #drawLine tratando de dibujanr una linea con valores invalidos
local x1,y1 = 100,100
local x2,y2 = 60,-120
canvas:attrColor("red")
canvas:drawLine(x1,y1,x2,y2)
canvas:flush()