-- Testea que #clear coloree una porción del área del canvas.
local x,y = 0,0
local width,height = 60,60
canvas:attrColor("white")
canvas:drawRect("fill",x,y,720,576)
canvas:attrColor("red")
canvas:drawRect("fill",x,y,width,height)
canvas:attrColor("green")
canvas:clear(x,y,width,height)
canvas:flush()