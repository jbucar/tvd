-- Testea que el método #attrFont funcione cuándo sólo se le pasa como parámetro la fuente y el tamaño
local texto = "Fuente Tiresias "
canvas:attrFont("Tiresias",20)
canvas:attrColor("blue")
canvas:drawText(10,0,texto)
canvas:flush()