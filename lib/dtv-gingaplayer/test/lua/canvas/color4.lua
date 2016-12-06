-- Testea que #attrColor funcione correctamente con parámetros RGBA válidos.

--BLACK
canvas:attrColor(0,0,0,0)
canvas:drawRect("fill",0,0,50,50)

canvas:attrColor(0,0,0,120)
canvas:drawRect("fill",60,0,50,50)

canvas:attrColor(0,0,0,255)
canvas:drawRect("fill",120,0,50,50)

--RED
canvas:attrColor(255,0,0,0)
canvas:drawRect("fill",0,60,50,50)

canvas:attrColor(255,0,0,120)
canvas:drawRect("fill",60,60,50,50)

canvas:attrColor(255,0,0,255)
canvas:drawRect("fill",120,60,50,50)

--GREEN
canvas:attrColor(0,255,0,0)
canvas:drawRect("fill",0,120,50,50)

canvas:attrColor(0,255,0,120)
canvas:drawRect("fill",60,120,50,50)

canvas:attrColor(0,255,0,255)
canvas:drawRect("fill",120,120,50,50)

--BLUE
canvas:attrColor(0,0,255,0)
canvas:drawRect("fill",0,180,50,50)

canvas:attrColor(0,0,255,120)
canvas:drawRect("fill",60,180,50,50)

canvas:attrColor(0,0,255,255)
canvas:drawRect("fill",120,180,50,50)

--WHITE
canvas:attrColor(255,255,255,0)
canvas:drawRect("fill",0,240,50,50)

canvas:attrColor(255,255,255,120)
canvas:drawRect("fill",60,240,50,50)

canvas:attrColor(255,255,255,255)
canvas:drawRect("fill",120,240,50,50)

canvas:flush()