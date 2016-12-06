-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usa #drawRoundRect
canvas:attrClip(100,100,200,200)
canvas:attrColor("red")
canvas:drawRoundRect("fill",0,0,500,500,10,10)
canvas:flush()