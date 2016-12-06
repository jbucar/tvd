-- Testea que #attrClip defina correctamente el área de clipping y esta sea respetada cuando se usar #compose
local subCanvas = canvas:new(500,500)
subCanvas:attrColor("red")
subCanvas:drawRect("fill",0,0,500,500)
canvas:attrClip(0,0,200,200)
canvas:compose(0,0,subCanvas)
canvas:flush()