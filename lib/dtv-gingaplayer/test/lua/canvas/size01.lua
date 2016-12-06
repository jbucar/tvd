-- Testea que #attrSize funcione correctamente
local canvasW,canvasH = canvas:attrSize()
canvas:attrColor("red")
canvas:attrFont("Tiresias",16,"bold")
canvas:drawText(10,0,tostring(canvasW).."x"..tostring(canvasH))

local subCanvas = canvas:new(200,300)
canvasW,canvasH = subCanvas:attrSize()
canvas:drawText(10,30,tostring(canvasW).."x"..tostring(canvasH))
canvas:flush()