-- Testea que no se vea nada cuando se setean valores RGBA inválidos

canvas:attrColor("red")
canvas:clear()

canvas:attrColor("transparent")
canvas:clear()

canvas:attrColor("red")
canvas:clear(0,0,100,100)

canvas:flush()
