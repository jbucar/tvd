-- Testea #drawRect pas�ndole como par�metro un n�mero negativo en la coordenada x
local x,y = -10,20
local width,height = 60,60
canvas:attrColor("red")
canvas:drawRect("frame",x,y,width,height)
canvas:flush()