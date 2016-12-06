-- Testea que no se dibuje nada si no se hace un #flush
canvas:attrColor("red")
canvas:drawRect("fill",0,0,200,200)
canvas:attrColor("blue")
canvas:drawRect("frame",50,50,100,100)
canvas:attrColor("green")
canvas:drawText(10,10, "Hello, World!!!")

-- canvas:flush()
