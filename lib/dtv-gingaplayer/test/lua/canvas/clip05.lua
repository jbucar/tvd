-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usa #drawText
canvas:attrColor("white")
canvas:drawRect("frame",0,0,310,310)
canvas:attrClip(0,0,310,310)

canvas:attrColor("red")
clipX,clipY, clipW, clipH = canvas:attrClip()
canvas:attrFont("Tiresias",20)
canvas:drawText(10,0,"clip x = "..tostring(clipX))
canvas:drawText(10,20,"clip y = "..tostring(clipY))
canvas:drawText(10,40,"clip w = "..tostring(clipW))
canvas:drawText(10,60,"clip h = "..tostring(clipH))

canvas:flush()
