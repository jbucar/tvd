-- Testea que no se vea nada cuando se setean valores RGBA inválidos

canvas:attrColor(0,0,0,256)
canvas:drawRect("fill",0,0,50,50)

canvas:attrColor(256,0,0,0)
canvas:drawRect("fill",60,0,50,50)

canvas:attrColor(0,256,0,0)
canvas:drawRect("fill",120,0,50,50)

canvas:attrColor(0,0,256,0)
canvas:drawRect("fill",180,0,50,50)

canvas:flush()