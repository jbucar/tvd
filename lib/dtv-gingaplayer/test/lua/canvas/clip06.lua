-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usa #drawElipse
canvas:attrClip(100,100,310,310)
canvas:attrColor("red")
canvas:drawEllipse("fill",160,160,320,320, 0,360)
canvas:flush()