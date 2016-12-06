-- Testea que #attrScale no funcione sobre el canvas principal.
canvas:attrColor("red")
canvas:drawRect("fill",0,0,100,100)
canvas:flush()

canvas:attrScale(150,150)
canvas:flush()
