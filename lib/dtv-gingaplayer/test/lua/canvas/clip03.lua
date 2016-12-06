-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usa #drawText
canvas:attrColor("white")
canvas:drawRect("frame",100,100,310,310)
canvas:attrClip(100,100,310,310)

canvas:attrColor("red")
canvas:attrFont("Tiresias",20)
canvas:drawText(520,20,"Hola mundo !")
canvas:drawText(0,0,"Hola mundo !")
canvas:drawText(90,90,"Hola mundo !")
canvas:drawText(350,90,"Hola mundo !")
canvas:drawText(200,200,"Hola mundo !")
canvas:drawText(90,395,"Hola mundo !")
canvas:drawText(350,395,"Hola mundo !")

canvas:flush()
