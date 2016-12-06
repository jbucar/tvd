-- Testea #compose() de una imagen con un canvas con transparencia
local c1 = canvas:new("resources/lifia.png")
local c2 = canvas:new(100,100)
local c3 = canvas:new(100,100)
local c4 = canvas:new(100,100)
local c5 = canvas:new(160,160)

c2:attrColor("transparent")
c3:attrColor("transparent")
c4:attrColor("transparent")
c5:attrColor("transparent")

c2:clear();
c3:clear();
c4:clear();
c5:clear();

c2:attrColor("red")
c3:attrColor("green")
c4:attrColor("blue")
c5:attrColor("yellow")

c2:drawRect("frame",0,0,100,100)
c3:drawRoundRect("frame",0,0,100,100,10,10)
c4:drawEllipse("arc",50,50,98,98,0,360)
c5:drawLine(0,0,159,159)

canvas:compose(0,0,c1)
canvas:compose(20,20,c2)
canvas:compose(40,40,c3)
canvas:compose(60,60,c4)
canvas:compose(0,0,c5)

canvas:flush()