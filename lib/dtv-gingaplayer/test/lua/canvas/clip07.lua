-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usa #drawLine
canvas:attrClip(100,100,200,200)
canvas:attrColor("red")
canvas:drawLine(100,100,600,600)
canvas:flush()