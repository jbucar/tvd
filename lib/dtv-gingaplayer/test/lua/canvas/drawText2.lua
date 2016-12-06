-- Testea #drawText sin setear antes attrFont
local texto = "Fuente indefinida "
canvas:attrColor("red")
canvas:drawText(10,0,texto)
canvas:flush()